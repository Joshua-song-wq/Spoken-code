#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <Arduino.h>
#include "wifi_credentials.h"
#include "genstage.h"
#include "4digit.h"    // Shared timer
#include "1digit.h"    // Shared chances
#include "tft.h"       // success(), fail()
#include "rfid.h"
#include "buzzer.h"
#include "led.h"

// =====================================================
// GLOBAL OBJECTS
// =====================================================
WebServer server(80);          // HTTP server (port 80)
WebSocketsServer webSocket(81); // WebSocket server (port 81)

// =====================================================
// GLOBAL VARIABLES
// =====================================================
unsigned long lastSecondTick = 0;   // Timer tick (1 second)
unsigned long lastStatusTick = 0;   // Status broadcast timer

volatile bool gameEnded = false;    // Game end flag
bool clientConnected = false;       // WebSocket connection status
bool stagesPassed = false;          // All stages cleared
bool waitShown = false;             // UI state flags
bool connectedShown = false;

// =====================================================
// HTTP ROOT HANDLER (SERVES WEB DASHBOARD)
// =====================================================
void handleRoot() {
    String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<title>ESP32 Game Dashboard</title>
<style>
body {font-family:Arial; text-align:center; margin-top:20px; background:#f0f0f0;}
.stage {display:inline-block; width:50px; height:50px; margin:5px; border-radius:50%; background:red;}
#popup {position:fixed; top:20px; left:50%; transform:translateX(-50%);
padding:15px 30px; background:#333; color:#fff; border-radius:10px;
font-size:20px; display:none; opacity:0; transition: opacity 0.5s;}
.pass {background:green;}
.fail {background:red;}
</style>
</head>
<body>
<h2>ESP32 Game Dashboard</h2>

<div>
<div id="stage1" class="stage"></div>
<div id="stage2" class="stage"></div>
<div id="stage3" class="stage"></div>
<div id="stage4" class="stage"></div>
<div id="stage5" class="stage"></div>
<div>
  <p>Time Left: <span id="timeLeft">--</span> s</p>
  <p>Chances Left: <span id="chancesLeft">--</span></p>
  <p>Win Game: <span id="winGame">--</span></p>
</div>
</div>

<!-- Controls -->
<div style="margin-top:15px;">
  <input type="number" id="addMinutes" min="1" value="1" style="width:60px;">
  <button onclick="addTime()">Add Time (min)</button>
</div>

<div style="margin-top:15px;">
  <button onclick="triggerMotor()">Trigger Motor</button>
</div>

<div id="popup"></div>

<script>
// =====================================================
// WEBSOCKET CLIENT
// =====================================================
let ws=new WebSocket("ws://"+location.hostname+":81/");

ws.onopen = function() {
    console.log("WebSocket opened");
};

ws.onmessage=function(event){
    let data=JSON.parse(event.data);

    // Stage result updates
    if(data.type=="stage"){
        let el=document.getElementById("stage"+data.stage);
        if(data.pass){
            el.style.background="green";
            showPopup("Stage "+data.stage+" PASSED!", true);
        } else {
            showPopup("Stage "+data.stage+" FAILED!", false);
        }
    }

    // Image stage data
    if(data.type=="stage_data"){
        console.log("Stage data received:", data);
        alert("Stage " + data.stage + " image received! (check console)");
    }

    // Status updates
    if(data.type=="status"){
        document.getElementById("timeLeft").innerText = data.timeLeft;
        document.getElementById("chancesLeft").innerText = data.chancesLeft;
        document.getElementById("winGame").innerText = data.winGame ? "YES" : "NO";
    }
};

// Popup UI
function showPopup(msg,pass){
    let p=document.getElementById("popup");
    p.innerText=msg;
    p.className=pass?"pass":"fail";
    p.style.display="block";
    p.style.opacity=1;
    setTimeout(()=>{p.style.opacity=0; setTimeout(()=>p.style.display="none",500);},2000);
}

// Add time control
function addTime(){
    let minutes = parseInt(document.getElementById("addMinutes").value);

    if(isNaN(minutes) || minutes <= 0){
        alert("Enter a valid number!");
        return;
    }

    ws.send(JSON.stringify({
        type: "add_time",
        value: minutes * 60
    }));
}

// Motor trigger
function triggerMotor(){
    ws.send(JSON.stringify({ type: "motor" }));
}
</script>
</body>
</html>
)rawliteral";

    server.send(200, "text/html", html);
}

