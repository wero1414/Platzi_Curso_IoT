#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

WiFiMulti wifiMulti;

void setup() {
    Serial.begin(115200);
    delay(10);

    // Añadimos un Access point para conectarnos a internet
    // Nuestro SSID y PSWD
    wifiMulti.addAP("TihBeta2", "1nv3nt0r3S");

    Serial.println();
    Serial.println();
    Serial.print("Conectado a WiFi... ");

    while(wifiMulti.run() != WL_CONNECTED) {
        Serial.print(".");
        delay(50);
    }

    Serial.println("");
    Serial.println("WiFi conectado");
    Serial.println("Direccion IP: ");
    Serial.println(WiFi.localIP());

    delay(500);
}

void loop() {
  // Creamos una instancia cada ciclo
  // de un cliente HTTP para asi evitar
  // saturacion de un cliente.
  HTTPClient http;
  Serial.print("[HTTP] Iniciando...\n");
  http.begin("https://www.arduino.cc/asciilogo.txt");

  Serial.print("[HTTP] GET...\n");
  // Inicia la conexion y manda el header 
  // Correspondiente al comando GET.
  int httpCode = http.GET();

  // Si el comando GET regresa un valor
  // negativo, significa que hubo un codigo
  // de respuesta de error de la conexion HTTP
  if(httpCode > 0) {
      // El encabezado de la peticion HTTP fue enviado al servidor y el server, 
      // contesto de forma exitosa.
      Serial.printf("[HTTP] Respuesta... codigo: %d\n", httpCode);
      // si se encuentra un archivo en el servidor se almacena en un String
      // y se muestra por la terminal serial.
      if(httpCode == HTTP_CODE_OK) {
          String response = http.getString();
          Serial.println(response);
      }
  } 
  else {
      // Con la funcion "errorToString(httpCode).c_str()" convertimos el codigo HTTP de respuesta
      // a su significado en palabras humanas. 
      Serial.printf("[HTTP] GET... fallido, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
  
  delay(10000);
}
