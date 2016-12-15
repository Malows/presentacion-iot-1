#include <Ethernet.h>
#include <SPI.h>


// Defino las constantes que uso

char RELAY[]   =   3;
char SERVER[]  =  "app.connectingthings.io";
char API_KEY[] =  "xxxxxxxxx";
char DEVICE[]  =  "xxxxxxxxx";


// Si falla el servidor DHCP, uso una ip estática

byte mac[] = { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x01 };
IPAddress ip(192, 168, 2, 177);
EthernetClient client;


// intervalo de tiempo entre las peticiones al servidor 

unsigned long lastConnectionTime = 0;
unsigned long postingInterval = 5000;

void getRequest() {
  /*Arma la petición y la manda a travez del cliente*/
  
    client.print("GET /resources/device/");
    client.print(DEVICE);
    client.print("/key/");
    client.print(API_KEY);
    client.println(" HTTP/1.1");
    
    client.println("Host: ");
    client.print(SERVER);
    client.println(":3001");
    
    client.println("User-Agent: arduino-ethernet");
    
    client.println("Connection: close");
    
    client.println();

}

void httpRequest() {
  /* Detiene el cliente, se conecta con el servidor y registra cuando se reliza una petición exitosa */
  
  client.stop();

  if ( client.connect( SERVER, 3001 ) ) {
    
    Serial.println("connecting...");

    getRequest();
    
    lastConnectionTime = millis();
    
  } else {
    
    Serial.println("connection failed");
    
  }
}

bool verEstadoLuces( String data ) {
  /* Extrae el valor que debe corresponder con las luces de la respuesta del servidor */
  
  int primero = data.indexOf( "{" );
  
  int segundo = data.lastIndexOf( "}" );
  
  String json = data.substring( primero, segundo + 1 );

  int dos_puntos = json.lastIndexOf( ":" );
  
  String valor =  json.substring( ( dos_puntos + 2 ), ( dos_puntos + 3 ) );
 
  return valor.toInt() != 0;
  
}

void manejarLuces() {
  /* En base al estado que debe tener la luz, activa o desactiva el releé */
  
  if ( client.available() ) {
    
    if ( verEstadoLuces( client.readString() ) ) {
      
      digitalWrite( RELAY, HIGH );
      
    } else {
      
      digitalWrite( RELAY, LOW );
      
    }
    
  }
    
}

void setup() { 
  
  pinMode( RELAY, OUTPUT );
  
  Serial.begin( 9600 );
  while( !Serial ) {
    ;
  }
  
  if ( Ethernet.begin( mac ) == 0 ) {
    Serial.println( "Failed to configure Ethernet using DHCP" );
    Ethernet.begin( mac, ip );
  }
  
  Serial.print("Mi IP es: ");
  Serial.println(Ethernet.localIP());
 
}

void loop(){
  manejarLuces();

  if ( millis() - lastConnectionTime > postingInterval ) {
    httpRequest();
  }
  
  delay(1000);
}








