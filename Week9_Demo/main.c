#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_MCP23X17.h>
#include <TM1637Display.h>
#include <TFT_eSPI.h>
#include <FastLED.h>
#include <WiFi.h>
#include <WebServer.h>
#include <SPI.h>
#include <MFRC522.h>

const char* ssid = "Ash";
const char* password = "244466666";
WebServer server(80);


TFT_eSPI tft = TFT_eSPI();

Adafruit_MCP23X17 mcp;
const char keys1[4][4] = {
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};

const char* keys2[4][4] = {
{"A1","A2","A3","A4"},
{"B1","B2","B3","B4"},
{"C1","C2","C3","C4"},
{"D1","D2","D3","D4"}
};

unsigned long lastKeypadTime = 0;
const unsigned long keypadDebounce = 200;

#define LED_PIN 4
#define NUM_LEDS 16
CRGB leds[NUM_LEDS];

#define IN1 18
#define IN2 8
#define IN3 3
#define IN4 9

const uint8_t halfStepSeq[8][4] = {
{1,0,0,0},{1,1,0,0},{0,1,0,0},{0,1,1,0},
{0,0,1,0},{0,0,1,1},{0,0,0,1},{1,0,0,1}
};

int stepIndex = 0;
int stepsToMove = 0;
unsigned long lastStepTime = 0;
const unsigned long stepDelay = 10;

const int BUZZER_PIN = 17;
const int BUZZER_CHANNEL = 0;
const int PWM_RESOLUTION = 8;
const int PWM_MAX = 127;

const int TONE_RIGHT = 1000;
const int TONE_WRONG = 400;

const int RED_PIN = 37;
const int GREEN_PIN = 38;
const int BLUE_PIN = 39;

#define CLK 13
#define DIO 14

TM1637Display display(CLK,DIO);

uint8_t displayData[4];
unsigned long lastSecondTick = 0;
const unsigned long interval = 1000;
int timerSeconds = 0;

#define RST_PIN 35
#define SS_PIN 20
MFRC522 rfid(SS_PIN, RST_PIN);

void playTone(int freq,int duration_ms){
  ledcWriteTone(BUZZER_CHANNEL,freq);
  ledcWrite(BUZZER_CHANNEL,PWM_MAX);
  delay(duration_ms);
  ledcWrite(BUZZER_CHANNEL,0);
}

void playRight(){ playTone(TONE_RIGHT,200); }
void playWrong(){ playTone(TONE_WRONG,400); }

void setRGB(int r,int g,int b){
  analogWrite(RED_PIN,255-r);
  analogWrite(GREEN_PIN,255-g);
  analogWrite(BLUE_PIN,255-b);
}

void setLEDMatrix(CRGB color){
  for(int i=0;i<NUM_LEDS;i++) leds[i]=color;
  FastLED.show();
}

void stepMotorOnce(int dir){
  stepIndex=(stepIndex+dir+8)%8;

  digitalWrite(IN1,halfStepSeq[stepIndex][0]);
  digitalWrite(IN2,halfStepSeq[stepIndex][1]);
  digitalWrite(IN3,halfStepSeq[stepIndex][2]);
  digitalWrite(IN4,halfStepSeq[stepIndex][3]);
}

void updateTM1637(){
  int m=timerSeconds/60;
  int s=timerSeconds%60;

  displayData[0]=display.encodeDigit(m/10);
  displayData[1]=display.encodeDigit(m%10)|0x80;
  displayData[2]=display.encodeDigit(s/10);
  displayData[3]=display.encodeDigit(s%10);

  display.setSegments(displayData);
}

void startTimer(int seconds){
  timerSeconds=seconds;
  lastSecondTick=millis();
}


void handleRoot(){
  server.send(200,"text/html",R"rawliteral(
<html>
<body>
<h2>ESP32 Dashboard</h2>

<h3>Stepper Motor</h3>
<button onclick="fetch('/stepper?dir=1')">Forward</button>
<button onclick="fetch('/stepper?dir=-1')">Backward</button>

<h3>LED Matrix</h3>
<button onclick="fetch('/led?c=FF0000')">Red</button>
<button onclick="fetch('/led?c=00FF00')">Green</button>
<button onclick="fetch('/led?c=0000FF')">Blue</button>
<button onclick="fetch('/led?c=000000')">Off</button>

</body>
</html>
)rawliteral");
}

void handleStepper(){
  stepsToMove += server.arg("dir").toInt()*50;
  server.send(200,"text/plain","OK");
}

void handleLED(){
  long c=strtol(server.arg("c").c_str(),nullptr,16);
  setLEDMatrix(CRGB((c>>16)&0xFF,(c>>8)&0xFF,c&0xFF));
  server.send(200,"text/plain","OK");
}


