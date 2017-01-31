#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

char server[] = "www.indoormatic.com.ar";
String GET      = "";// a string to hold incoming data
String inputString = "";
boolean stringComplete  = false;  // whether the string is complete
boolean datoRecibido    = false;  // cuando es verdadero lee el dato de serie

// Inicializa la libreria Wifi client
WiFiClient client;

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    //WiFiManager - Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //wifiManager.resetSettings(); //reset saved settings
    wifiManager.autoConnect("IndoorMatic");
    //if you get here you have connected to the WiFi
    Serial.println("CONECTADO_WiFi");
}

void loop()
{
  inputString = escuchaSerial();
  GET = analizaComando(inputString);

  if (datoRecibido)
  {
    datoRecibido = false;
    Serial.println( WiFi.isConnected() ) ;
    //Serial.println( inputString ) ;
    conexionServidor(server, GET);
  }
}



//conexion con el servidor

void conexionServidor( char servidor[], String solicitud )
{
  if (client.connect(servidor, 80))
  {
    //Serial.println("connecting...");
    // envia el request http
    client.println(solicitud);
    // cierra la conexion
    client.println("Connection: close");
    // imprime el request http
    //Serial.println(solicitud);
    // imprime la respuesta del servidor
    while (client.available())
    {
      Serial.write(client.read());
    }
  }
  else
  {
  // if you couldn't make a connection:
  Serial.println("conexion FALLIDA");
  }

}

// Escucha el puerto serie y si hay dato lo devuelve en un string.
String escuchaSerial()
{
  String stringSerial = "";// a string to hold incoming data
  if (Serial.available() > 0)
  {
    //Serial.print(Serial.readString());
    stringSerial = Serial.readString();
    return stringSerial;
  }
  else
  {
    return "0";
  }
}

//parsea un string para ver que tipo de comando es
String analizaComando(String comando)
{
  String comandoAnalizado = "";
  //comando.trim();
  if( comando.startsWith( "<" ) && comando.endsWith( ">" ) )
  {
    datoRecibido = true;
    comando = comando.substring( 1, comando.length() - 1) ;
    comandoAnalizado = "GET http://www.indoormatic.com.ar/im/im.php";
    comandoAnalizado += comando;
    return comandoAnalizado;
  }

  if( comando.startsWith( "[" ) && comando.endsWith( "]" ) )
  {
    comando = comando.substring( 1, comando.length() - 1) ;
    if (comando.equals("ESP_status"))
    {
      if (WiFi.status())
      {
        Serial.println("CONECTADO_OK");
      }

    }
  }

  else
  {
    datoRecibido = false;
  }
}
