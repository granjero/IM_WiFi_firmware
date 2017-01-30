# 1 "/var/folders/8z/vhz34kzj3ql22f12djy1_4vr0000gn/T/tmpBaDQwT"
#include <Arduino.h>
# 1 "/Volumes/almacen/Electronica/IndoorMatic/arduino/IM_WiFi_firmware/src/WiFi_firm_0.1.ino"
#include <ESP8266WiFi.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

char server[] = "www.indoormatic.com.ar";
String GET = "";
String inputString = "";
boolean stringComplete = false;
boolean datoRecibido = false;

WiFiClient client;
void setup();
void loop();
void conexionServidor( char servidor[], String solicitud );
String escuchaSerial();
String analizaComando(String comando);
#line 15 "/Volumes/almacen/Electronica/IndoorMatic/arduino/IM_WiFi_firmware/src/WiFi_firm_0.1.ino"
void setup()
{

    Serial.begin(115200);

    WiFiManager wifiManager;

    wifiManager.autoConnect("IndoorMatic");

    Serial.println("CONECTADO_OK");
}

void loop()
{
  inputString = escuchaSerial();
  GET = analizaComando(inputString);

  if (datoRecibido)
  {
    datoRecibido = false;
    Serial.println( WiFi.isConnected() ) ;

    conexionServidor(server, GET);
  }
}





void conexionServidor( char servidor[], String solicitud )
{
  if (client.connect(servidor, 80))
  {


    client.println(solicitud);

    client.println("Connection: close");



    while (client.available())
    {
      Serial.write(client.read());
    }
  }
  else
  {

  Serial.println("conexion FALLIDA");
  }

}


String escuchaSerial()
{
  String stringSerial = "";
  if (Serial.available() > 0)
  {

    stringSerial = Serial.readString();
    return stringSerial;
  }
  else
  {
    return "0";
  }
}


String analizaComando(String comando)
{
  String comandoAnalizado = "";

  if( comando.startsWith( "<" ) && comando.endsWith( ">" ) )
  {
    datoRecibido = true;
    comando = comando.substring( 1, comando.length() - 1) ;
    comandoAnalizado = "GET http://www.indoormatic.com.ar/test.php?debug=";
    comandoAnalizado += comando;
    return comandoAnalizado;
  }
  else
  {
    datoRecibido = false;
  }
}