// C++ code
//
int photoResistor = A0;
int led = 3;
void setup()
{
  pinMode(photoResistor, INPUT);
  pinMode(led, OUTPUT);
}

void loop()
{
  float value = analogRead(photoResistor);
  Serial.println(value);
  if(value < 500)
    digitalWrite(led, HIGH);
  else
    digitalWrite(led, LOW);
}