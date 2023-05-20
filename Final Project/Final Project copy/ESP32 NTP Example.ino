#include "DHTesp.h"
#include <LiquidCrystal_I2C.h>
#include <pthread.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* mqttServer = "broker.mqtt-dashboard.com"; 
int port = 1883;
WiFiClient espClient;
PubSubClient client(espClient);

bool isConnected = false;

const int DHT_PIN = 14;
const int OUTPUT_PIEZO_PIN = 12;
const int PIR = 13;
const int SOUND_DURATION_MS = 325;
const int photoRes = 27;
bool warning = false;

int threadholdTempMax = 50;
int threadholdTempMin = 15;
int threadholdHumidMax = 80;
int threadholdHumidMin = 40;

String suspiciousTemp = String("");
String suspiciousHumid = String("");
String suspiciousMotion = String("");

DHTesp dhtSensor;

LiquidCrystal_I2C lcd(0x27, 20, 4);


TempAndHumidity  data;
int Hrs, Mins;
int Secs = 0;
String AmPm;  // to store the string data to display 'AM' or 'PM'
int ampm;     // to store user input
int day, d, m, yr;
char *DAYS[] = {"Mon", "Tues", "Wed", "Thurs", "Fri", "Sat", "Sun"};
char *MONTHS[] = {"Jan", "Feb", "Mar", "Apr", "May", "June",
                  "July", "Aug", "Sep", "Oct", "Nov", "Dec"
                 };

void setup()
{
  Serial.begin(115200);
  pinMode(OUTPUT_PIEZO_PIN, OUTPUT);
  pinMode(PIR, INPUT);
  pinMode(photoRes, INPUT);

  wifiConnect();
  client.setServer(mqttServer, port);
  client.setCallback(callback);

  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  lcd.init();
  lcd.backlight();
  clkInit();



  pthread_t threads[7];
  pthread_create(&threads[0], NULL,clock, (void *)0);
  pthread_create(&threads[1], NULL,dht22, (void *)0);
  pthread_create(&threads[2], NULL,detectMotion, (void *)0);
  pthread_create(&threads[3], NULL,playingSound, (void *)0);
  pthread_create(&threads[4], NULL,handleConnection, (void *)0);
  pthread_create(&threads[5], NULL,handleSendInfo, (void *)0);
  pthread_create(&threads[6], NULL,handleSendAlert, (void *)0);
}
void loop()
{
  printlcd();
  delay(10);
}


///////////////////////////////////////
////Initialise the 'clock' settings////
///////////////////////////////////////
void clkInit() {
  day = 6;
  d = 1;
  m = 1;
  yr = 2022;
  AmPm = "AM";
  Hrs = 0;
  Mins = 0;
  lcd.setCursor(0, 0);
  String line1 = String(DAYS[day - 1]) + String(", ") + String(d) + String(" ") + String(MONTHS[m - 1])
    + String(" ") + String(yr);
  lcd.print(line1);
}


//////////////////////////////////////////////////////////////////////////
////To put an extra '0' infront of the variables on the Serial Monitor////
//////////////////////////////////////////////////////////////////////////
String precision2(int val)
{
  if (val < 10) 
    return String(0)+String(val);
  return String(val);
}


