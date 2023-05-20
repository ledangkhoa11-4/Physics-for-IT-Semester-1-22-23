#include <LiquidCrystal.h>

int tmp_port = A0;

LiquidCrystal lcd_1(12, 11, 5, 4, 3, 2);

void setup()
{
  lcd_1.begin(16, 2); 
  pinMode(tmp_port, INPUT);
}

void loop()
{
  double analogValue = analogRead(tmp_port);
  double voltage = (analogValue *  5 )/1024;
  int tempAtC = (voltage / 0.01 ) - 50;
  int tempAtF = (tempAtC * 1.8 + 32);
  
  String resC = String("Nhiet Do: ") + String(tempAtC) + String((char)178) + String("C   ");
  String resF = String("Nhiet Do: ") + String(tempAtF) + String((char)178) + String("F   ");
  
  lcd_1.setCursor(0, 0);
  lcd_1.print(resC);
  lcd_1.setCursor(0, 1);
  lcd_1.print(resF);
  
}


