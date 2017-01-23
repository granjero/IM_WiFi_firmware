#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager


char server[] = "www.indoormatic.com.ar";

String inputString      = "";// a string to hold incoming data
boolean stringComplete  = false;  // whether the string is complete
boolean datoRecibido    = false;  // cuando es verdadero lee el dato de serie
// Inicializa la libreria Wifi client
WiFiClient client;

void setup() {
    // put your setup code here, to run once:
    Serial.begin(115200);

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
    //wifiManager.resetSettings();
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("IndoorMatic");
    //if you get here you have connected to the WiFi
    Serial.println("CONECTADO");
}

void loop()
{
  inputString      = "";// a string to hold incoming data

  while (Serial.available() > 0)
  {
    //Serial.print(Serial.readString());
    inputString = Serial.readString();
    datoRecibido = true;
  }

  if (datoRecibido)
  {
    datoRecibido = false;
    String GET = "GET http://www.indoormatic.com.ar/test.php?debug=";
    GET += inputString;
    Serial.println(inputString);

    if (client.connect(server, 80))
    {
      Serial.println("connecting...");
      // envia el request http
     client.println(GET);
     // cierra la conexion
     client.println("Connection: close");
     // imprime el request http
     Serial.println(GET);
     // imprime la respuesta del servidor
     while (client.available())
     {
       Serial.write(client.read());
     }
    }
    else
    {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    }
  }
}
