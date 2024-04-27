int PIR_IN = D1; // PIR sensor input pin
int LED = D0; // LED 
int outputPin = D2; // Relay pin


void setup()
{
  Serial.begin(9600);
  pinMode(PIR_IN, INPUT);
  pinMode(outputPin, OUTPUT);
  pinMode(LED, OUTPUT);

}

void loop(){
  int pinread = digitalRead(PIR_IN);

  if (pinread == HIGH){
    Serial.println("Moton Detected");
    digitalWrite(outputPin, HIGH);
    delay(5000);
    digitalWrite(outputPin, LOW);
    delay(1000);
  } else  {
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