void scanKeypads(){

  unsigned long now=millis();
  if(now-lastKeypadTime<keypadDebounce) return;

  for(int row=0;row<4;row++){

    for(int i=0;i<4;i++) mcp.digitalWrite(i,HIGH);

    mcp.digitalWrite(row,LOW);

    for(int col=0;col<4;col++){

      if(mcp.digitalRead(col+4)==LOW){

        char key = keys1[row][col];

        Serial.print("Keypad1: ");
        Serial.println(key);

        tft.fillRect(10,100,220,40,TFT_BLACK);
        tft.setCursor(10,100);
        tft.setTextColor(TFT_GREEN);
        tft.setTextSize(3);
        tft.print("Key: ");
        tft.println(key);

        lastKeypadTime=now;
        return;
      }
    }
  }

  for(int row=0;row<4;row++){

    for(int i=8;i<12;i++) mcp.digitalWrite(i,HIGH);

    mcp.digitalWrite(row+8,LOW);

    for(int col=0;col<4;col++){

      if(mcp.digitalRead(col+12)==LOW){

        const char* key = keys2[row][col];

        Serial.print("Keypad2: ");
        Serial.println(key);

        tft.fillRect(10,150,220,40,TFT_BLACK);
        tft.setCursor(10,150);
        tft.setTextColor(TFT_CYAN);
        tft.setTextSize(3);
        tft.print("Key: ");
        tft.println(key);

        lastKeypadTime=now;
        return;
      }
    }
  }
}

void setup(){

  Serial.begin(115200);

  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);

  pinMode(BUZZER_PIN,OUTPUT);
  ledcSetup(BUZZER_CHANNEL,TONE_RIGHT,PWM_RESOLUTION);
  ledcAttachPin(BUZZER_PIN,BUZZER_CHANNEL);

  pinMode(RED_PIN,OUTPUT);
  pinMode(GREEN_PIN,OUTPUT);
  pinMode(BLUE_PIN,OUTPUT);

  setRGB(0,0,0);

  FastLED.addLeds<WS2812B,LED_PIN,GRB>(leds,NUM_LEDS);
  FastLED.setBrightness(80);
  setLEDMatrix(CRGB::Black);

  Wire.begin(11,12);

  if(!mcp.begin_I2C(0x20)){
    Serial.println("MCP NOT FOUND!");
    while(1);
  }

  for(int i=0;i<4;i++){
    mcp.pinMode(i,OUTPUT);
    mcp.digitalWrite(i,HIGH);
  }

  for(int i=4;i<8;i++) mcp.pinMode(i,INPUT_PULLUP);

  for(int i=8;i<12;i++){
    mcp.pinMode(i,OUTPUT);
    mcp.digitalWrite(i,HIGH);
  }

  for(int i=12;i<16;i++) mcp.pinMode(i,INPUT_PULLUP);

  display.setBrightness(2);
  startTimer(0);

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(2);
  tft.setCursor(10,10);
  tft.println("Tap card to start");

  SPI.begin(21,45,47,20);
  rfid.PCD_Init();

  WiFi.begin(ssid,password);

  Serial.print("Connecting WiFi");

  unsigned long start=millis();

  while(WiFi.status()!=WL_CONNECTED && millis()-start<30000){
    Serial.print(".");
    delay(500);
  }

  if(WiFi.status()==WL_CONNECTED){
    Serial.println("\nWiFi connected!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
  }

  server.on("/",handleRoot);
  server.on("/stepper",handleStepper);
  server.on("/led",handleLED);

  server.begin();
}

void loop(){

  server.handleClient();
  scanKeypads();

  if(stepsToMove!=0){
    unsigned long now=millis();
    if(now-lastStepTime>=stepDelay){
      lastStepTime=now;
      int dir=(stepsToMove>0)?1:-1;
      stepMotorOnce(dir);
      stepsToMove-=dir;
    }
  }

  if(rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()){

    playRight();
    setRGB(0,255,0);
    startTimer(30);

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }

  unsigned long now=millis();

  if(timerSeconds>0 && now-lastSecondTick>=interval){
    lastSecondTick=now;
    timerSeconds--;
    updateTM1637();

    tft.fillRect(10,40,200,40,TFT_BLACK);
    tft.setCursor(10,40);
    tft.setTextColor(TFT_YELLOW);
    tft.setTextSize(3);
    tft.print("Time: ");
    tft.println(timerSeconds);

    setRGB(0,0,255);

    if(timerSeconds==0){
      setRGB(255,0,0);
      for(int i=0;i<3;i++){
        playWrong();
        delay(300);
      }
    }
  }
}