void printlcd(){
  lcd.clear();
  lcd.setCursor(0, 0);
  String line1 = String(DAYS[day - 1]) + String(", ") + precision2(d) + String(" ") + String(MONTHS[m - 1])
    + String(" ") + String(yr);
  lcd.print(line1);
 
  unsigned long per1 = millis();
  while(millis() - per1 < 5000){
    lcd.setCursor(0, 1);
    String line2 = String("Time: ") + precision2(Hrs) + String(":") + precision2(Mins) + String(":") + precision2(Secs) + String(AmPm);
    lcd.print(line2);
    if (Hrs == 12 && Mins == 00 && Secs == 00) {
      if (AmPm == "AM") {
        AmPm = "PM";
      }
      else {
        AmPm = "AM";
        d++;
        reset_dom(m); // reset the day to 1 when the days of the month exceed it's limit
        //...Reset LCD row 0 and update date...
        lcd.setCursor(0, 0);
        lcd.print("                    ");
        lcd.setCursor(0, 0);
        String line1 = String(DAYS[day - 1]) + String(", ") + precision2(d) + String(" ") + String(MONTHS[m - 1])
            + String(" ") + String(yr);
        lcd.print(line1);
        day++;
      }
    }
    delay(900);
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp: " + String(data.temperature, 2) + " C");
  lcd.setCursor(0,1);
  lcd.print("Humidity: " + String(data.humidity, 1) + "%");
  delay(1000);
  per1 = millis();
  while(millis() - per1 < 4000){
    lcd.setCursor(6,0);
    lcd.print(String(data.temperature, 2));
    lcd.setCursor(10, 1);
    lcd.print(String(data.humidity, 1));
    delay(100);
  }
}

void reset_dom(int var) {
  switch (var) {
    case 1:   // Jan - 31 days
      if (d == 32) {
        d = 1;
        m++;
      }
      break;
    case 2:   // Feb - 28 days
      if (d == 29) {
        d = 1;
        m++;
      }
      break;
    case 3:   // March - 31 days
      if (d == 32) {
        d = 1;
        m++;
      }
      break;
    case 4:   // April - 30 days
      if (d == 31) {
        d = 1;
        m++;
      }
      break;
    case 5:   // May - 31 days
      if (d == 32) {
        d = 1;
        m++;
      }
      break;
    case 6:   // June - 30 days
      if (d == 31) {
        d = 1;
        m++;
      }
      break;
    case 7:   // July - 31 days
      if (d == 32) {
        d = 1;
        m++;
      }
      break;
    case 8:   // August - 31 days
      if (d == 32) {
        d = 1;
        m++;
      }
      break;
    case 9:   // September - 30 days
      if (d == 31) {
        d = 1;
        m++;
      }
      break;
    case 10:  // October - 31 days
      if (d == 32) {
        d = 1;
        m++;
      }
      break;
    case 11:  // November - 30 days
      if (d == 31) {
        d = 1;
        m++;
      }
      break;
    case 12:  // December - 31 days
      if (d == 32) {
        d = 1;
        m++;
      }
      break;
  }
}

void *clock(void *start){
  //...Display Time to LCD...
  while(true){
    //...Clock starts...
    Secs++;
    if (Secs == 60) {
      Secs = 0;
      Mins++;
    }
    if (Mins == 60) {
      Mins = 0;
      Hrs++;
    }
    if (Hrs == 13) {
      Hrs = 1;
    }
    
    delay(900);  // delay acts as the clock second hand
  }
}

void *dht22(void *start){
  while(true){
    data = dhtSensor.getTempAndHumidity();
    if(data.temperature >= threadholdTempMax || data.temperature <= threadholdTempMin)
      suspiciousTemp = String("High temperature warning ") + String(data.temperature,2);
    else
      suspiciousTemp = String("");
    if(data.humidity >= threadholdHumidMax || data.humidity <= threadholdHumidMin)
      suspiciousHumid = String("High humidity warning ") + String(data.humidity,1);
    else
      suspiciousHumid= String("");
    delay(10);
  }
}
void *detectMotion(void *start){
  int lastState = false;
  int forceStop = false;
   while(true){
    while(digitalRead(photoRes) == HIGH){
      if(!lastState){
        Serial.println("Start detect motion");
        lastState = true;
      }
      unsigned long start = millis();
      while(digitalRead(PIR) == HIGH){
        if(forceStop){
          suspiciousMotion = String("");
          warning = false;
          delay(100);
          continue;
        }
        if(millis() - start >= 3000){
          suspiciousMotion = String("Motion detected warning");
          warning = true;
        }
        delay(100);
      }
      suspiciousMotion = String("");
      warning = false;
      delay(100);
    }
    if(lastState){
      Serial.println("Stop detect motion");
      lastState = false;
    }

    delay(100);
   }
}
void *playingSound(void *start){
  int lastState = false;
   while(true){
     while(warning == true){
        lastState = true;
        tone(OUTPUT_PIEZO_PIN, 800); 
        delay(SOUND_DURATION_MS);
        tone(OUTPUT_PIEZO_PIN, 500);   
        delay(SOUND_DURATION_MS);
     }
     if(lastState == true)
        noTone(OUTPUT_PIEZO_PIN);
     delay(100);
   }
}

void *handleConnection(void* start){
  while(true){
    if(!client.connected()) {
      isConnected = false;
      mqttReconnect();
    }
    isConnected = true;
    client.loop();
    delay(200);
    
  }

}
void *handleSendAlert(void* start){
  while(true){
    String lastNoti = "";
    while(isConnected == true){
      if(suspiciousMotion == "" && suspiciousTemp == "" && suspiciousHumid == ""){
      lastNoti = false;
      delay(200);
      continue;
      }
      char buffer[200];
      String msg = suspiciousTemp + String("|")+ suspiciousHumid + String("|") + suspiciousMotion;
      unsigned long start = millis();
      while(lastNoti == msg){
        msg = suspiciousTemp + String("|")+ suspiciousHumid + String("|") + suspiciousMotion;
        if(millis() - start >= 4000)
          break;
        delay(100);
      }
      if(msg == "||")
        continue;
      msg.toCharArray(buffer,200);
      client.publish("VL4IT20CLC04OUT/ALERT", buffer);
      lastNoti = msg;
      delay(200);
    }
    delay(300);
  }
}

void *handleSendInfo(void* start){
  while(true){
    while(isConnected == true){
      char buffer[50];
      String msg = String("Temp: ")  + String(data.temperature,2) + String("|") + String("Humid: ")  + String(data.humidity,2);
      msg.toCharArray(buffer,50);
      client.publish("VL4IT20CLC04OUT/INFO", buffer);
      delay(5000);
    }
    delay(300);
  }
}
/*MQTT Connected*/
void wifiConnect() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected!");
}

void mqttReconnect() {
  while(!client.connected()) {
    Serial.println("Attemping MQTT connection...");
    //***Change "123456789" by your student id***
    if(client.connect("533442596")) {
      Serial.println("connected");

      //***Subscribe all topic you need***
      client.subscribe("VL4IT20CLC04IN/Dat");
    }
    else {
      Serial.println("try again in 5 seconds");
      delay(5000);
    }
  }
}

//MQTT Receiver
void callback(char* topic, byte* message, unsigned int length) {
  Serial.println(topic);
  String strMsg;
  for(int i=0; i<length; i++) {
    strMsg += (char)message[i];
  }
  Serial.println(strMsg);
  //***Insert code here to control other devices***
}
