// C++ code
//
#include <Adafruit_LiquidCrystal.h>

int screen = 1;
int tempPort = A0;
unsigned long lastExecutedMillis_1 = 0; 

Adafruit_LiquidCrystal lcd_1(0);

void setup()
{
  lcd_1.begin(16, 2);
  Serial.begin(9600);
  pinMode(2, INPUT); 
  pinMode(tempPort, INPUT);

  printLed("CLASS","VATLY 4IT-20CLC2", true);
 
}
void printLed(String line1, String line2, bool isClear){
  if(isClear)
  	lcd_1.clear();
  unsigned int length1 = line1.length();
  unsigned int length2 = line2.length();
  
  lcd_1.setCursor(8-length1/2,0);
  lcd_1.print(line1);
  lcd_1.setCursor(8-length2/2,1);
  lcd_1.print(line2);
}
float getTemp(){
  float value = analogRead(tempPort);
  float voltage = value * 5 / 1023;
  return voltage/0.01 - 50;
}
void loop()
{
  
  while(digitalRead(2) == HIGH){
    while(digitalRead(2) == HIGH){
     if (screen == 2){ //dang o screen 1
     	unsigned long start = millis();
        lcd_1.noDisplay();
     	while(millis()-start < 300 && digitalRead(2) == HIGH);
     	lcd_1.display();
     	start = millis();
     	while(millis()-start < 300 && digitalRead(2) == HIGH);
     	
  }
    }
    if(screen == 0)
      printLed("CLASS","VATLY 4IT-20CLC2", true);
    if(screen == 1)
      printLed("Le Dang Khoa","20127533", true);
    if (screen == 2){
      String temp = String(getTemp());
      String res = String(" TEMP: ") + temp + String(" ");
      printLed(res,"", true);
    }
    screen++;
    screen %= 3;
  }
  
  if (screen == 0){ //dang o screen 2
    unsigned long current = millis();
    if(current - lastExecutedMillis_1 >= 700){
      	lastExecutedMillis_1 = millis();
        String temp = String(getTemp());
    	String res = String(" TEMP: ") + temp  + String(" ");
   		printLed(res,"", false);
   		unsigned long current = millis();	
    }
  }
   if (screen == 2){ //dang o screen 1
     unsigned long start = millis();
     lcd_1.noDisplay();
     while(millis()-start < 300 && digitalRead(2) == LOW);
     lcd_1.display();
     start = millis();
     while(millis()-start < 300 && digitalRead(2) == LOW);
  }
}








