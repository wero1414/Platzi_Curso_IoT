#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <DHT.h>
#include <BlynkSimpleEsp32.h>

#define INTPIN 4
#define LED 16
#define DHTPIN 23

char token[]="83c96384acdf4451aa5adce70574101c";

DHT dht(DHTPIN,DHT22);
BlynkTimer timer;
WidgetLED led1(V1);

bool ext_intFlag = 0;
bool pinChanged = 0;

BLYNK_WRITE(V0){
  int V0Data = param.asInt();
  //.asString
  //.asDouble
  Serial.print("Valor de V0 = ");
  Serial.println(V0Data);
  digitalWrite(16,V0Data);
  }

void sendSensor(){
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  
  Blynk.virtualWrite(V5,h);
  Blynk.virtualWrite(V6,t);
  }

void checkPin(){
  ext_intFlag = !ext_intFlag;
  pinChanged = true;
  }

void setup() {
  Serial.begin(115200);
  pinMode(INTPIN,INPUT_PULLUP);
  pinMode(LED,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(INTPIN),checkPin,RISING);
  digitalWrite(LED,LOW);
  dht.begin();
  timer.setInterval(2000L,sendSensor);
  Blynk.begin(token,"Wifi_prueba","123456789");
}

void loop() {
  Blynk.run();
  timer.run();
  if(pinChanged){
    if(ext_intFlag){
      led1.on();
      }
      else{
        led1.off();
        }
    pinChanged=0;
    }
    
}
