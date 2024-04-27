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
#define WIFI_SSID "********"
#define WIFI_PASSWORD "********"

// Insert Firebase project API Key
#define API_KEY "***********************"
// Insert RTDB URLefine the RTDB URL 
#define DATABASE_URL "***********************" 

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

int PIR_IN = D1; // PIR sensor pin
int LED = D0; // LED
int outputPin = D2; // Relay pin 
int LdrPin = A0; // Analog pin connected to LDR
int ldrValue = 0; // Variable to store LDR value
unsigned long sendDataPrevMillis = 0;
int motioncount = 0;
bool signupOK = false;


void setup(){
  Serial.begin(9600);
  pinMode(PIR_IN, INPUT);
  pinMode(outputPin, OUTPUT);
  pinMode(LED, OUTPUT);

  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }

  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;
  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("Firebase sign up successful");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop()
{
  int motionDetected = digitalRead(PIR_IN);
  ldrValue = analogRead(LdrPin);

  if (motionDetected == HIGH){
    Serial.println("Moton Detected");
    digitalWrite(outputPin, HIGH);
    delay(5000);
    motioncount++;
      if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    if (Firebase.RTDB.setInt(&fbdo, "Value/PIR Count", motioncount)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    
    if (Firebase.RTDB.setFloat(&fbdo, "Value/LDR", ldrValue)){
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
    } else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }

    digitalWrite(outputPin, LOW);
    delay(1000);
  } else {
    Serial.println("No Motion");
    digitalWrite(outputPin, LOW);
    blinkLED();
    }
}


void blinkLED(){
  digitalWrite(LED,HIGH);
  delay(500);
  digitalWrite(LED,LOW);
  delay(500);
  }

  

/*
Credits for Firebase Realtime Database connection code:
Database connection code adapted from Rui Santos's tutorial on RandomNerdTutorials.com:
ESP8266: https://RandomNerdTutorials.com/esp8266-nodemcu-firebase-realtime-database/
*/
