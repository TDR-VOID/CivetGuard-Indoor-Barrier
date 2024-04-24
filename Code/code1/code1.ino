int PIC_IN = D1;
int LED = 13;

const int outputPin = D2; 
void setup()
   {
  Serial.begin(9600);
   pinMode(PIC_IN, INPUT);
     pinMode(outputPin, OUTPUT);

  digitalWrite(LED, LOW);
   }

void loop()
{
  int val = digitalRead(PIC_IN);
   if (val == HIGH)
   {
  Serial.println("Moton detected");
  digitalWrite(LED, HIGH);
  digitalWrite(outputPin, HIGH);
  delay(5000);
  digitalWrite(outputPin, LOW);
  delay(1000);

   }
else
   {
    Serial.println("...........");
  digitalWrite(LED, LOW);
    }
  }