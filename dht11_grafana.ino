#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include<ESP8266WebServer.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN D6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

 
const char* ssid     = "LANCOMBEIA";
const char* password = "beialancom";
 
WiFiClient WiFiclient;
MQTTClient client;
 
unsigned long lastMillis = 0;

char data[80];
StaticJsonDocument<200> doc;
 
void setup() {
 Serial.begin(115200);
 delay(10);
 Serial.println();
 Serial.println();
 Serial.print("Connecting to ");
 Serial.println(ssid);

WiFi.hostname("ESP_Loredana_C");

// WIFI CONNECTION
 
 WiFi.begin(ssid, password);
 while (WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.print(".");
 }
 
 Serial.println("");
 Serial.println("WiFi connected");  
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
 delay(2000);

// Pornim senzorul DHT11
dht.begin();

// MQTT CONNECTION

 
 Serial.print("connecting to MQTT broker...");
 client.begin("mqtt.beia-telemetrie.ro", WiFiclient);
 connect();
}
 
void connect() {
 while (!client.connect("odsi/rpi/loredana", "try", "try")) {
   Serial.print(".");
 }
 
 Serial.println("\nconnected!");
 client.subscribe("odsi/rpi/loredana");

}


// SENZOR DHT11


void loop(void)
{
  // Atribuim variabilei "h" valoarea umiditatii, citita de functia "dht.readHumidity()"
  float h = dht.readHumidity();

  // Atribuim variabilei "t" valoarea temperaturii, citita de functia "dht.readTemperature()"
  float t = dht.readTemperature();

//  //if (t>100 || t<0)
//    if (t='nan')
//  {
//    t=0;
//  }
//    if (h>100 || h<0)
//  {
//    h=0;
//  }
  // Afisam in consola mesajul "Exterior Temperature (C) = "
  Serial.print("Exterior Temperature (C) = ");

  // Afisam temperatura din camera, citita cu ajutorul senzorului Dht11
  Serial.println(t);

  // Afisam in consola mesajul "Exterior Humidity = "
  Serial.print("Exterior Humidity = ");

  // Afisam umiditatea din camera, citita cu ajutorul senzorului Dht11
  Serial.println(h);

  String value = "\"HUMID\": " + String(h) ;
  String value2 = ", \"TEMP\": " + String(t) ;
  value = value + value2;
  delay(3000);



 client.loop();
 if(!client.connected()) {
   connect();
 }


// SEND VALUE TO MQTT

 
 if(millis() - lastMillis > 1000) {
   lastMillis = millis();
   String payload = "{" + value + "}";
   payload.toCharArray(data, (payload.length() + 1));
   client.publish("odsi/rpi/loredana", data);
 }
}
