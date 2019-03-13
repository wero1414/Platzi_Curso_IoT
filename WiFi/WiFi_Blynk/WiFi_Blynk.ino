#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Tu tendras tu token de autorizacion en la aplicacion de Blynk "Auth Token".
// Para obtenerlo ve a configuracion de proyecto.
char auth[] = "6b1e353ac76c4270b5e24a269c6415f8";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "TihBeta2";
char pass[] = "1nv3nt0r3S";

#define DHTPIN 23        
#define INTPIN 4

// Descomenta el tipo de DHT que estes usando
//#define DHTTYPE DHT11     // DHT 11
#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHT dht(DHTPIN, DHTTYPE);
BlynkTimer timer;

WidgetLED led1(V1);

// Creamos las variables que se usaran dentro de la interrupcion
// Variable tipo volatile porque puede cambiar de forma rapida.
volatile bool pinChanged = false;
volatile int  ext_intFlag   = 0;


// Funcion que maneja la interrupcion del pin donde tenemos nuestro boton
// Usamos esta funcion como una funcion dummy para activar una bandera
// para poder establecer la conexion a internet en el void loop
void checkPin(){
  // Invertimos el estado logico que usamos como auxiliar
  ext_intFlag = !ext_intFlag;
  // Activamos la bandera de interrupcion
  pinChanged = true;
}


// Esta funcion envia cada dos segundos por dos pines virtuales
// la temperatura y la humedad sensada por el sensor DHT.
// El tiempo de reporte se maneja con un timer que se define en el void setup()
void sendSensor()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature(); 

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V5, h);
  Blynk.virtualWrite(V6, t);
}

// Esta funcion se activa cada que se reciben datos por
// el pin virtual V0, cada que en la aplicacion se modifica
// el valro
BLYNK_WRITE(V0){
  int V0Data = param.asInt(); // Asignamos el valor que recibimos en V0 como entero.
  // Tambien puedes usar:
  //    String i = param.asStr();
  //    double d = param.asDouble();
  // Para distintos tipos de datos.
  // Serial.print("V0 value is: ");
  // Serial.println(V0Data);
  digitalWrite(16,V0Data);
}

void setup(){
  // Debug console
  Serial.begin(9600);
  //Configuramos 
  pinMode(INTPIN, INPUT_PULLUP);
  pinMode(16,OUTPUT);
  // Definimos pin con interrupcion a usar, asi como su funcion.
  attachInterrupt(digitalPinToInterrupt(INTPIN), checkPin, RISING);
  digitalWrite(16, LOW);

  dht.begin();

  // Configura la ejecucion de una funcion cada 2000 mS
  timer.setInterval(2000L, sendSensor);
  // Funcion para iniciar conexion WiFi 
  // y configurar las credenciales de Blynk
  Blynk.begin(auth, ssid, pass);
}

void loop(){
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
    // Borramos la bandera de cambio de estado por interrupcion
    pinChanged = false;
  }
}
