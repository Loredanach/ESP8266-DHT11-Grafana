#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include<ESP8266WebServer.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN D6
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

 
const char* ssid     = "Wi-Fi_network_name";  //here you complete with your Wi-Fi network name
const char* password = "Wi-Fi_network_pass";  //here you complete with your Wi-Fi network password
 
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

WiFi.hostname("ESP_name); //ESP_name -> If you want your NodeMCU to have a specific name in the LAN network, put the name in -> ESP_name

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
 client.begin("mqtt_server_name", WiFiclient); //mqtt_server_name -> here you complete with you mqtt server name (where you want to upload your data)
 connect();
}
 
void connect() {
 while (!client.connect("Grafana_topic", "try", "try")) { //Grafana_topic -> you will complete with the topic you want to retrieve the data
   Serial.print(".");
 }
 
 Serial.println("\nconnected!");
 client.subscribe("Grafana_topic");

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
   client.publish("Grafana_topic", data);
 }
}
