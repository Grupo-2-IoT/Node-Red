#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#define DHT11PIN 5 //pin.D1
#define DHT11PIN2 16 //pin.D0
#define DHT11TYPE DHT11

// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "Tec-Contingencia";
const char* password = "";

// MQTT broker credentials (set to NULL if not required)
const char* MQTT_username = NULL; 
const char* MQTT_password = NULL; 

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "10.41.53.178";
//For example
//const char* mqtt_server = "192.168.1.106";

// Initializes the espClient. You should change the espClient name if you have multiple ESPs running in your home automation system
WiFiClient espClient;
PubSubClient client(espClient);

DHT dht11(DHT11PIN, DHT11TYPE);
int LDR = 16;
float h11;
float t11;
bool day;

int analog_pin = A0;
int humedad_suelo;
int humedad_suelomap;
int luz;
int luz_map;


// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;

// This functions connects your ESP8266 to your router
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(String topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic room/lamp, you check if the message is either on or off. Turns the lamp GPIO according to the message
  // if(topic=="room/lamp"){
  //     Serial.print("Changing Room lamp to ");
  //     if(messageTemp == "on"){
  //       digitalWrite(lamp, HIGH);
  //       Serial.print("On");
  //     }
  //     else if(messageTemp == "off"){
  //       digitalWrite(lamp, LOW);
  //       Serial.print("Off");
  //     }
  // }
  // Serial.println();
}

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    /*
     YOU MIGHT NEED TO CHANGE THIS LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a new name to the ESP8266.
     Here's how it looks:
       if (client.connect("ESP8266Client")) {
     You can do it like this:
       if (client.connect("ESP1_Office")) {
     Then, for the other ESP:
       if (client.connect("ESP2_Garage")) {
      That should solve your MQTT multiple connections problem
    */
    if (client.connect("ESP8266Client", MQTT_username, MQTT_password)) {
      Serial.println("connected");  
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
      client.subscribe("room/lamp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht11.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  if(!client.loop())
    client.connect("ESP8266Client", MQTT_username, MQTT_password);

  now = millis();
  // Publishes new temperature and humidity every 30 seconds
  if (now - lastMeasure > 30000) {
    dht_11();
    // // Check if any reads failed and exit early (to try again).
    // if (isnan(humidity) || isnan(temperatureC) || isnan(temperatureF)) {
    //   Serial.println("Failed to read from DHT sensor!");
    //   return;
    // }

    // Publishes Temperature and Humidity values
//    client.publish("room/temperature", String(t11).c_str());
//    client.publish("room/humidity", String(h11).c_str());
//    client.publish("room/Ghumidity", String(humedad_suelomap).c_str());
//    client.publish("room/day", String(day).c_str());
    //Uncomment to publish temperature in F degrees
    //client.publish("room/temperature", String(temperatureF).c_str());

    StaticJsonDocument<128> doc;

    doc["Temperatura"] = t11;
    doc["Humedad"] = h11;
    doc["HumedadTierra"] = humedad_suelomap;
    doc["Dia"] = day;

    String output;
    
    serializeJson(doc, output);

    Serial.print("Publish message: ");
    Serial.println(output);
    client.publish("room/desertico", output.c_str());

    // put your main code here, to run repeatedly:
    humedadsuelo();
    delay(1000);
    dht_11();
    delay(1000);
    luzSensor();
    delay(1000);
  }

}


void humedadsuelo(){
  humedad_suelo = analogRead(analog_pin);
  humedad_suelomap = map(humedad_suelo, 0, 1023, 100, 0);
  Serial.print("Humedad de suelo: ");
  Serial.print(humedad_suelomap);
  Serial.println("%");
}

void luzSensor(){
  luz = analogRead(LDR);
  luz_map = map(luz, 0, 1023, 100, 0);

  if(luz_map > 50){
    day = "1";
    Serial.print("Luz: ");
    Serial.print("Dia");
    Serial.print("\n");
  }else{
    day = "0";
    Serial.print("Luz: ");
    Serial.print("Noche");
    Serial.print("\n");
  }
};


void dht_11(){
  h11 = dht11.readHumidity();
  // Read temperature as Celsius (the default)
  t11 = dht11.readTemperature();

  if (isnan(h11) || isnan(t11)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Humidity: "));
  Serial.print(h11);
  Serial.print(F("%  Temperature: "));
  Serial.print(t11);
  Serial.println(F("°C "));

  //if (t11 >= 30){
  digitalWrite(LED_BUILTIN, LOW);
    //delay(1000);
  //}
  //else{
    //digitalWrite(LED_BUILTIN, HIGH);
  //}

}
