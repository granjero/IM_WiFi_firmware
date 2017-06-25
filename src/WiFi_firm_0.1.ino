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

char servidor[]                     = "www.indoormatic.com.ar";
/* V A R I A B L E S */
//int debug = 0;
String GET                          = "";     //
String stringDelSerial              = "";     // string que recibe lo que viene por el puerto serial
String macAddress                   = "";
String respuestaServidor            = "";

unsigned long millisAhora;
unsigned long millisUltimaConsulta  = 0; //varialble que guardará el valor de tiempo donde se leen los sensores
unsigned long millisUltimoStatus  = 0; //varialble que guardará el valor de tiempo donde se leen los sensores

const long intervaloConsulta        = 60000;
const long intervaloStatus          = 120000;

boolean stringCompleta              = false;  // será true cuando llegue un comando por serial


/* I N I C I A L I Z A libreria WiFi*/
WiFiClient client;

/* S E T U P - Cosas que corren una sola vez */
void setup()
{
    Serial.begin( 115200 ); // inicia comunicacion serie a 115.200 baudios
    //WiFi.begin();
    WiFiManager wifiManager; //WiFiManager - Local intialization. Once its business is done, there is no need to keep it around
    wifiManager.setConfigPortalTimeout(300);
    macAddress = WiFi.macAddress();
    Serial.println( macAddress );
    wifiManager.resetSettings(); //reset saved settings
    wifiManager.autoConnect( "IndoorMatic" );
    //si llega hasta acá es porque se conectó al WiFi
    Serial.println( F( "[ONLINE]" ) );
}

void loop()
{
  escuchaSerial();
  analizaComando( stringDelSerial );
  consultaAccion();
  estatus();
}


/* F U N C I O N E S */

//conexion con el servidor recibe el server a donde conectarse y el string de conexion carga la respuesta en respuestaServidor
void conexionServidor( char servidor[], String solicitud )
{
  respuestaServidor = "";
  if ( client.connect( servidor, 80 ) )
  {
    //Serial.println( "[ENVIANDO_DATOS]" );
    client.println( solicitud ); // envia el request http
    client.println( F( "Connection: close" ) );// cierra la conexion
    while ( client.available() )
    {
      respuestaServidor += (char)client.read();
    }
    if ( respuestaServidor == "" )
    {
      Serial.print( F( "[RESPUESTA_NULA]" ) );
    }
  }
  else
  {
  // if you couldn't make a connection:
  Serial.println( F( "[FALLO_CONEXION]" ) );
  // respuestaServidor = "NO_CONECTA_AL SERVER";
  }
}

//consulta a la pagina web si debe realizar alguna accion
void consultaAccion()
{
  millisAhora = millis();
  if ( millisUltimaConsulta > millisAhora )
  {
    millisUltimaConsulta  = 0;
  }

  if (millisAhora - millisUltimaConsulta >= intervaloConsulta)
  {
    millisUltimaConsulta = millis();
    GET = "GET http://www.indoormatic.com.ar/im/ordenes.php";
    GET += "?disp=";
    GET += macAddress;
    conexionServidor(servidor, GET);
    Serial.println( respuestaServidor );
  }
}

void estatus()
{
  millisAhora = millis();
  if ( millisUltimoStatus > millisAhora )
  {
    millisUltimoStatus  = 0;
  }

  if (millisAhora - millisUltimoStatus >= intervaloStatus)
  {
    millisUltimoStatus = millis();
    GET = "GET http://www.indoormatic.com.ar/im/status.php";
    conexionServidor(servidor, GET);
    //Serial.println( respuestaServidor );
    if ( respuestaServidor != "[INDOORMATIC]" && respuestaServidor != "")
    {
      Serial.println( F( "[OFFLINE]" ) );
      delay(1000);
      ESP.reset();
    }
    else
    Serial.println( F( "[ONLINE]" ) );
  }
}



/* F U N C I O N E S   S E R I A L */

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
      Serial.print("[");
      Serial.print( respuestaServidor );
      Serial.println("]");

    }

    else if( comando.startsWith( "[" ) && comando.endsWith( "]" ) )
    {
      //comando = comando.substring( 1, comando.length() - 1) ;
      if ( comando.equals( "[ESP_status]" ) )
      {
        if ( WiFi.status() == 3 )
        {
          Serial.println( F( "[ONLINE]" ) );
          //Serial.print(WiFi.status());
        }
        else
        {
          Serial.println( F( "[OFFLINE]" ) );
          delay(1000);
          //Serial.println( "[RESET]" );
          //ESP.reset();
        }
      }

      else if (comando.equals( "[ESP_reset]" ))
      {
        Serial.println( F( "[RESET]" ) );
        ESP.reset();
      }

      else if (comando.equals( "[CONSULTA]" ))
      {
        consultaAccion();
      }

      else if (comando.equals( "[CONN_status]" ))
      {
        estatus();
      }

      if ( comando.equals( "[REGADO]" ) )
      {
        GET = "GET http://www.indoormatic.com.ar/im/ordenes.php";
        GET += "?disp=";
        GET += macAddress;
        GET += "&regado=1";
      }

    }

    else
    {
      Serial.print( F( "[comando_error]" ) );
      //ESP.reset();
    }
  }
}
