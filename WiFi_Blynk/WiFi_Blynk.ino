#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Tu tendras tu token de autorizacion en la aplicacion de Blynk "Auth Token".
// Para obtenerlo ve a onfiguracion de proyecto.
char auth[] = "560a4a918c5048e89c398d3d6b5f76b7";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "TihBeta2";
char pass[] = "1nv3nt0r3S";

#define DHTPIN 2         // What digital pin we're connected to
#define INTPIN 4

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11     // DHT 11
#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

WidgetLED led1(V1);

// We make these values volatile, as they are used in interrupt context
volatile bool pinChanged = false;
volatile int  ext_intFlag   = 0;



// Most boards won't send data to WiFi out of interrupt handler.
// We just store the value and process it in the main loop.
void checkPin(){
  // Invert state, since button is "Active LOW"
  ext_intFlag = !ext_intFlag;
  // Activamos la bandera de interrupcion
  pinChanged = true;
}


// This function sends Arduino's up time every second to Virtual Pin (5).
// In the app, Widget's reading frequency should be set to PUSH. This means
// that you define how often to send data to Blynk App.
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  // You can send any value at any time.
  // Please don't send more that 10 values per second.
  Serial.println("Humedad: "+String(h));
  Serial.println("Temperatura: "+String(t));
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
}

// This function will be called every time Slider Widget
// in Blynk app writes values to the Virtual Pin 1
BLYNK_WRITE(V0){
  int V0Data = param.asInt(); // Asignamos el valor que recibimos en V0 como entero.
  // Tambien puedes usar:
  // String i = param.asStr();
  // double d = param.asDouble();
  // Para distintos tipos de datos.
  // Serial.print("V0 value is: ");
  // Serial.println(V0Data);
  digitalWrite(16,V0Data);
}

void setup(){
  // Debug console
  Serial.begin(9600);
  // Make pin 2 HIGH by default
  pinMode(INTPIN, INPUT_PULLUP);
  pinMode(16,OUTPUT);
  // Attach INT to our handler
  attachInterrupt(digitalPinToInterrupt(INTPIN), checkPin, RISING);
  digitalWrite(16, LOW);

  
  dht.begin();

  // Setup a function to be called every 2 seconds
  timer.setInterval(2000L, sendSensor);
  
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run();
  timer.run();
  if (pinChanged) {
    // Procesa el valor
    if (ext_intFlag) {
      led1.on();
     }  
    else{
      led1.off();
    }
    // Clear the mark, as we have processed the value
    pinChanged = false;
  }
}
