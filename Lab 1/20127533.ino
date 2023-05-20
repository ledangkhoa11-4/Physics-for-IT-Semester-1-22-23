// C++ code
//
int btn = 2;
int ledPort[6] = {3,4,5,6,7,8};
int length = sizeof(ledPort) / sizeof(ledPort[0]);

void lowAll(int port[], int n){
  for(int i = 0; i< n ; i++){
    digitalWrite(port[i],LOW);
  }
}
void highAll(int port[], int n){
  for(int i = 0; i< n ; i++){
    digitalWrite(port[i],HIGH);
  }
}
void setup()
{
  Serial.begin(9600);
  pinMode(btn, INPUT);
  
  for(int i=0;i<length;i++){
    pinMode(ledPort[i], OUTPUT);
  }
}

void loop()
{
  int idx = 0;
  unsigned long startPress = millis();
  while(digitalRead(btn)==HIGH){
    digitalWrite(ledPort[idx],HIGH);
    if(idx == 0)
      digitalWrite(ledPort[length-1],LOW);
    else
      digitalWrite(ledPort[idx-1],LOW);
    idx++;
    if(idx == length)
      idx = 0;
    //Dùng millis để có thể dừng ngay khi thả nút
    unsigned long startDelay = millis();
      while(digitalRead(btn)==HIGH){
        unsigned long current = millis();
        if(current - startDelay >= 1000)
          	break;
      }
   	unsigned long current = millis();
    if(current - startPress >= 12000){
      //Bật tất cả đèn và đợi cho đến khi thả nút
      highAll(ledPort,length);
      while(digitalRead(btn)==HIGH);
    }
  }
	//Tắt tất cả đèn
  lowAll(ledPort, length);
}