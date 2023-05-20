// C++ code
//
int trig_pin = 2;
int echo_pin = 3;

int buzzer_pin = 5;

int blue_pin = 7;
int red_pin = 8;
int orange_pin = 9;

void setup()
{
  	Serial.begin(9600);
	pinMode(trig_pin, OUTPUT);
  	pinMode(echo_pin, INPUT);
  
  	pinMode(buzzer_pin, OUTPUT);
  
  	pinMode(blue_pin, OUTPUT);
  	pinMode(red_pin, OUTPUT);
  	pinMode(orange_pin, OUTPUT);
}

long getDistance(){
 	digitalWrite(trig_pin, LOW);
 	delayMicroseconds(2);
  	digitalWrite(trig_pin, HIGH);
  	delayMicroseconds(10);
  	digitalWrite(trig_pin, LOW);
  
  	long duration = pulseIn(echo_pin, HIGH);
  	long distance = ceil(0.0343 * duration/2.0);
  	return distance;
}

void loop()
{
  int distance = getDistance();
  if(distance >= 150){
    digitalWrite(blue_pin, HIGH);
    digitalWrite(red_pin, LOW);
    digitalWrite(orange_pin, LOW);    
    noTone(buzzer_pin);
    delay(100);
  }
 
  if(distance > 50 && distance < 150){
    digitalWrite(blue_pin, LOW);
    digitalWrite(red_pin, HIGH);
    digitalWrite(orange_pin, LOW);
	tone(buzzer_pin, 350,500);
    delay(500);
    digitalWrite(red_pin, LOW);
    delay(500);
  }
  if (distance <= 50){
    digitalWrite(blue_pin, HIGH);
    digitalWrite(red_pin, HIGH);
    digitalWrite(orange_pin, HIGH);
    tone(buzzer_pin, 350, distance*10);
    delay(distance*10);
    digitalWrite(blue_pin, LOW);
    digitalWrite(red_pin, LOW);
    digitalWrite(orange_pin, LOW);
    delay(distance*10);
  }
}
