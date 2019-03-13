#include "config.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "Adafruit_CCS811.h"

#define DATA_PIN 23

Adafruit_CCS811 ccs;

DHT_Unified dht(DATA_PIN, DHT22);

AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *humidity = io.feed("humidity");
AdafruitIO_Feed *CO2 = io.feed("co2");
AdafruitIO_Feed *TVOC = io.feed("tvoc");
AdafruitIO_Feed *TEMP = io.feed("temp");

void setup() {

  // start the serial connection
  Serial.begin(115200);

  if(!ccs.begin()){
    Serial.println("Failed to start sensor! Please check your wiring.");
  }

  while(!ccs.available());
  float temp = ccs.calculateTemperature();
  ccs.setTempOffset(temp - 25.0);
  
  // initialize dht22
  dht.begin();

  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO");
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());

}

void loop() {

  io.run();
  float co2,tvoc=0;
  if(ccs.available()){
    float temp = ccs.calculateTemperature();
    if(!ccs.readData()){
      co2=ccs.geteCO2();
      Serial.print("CO2: ");
      Serial.print(co2);
      CO2->save(co2);
      Serial.print("ppm, TVOC: ");
      tvoc=ccs.getTVOC();
      Serial.print(tvoc);
      TVOC->save(tvoc);
      Serial.print("ppb   Temp:");
      Serial.println(temp);
      TEMP->save(temp);
    }
    else{
      Serial.println("Error en CCS811");
    }
  }
  
  sensors_event_t event;
  dht.temperature().getEvent(&event);

  float celsius = event.temperature;

  Serial.print("celsius: ");
  Serial.print(celsius);
  Serial.println("C");

  // save fahrenheit (or celsius) to Adafruit IO
  temperature->save(celsius);

  dht.humidity().getEvent(&event);

  Serial.print("humidity: ");
  Serial.print(event.relative_humidity);
  Serial.println("%");

  // save humidity to Adafruit IO
  humidity->save(event.relative_humidity);

  // wait 5 seconds (5000 milliseconds == 5 seconds)
  delay(5000);

}
