#include <lmic.h>
#include <hal/hal.h>
#include <SPI.h>
#include <DHT.h>
#include <CayenneLPP.h>

#define DHTPIN 23
#define DHTYPE DHT22

DHT dht(DHTPIN,DHTYPE);
CayenneLPP lpp(51);

uint8_t NWSK[16] = { 0xA7, 0x97, 0xD2, 0x65, 0x14, 0x6B, 0x31, 0x39, 0xD3, 0x97, 0xC1, 0xDE, 0x25, 0x5D, 0x7B, 0xAA };
uint8_t APPSKEY[16] = { 0x79, 0x90, 0xEC, 0xC7, 0xA9, 0x50, 0x57, 0x46, 0x7A, 0x95, 0xDA, 0x6A, 0x58, 0x62, 0xAC, 0xA4 };
uint32_t DEVADDR = 0x26021460;

void os_getArtEui(u1_t* buf){}
void os_getDevEui(u1_t* buf){}
void os_getDevKey(u1_t* buf){}

const unsigned TX_INTERVAL = 10;
unsigned long previousMilis =0;

const lmic_pinmap lmic_pins = {
  .nss = 18,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 14,
  .dio = { 26 , 33, 32 }
};

void onEvent(ev_t ev){
  switch(ev){
    case EV_TXCOMPLETE:
    Serial.println("[LMIC] Radio TX complete");
    digitalWrite(LED_BUILTIN,LOW);
    break;
    default:
    Serial.println("Evento desconocido");
    break;
    }
  
  }

void setup() {
  Serial.begin(115200);
  Serial.println("[INFO] Iniciando");
  pinMode(LED_BUILTIN,OUTPUT);
  dht.begin();
  os_init();
  LMIC_reset();
  LMIC_setSession(0x1,DEVADDR,NWSK,APPSKEY);
  for(int chan=0;chan<72;++chan){
    LMIC_disableChannel(chan);
  }
  //904.3 Mhz banda de 915 Mhz 902-928 Mhz
  LMIC_enableChannel(10);
  LMIC_setLinkCheckMode(0);
  LMIC_setDrTxpow(DR_SF7,20);
  previousMilis=millis();
}

void loop() {
  if(millis()>previousMilis+(TX_INTERVAL*1000)){
    getInfoAndSend();
    previousMilis=millis();
    }
  os_runloop_once();
}

void enviar_datos(uint8_t *mydata, uint16_t len){
  if(LMIC.opmode & OP_TXRXPEND){
    Serial.println("[LMIC] OP_TXRXPEND, not sendig");
    }
  else{
    LMIC_setTxData2(1,mydata,len,0);
      }
  }

void getInfoAndSend(){
  digitalWrite(LED_BUILTIN,HIGH);
  //Leer sensores y transmitir
  float temp = dht.readTemperature();
  lpp.addTemperature(1,temp);
  enviar_datos(lpp.getBuffer(),lpp.getSize());
  }
