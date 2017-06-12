/*********************************
*                                *
* Firmware del Indoor MATIC V0.1 *
*                                *
*********************************/

/* L I B R E R I A S */
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

char servidor[] = "www.indoormatic.com.ar";

/* V A R I A B L E S */
//int debug = 0;
String GET              = "";     //
String stringDelSerial  = "";     // string que recibe lo que viene por el puerto serial
String macAddress = "";

boolean stringCompleta  = false;  // será true cuando llegue un comando por serial

/* I N I C I A L I Z A libreria WiFi*/
WiFiClient client;

/* S E T U P - Cosas que corren una sola vez */
void setup()
{
    Serial.begin( 115200 ); // inicia comunicacion serie a 115.200 baudios
    //WiFi.begin();
    WiFiManager wifiManager; //WiFiManager - Local intialization. Once its business is done, there is no need to keep it around
    wifiManager.setConfigPortalTimeout(180);
    macAddress = WiFi.macAddress();
    //wifiManager.resetSettings(); //reset saved settings
    wifiManager.autoConnect( "IndoorMatic" );
    //si llega hasta acá es porque se conectó al WiFi
    Serial.println( "[ONLINE]" );
}

void loop()
{
  escuchaSerial();
  analizaComando( stringDelSerial );
}


/********************
* F U N C I O N E S *
********************/
//conexion con el servidor
void conexionServidor( char servidor[], String solicitud )
{
  if ( client.connect( servidor, 80 ) )
  {
    //Serial.println( "[ENVIANDO_DATOS]" );
    client.println( solicitud ); // envia el request http
    client.println("Connection: close");// cierra la conexion
    // imprime la respuesta del servidor
    Serial.print( "[" );
    while ( client.available() )
    {
      Serial.write( client.read() );
    }
    Serial.println( "]" );
  }
  else
  {
  // if you couldn't make a connection:
  Serial.print( "[FALLO_CONEXION]" );
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
}

//parsea un string para ver que tipo de comando es
void analizaComando( String comando )
{
  comando.trim();
  if ( stringCompleta )
  {
    stringCompleta = false;

    if( comando.startsWith( "<" ) && comando.endsWith( ">" ) )
    {
      GET = "";
      comando = comando.substring( 1, comando.length() - 1);
      GET = "GET http://www.indoormatic.com.ar/im/im.php";
      GET += "?1a1dc91c907325c69271ddf0c944bc72&disp=";
      GET += macAddress;
      GET += comando;
      conexionServidor(servidor, GET);
    }

    else if( comando.startsWith( "[" ) && comando.endsWith( "]" ) )
    {
      //comando = comando.substring( 1, comando.length() - 1) ;
      if ( comando.equals( "[ESP_status]" ) )
      {
        if ( WiFi.status() == 3 )
        {
          Serial.println( "[ONLINE]" );
          //Serial.print(WiFi.status());
        }
        else
        {
          Serial.println( "[OFFLINE]" );
          Serial.println( "[RESET]" );
          ESP.reset();
        }
      }

      else if (comando.equals( "[ESP_reset]" ))
      {
        Serial.println( "[RESET]" );
        ESP.reset();
      }
    }

    else
    {
      Serial.print( "[comando_error]" );
      ESP.reset();
    }
  }
}