// =====================================================
// WEBSOCKET EVENT HANDLER
// =====================================================
void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {

    // Connection event
    if(type == WStype_CONNECTED) {
        Serial.println("WebSocket client connected");
        clientConnected = true;
        return;
    }

    if(type != WStype_TEXT) return;

    String msg = String((char*)payload);
    Serial.println("WS Received: " + msg);

    // ---------------- PARSE MESSAGE TYPE ----------------
    String msgType = "";
    int typePos = msg.indexOf("\"type\":\"");
    if(typePos >= 0){
        int start = typePos + 8;
        int end = msg.indexOf("\"", start);
        msgType = msg.substring(start, end);
    }

    // ---------------- PARSE STAGE ----------------
    int stage = -1;
    int stagePos = msg.indexOf("\"stage\":");
    if(stagePos >= 0){
        stage = msg.substring(stagePos + 8).toInt();
    }

    // =====================================================
    // HANDLE MOTOR COMMAND
    // =====================================================
    if(msgType == "motor") {
        webSocket.broadcastTXT("{\"type\":\"motor\"}");
        Serial.println("Motor command broadcasted");
        return;
    }

    // =====================================================
    // HANDLE ADD TIME COMMAND
    // =====================================================
    if(msgType == "add_time") {
        int valPos = msg.indexOf("\"value\":");
        int secondsToAdd = 0;

        if(valPos >= 0){
            secondsToAdd = msg.substring(valPos + 8).toInt();
        }

        timeLeft += secondsToAdd;

        Serial.printf("Added %d seconds. New timeLeft: %d\n", secondsToAdd, timeLeft);

        String statusMsg = "{\"type\":\"status\",\"timeLeft\":" + String(timeLeft) +
                           ",\"chancesLeft\":" + String(chancesLeft) +
                           ",\"winGame\":" + String(winGame ? "true" : "false") + "}";

        webSocket.broadcastTXT(statusMsg);
        return;
    }

    // =====================================================
    // HANDLE KEY INPUT (STAGE 1 & 2)
    // =====================================================
    if(msgType == "key"){
        char key = 0;
        int valPos = msg.indexOf("\"value\":\"");
        if(valPos >= 0){
            key = msg.charAt(valPos + 9);
        }

        if(stage == 1){
            if(handleStage1Input(key)){
                webSocket.broadcastTXT("{\"type\":\"stage\",\"stage\":1,\"pass\":true}");
            } else {
                webSocket.broadcastTXT("{\"type\":\"stage\",\"stage\":1,\"pass\":true}");
            }
        } 
        else if(stage == 2){
            if(handleStage2Input(key)){
                webSocket.broadcastTXT("{\"type\":\"stage\",\"stage\":2,\"pass\":true}");

                generateImageStage();

                String imgMsg = "{\"type\":\"stage_data\",\"stage\":3,\"correctIndex\":" +
                                String(correctIndex) +
                                ",\"image\":\"" + currentImage + "\"}";

                webSocket.broadcastTXT(imgMsg);

                Serial.println("Stage 3 image SENT");
                Serial.println(imgMsg);
            }
        }

        return;
    }

    // =====================================================
    // HANDLE ANSWERS (STAGE 3 ~ 5)
// =====================================================
    if(msgType == "answer"){
        int answer = -1;
        int ansPos = msg.indexOf("\"answer\":");
        if(ansPos >= 0){
            answer = msg.substring(ansPos + 9).toInt();
        }

        if(stage < 3 || stage > 5) return;

        Serial.printf("Stage %d answer received: %d\n", stage, answer);

        if(answer == correctIndex){
            ledCommand = LED_GREEN;
            buzzerCommand = BUZZ_RIGHT;
            delay(1000);
            ledCommand = LED_NONE;

            webSocket.broadcastTXT("{\"type\":\"stage\",\"stage\":" + String(stage) + ",\"pass\":true}");

            if(stage < 5){
                currentStage = stage + 1;
                generateImageStage();

                String imgMsg = "{\"type\":\"stage_data\",\"stage\":" +
                                String(currentStage) +
                                ",\"correctIndex\":" +
                                String(correctIndex) +
                                ",\"image\":\"" + currentImage + "\"}";

                webSocket.broadcastTXT(imgMsg);
            } else {
                stagesPassed = true;
                success();
            }

        } else {
            webSocket.broadcastTXT("{\"type\":\"stage\",\"stage\":" + String(stage) + ",\"pass\":false}");

            ledCommand = LED_RED;
            buzzerCommand = BUZZ_WRONG;
            delay(1000);
            ledCommand = LED_NONE;

            chancesLeft--;

            if(chancesLeft <= 0 && !stagesPassed){
                fail();
                gameEnded = true;
            }
        }
    }
}

// =====================================================
// SETUP FUNCTION
// =====================================================
void iotsetup() {
    Serial.begin(115200);

    // Static IP config
    IPAddress local_IP(192, 168, 137, 100);
    IPAddress gateway(192, 168, 137, 1);
    IPAddress subnet(255, 255, 255, 0);

    if(!WiFi.config(local_IP, gateway, subnet)){
        Serial.println("STA Failed to configure");
    }

    // Connect WiFi
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    while(WiFi.status() != WL_CONNECTED){
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nConnected!");
    Serial.println(WiFi.localIP());

    // Initialize game
    genStageSetup();
    generateStage1();

    // Start servers
    server.on("/", handleRoot);
    server.begin();

    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
}

// =====================================================
// MAIN LOOP
// =====================================================
void iotloop() {
    server.handleClient();
    webSocket.loop();

    // Connection UI states
    if(!clientConnected){
        if (!waitShown) { waitForNetwork(); waitShown = true; }
        return;
    } else if (!connectedShown) {
        connected();
        connectedShown = true;
    }

    // Game ended state
    if(gameEnded) {
        webSocket.broadcastTXT("{\"type\":\"status\",\"timeLeft\":" + String(timeLeft) +
                               ",\"chancesLeft\":" + String(chancesLeft) +
                               ",\"winGame\":" + String(winGame ? "true" : "false") + "}");
        return;
    }

    unsigned long now = millis();

    // ---------------- TIMER ----------------
    if(now - lastSecondTick >= 1000){
        lastSecondTick = now;

        if(timeLeft > 0) timeLeft--;

        if(timeLeft <= 0){
            fail();
            gameEnded = true;
            return;
        }
    }

    // ---------------- CHANCES ----------------
    if(chancesLeft <= 0){
        fail();
        gameEnded = true;
        return;
    }

    // ---------------- WIN ----------------
    if(winGame){
        win();
        gameEnded = true;
    }

    // ---------------- STATUS UPDATE ----------------
    if(now - lastStatusTick >= 500){
        lastStatusTick = now;

        webSocket.broadcastTXT("{\"type\":\"status\",\"timeLeft\":" + String(timeLeft) +
                               ",\"chancesLeft\":" + String(chancesLeft) +
                               ",\"winGame\":" + String(winGame ? "true" : "false") + "}");
    }
}