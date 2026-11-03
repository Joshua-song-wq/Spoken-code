#include <WiFi.h>
#include "wifi_credentials.h"

// ---------------- Setup ----------------

void wifisetup() {

    // Configure static IP
    WiFi.config(
        IPAddress(192, 168, 137, 101),
        IPAddress(192, 168, 137, 1),
        IPAddress(255, 255, 255, 0)
    );

    // Start WiFi connection
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait until connected
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("ESP101 Connected");
    Serial.println(WiFi.localIP());
}

// ---------------- Loop ----------------

void wifiloop() {
    // No repeated logic required
}