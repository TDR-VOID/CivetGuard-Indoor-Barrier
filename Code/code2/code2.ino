#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "2263081slt"
#define WIFI_PASSWORD "199202FJ5"

// Insert Firebase project API Key
#define API_KEY "AIzaSyD_yCl3ArVFH1pKArFTmTgOgeM7iwvL5AM"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://indoor-barrier-default-rtdb.firebaseio.com" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int intValue;
int floatValue;
bool signupOK = false;
int PIR_IN = D1; // PIR sensor
int LED = D0; // LED
int outputPin = D2; // Relay
int wifiConnectAttempts = 0;
const int maxWifiConnectAttempts = 5; // Maximum attempts to connect to Wi-Fi

void setup()
{
  Serial.begin(9600);
  pinMode(PIR_IN, INPUT);
  pinMode(outputPin, OUTPUT);
  pinMode(LED, OUTPUT);

  Serial.begin(115200);
}

void loop()
{
  // Connect to Wi-Fi
  if (WiFi.status() != WL_CONNECTED && wifiConnectAttempts < maxWifiConnectAttempts) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
    wifiConnectAttempts++;
  }

  // Proceed with Firebase setup and operations if Wi-Fi is connected
  if (WiFi.status() == WL_CONNECTED) {
    // Firebase setup
    if (!signupOK) {
      /* Assign the api key (required) */
      config.api_key = API_KEY;

      /* Assign the RTDB URL (required) */
      config.database_url = DATABASE_URL;

      /* Sign up */
      if (Firebase.signUp(&config, &auth, "", "")) {
        Serial.println("ok");
        signupOK = true;
      }
      else {
        Serial.printf("%s\n", config.signer.signupError.message.c_str());
      }

      /* Assign the callback function for the long running token generation task */
      config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

      Firebase.begin(&config, &auth);
      Firebase.reconnectWiFi(true);
    }

    // Firebase operations
    if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
      sendDataPrevMillis = millis();
      if (Firebase.RTDB.getInt(&fbdo, "/Value/PIR")) {
        if (fbdo.dataType() == "int") {
          intValue = fbdo.intData();
          Serial.println(intValue);
        }
      }
      else {
        Serial.println(fbdo.errorReason());
      }
      
      if (Firebase.RTDB.getFloat(&fbdo, "/Value/LDR")) {
        if (fbdo.dataType() == "int") {
          floatValue = fbdo.floatData();
          Serial.println(floatValue);
        }
      }
      else {
        Serial.println(fbdo.errorReason());
      }
    }
  }

  // Your other code here
  
  int pinread = digitalRead(PIR_IN);

  if (pinread == HIGH){
    Serial.println("Motion Detected");
    digitalWrite(outputPin, HIGH);
    delay(5000);
    digitalWrite(outputPin, LOW);
    delay(1000);
  }
  else {
    Serial.println("No Motion");
    digitalWrite(outputPin, LOW);
    blinkLED();
  }

  if (intValue == 1){
    digitalWrite(outputPin, HIGH);
    delay(10000);
  }
}

void blinkLED(){
  digitalWrite(LED,HIGH);
  delay(500);
  digitalWrite(LED,LOW);
  delay(500);
}
