#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

char servidor[] = "www.indoormatic.com.ar";
int debug = 0;
String GET      = "";// a string to hold incoming data
String stringDelSerial = "";
boolean stringCompleta  = false;  // whether the string is complete

// Inicializa la libreria Wifi client
WiFiClient client;

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    //WiFi.begin();
    //WiFiManager - Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //wifiManager.resetSettings(); //reset saved settings
    wifiManager.autoConnect("IndoorMatic");
    //if you get here you have connected to the WiFi
    Serial.println("CONECTADO_WiFi");
}

void loop()
{
  escuchaSerial();
  if (stringCompleta)
  {
    stringCompleta = false;
    analizaComando(stringDelSerial);
  }
}


/********************
* F U N C I O N E S *
********************/
//conexion con el servidor
void conexionServidor( char servidor[], String solicitud )
{
  if(debug > 0)
  {
    Serial.print("\n");
    Serial.print("solicitud GET= ");
    Serial.println(solicitud);
  }
  if (client.connect(servidor, 80))
  {
    client.println(solicitud); // envia el request http
    client.println("Connection: close");// cierra la conexion
    // imprime la respuesta del servidor
    while (client.available())
    {
      Serial.write(client.read());
    }
  }
  else
  {
  // if you couldn't make a connection:
  Serial.print("conexion_FALLIDA");
  }

}


/***********************************
 * F U N C I O N E S   S E R I A L *
/**********************************/

// Escucha el puerto serie y si hay dato lo devuelve en stringDelSerial.
void escuchaSerial()
{
  if (Serial.available())
  {
    stringDelSerial = Serial.readString();
    stringCompleta = true;
  }
  if(debug > 0)
  {
    Serial.print("\n");
    Serial.print("string del Serial = ");
    Serial.println(stringDelSerial);
  }
}

//parsea un string para ver que tipo de comando es
void analizaComando(String comando)
{
  GET = "";
  comando.trim();
  if( comando.startsWith( "<" ) && comando.endsWith( ">" ) )
  {
    //datoRecibido = true;
    comando = comando.substring( 1, comando.length() - 1);
    GET = "GET http://www.indoormatic.com.ar/im/im.php";
    GET += comando;
    conexionServidor(servidor, GET);
  }

  else if( comando.startsWith( "[" ) && comando.endsWith( "]" ) )
  {
    //comando = comando.substring( 1, comando.length() - 1) ;
    if (comando.equals("[ESP_status]"))
    {
      if (WiFi.status() == 3)
      {
        Serial.print("CONECTADO_OK");
        //Serial.print(WiFi.status());
      }
      else
      {
        Serial.print("CONECTADO_NO");
      }
    }

    if (comando.equals("[ESP_mac]"))
    {
        Serial.print("MacAddress: ");
        Serial.print(WiFi.macAddress());
    }
  }

  else
  {
    Serial.print("comando_error");
    ESP.reset();
  }
}
