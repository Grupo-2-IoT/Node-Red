/*

#define DHT11PIN 5 //pin.D1
// #define DHT11PIN 16 
// pin.D0

#define DHT11TYPE DHT11
#define BOMBADEAGUA 12 // D6
#define LUZPIN 2 // D4
#define VENTILADORPIN 4 // D2
#define LDR 16 // D0, to be changed 


 */
#include "DHT.h"
#define SECONDS_PER_CHECK 5000

#define DHT11PIN 5 //pin.D1
// #define DHT11PIN 16 
// pin.D0

#define DHT11TYPE DHT11
#define BOMBADEAGUA 12 // D6
#define LUZPIN 2 // D4
#define VENTILADORPIN 4 // D2
#define LDR 16 // D0, to be changed 

float h11;
float t11;
float rh11;
bool day;

int analog_pin = A0;
int humedad_suelo;
int humedad_suelomap;
int luz;
int luz_map;
int consumo = 0; //CAMBIAR DESPUES





// Timers auxiliar variables
long now = millis();
long lastMeasure = 0;

  
DHT dht11(DHT11PIN, DHT11TYPE);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  dht11.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BOMBADEAGUA, OUTPUT);
  pinMode(LUZPIN, OUTPUT);
  pinMode(VENTILADORPIN, OUTPUT);
}

void loop() {

  now = millis();
  // Publishes new temperature and humidity every 30 seconds
  if (now - lastMeasure > SECONDS_PER_CHECK) {
  lastMeasure = now;
    



    // put your main code here, to run repeatedly:
    humedadsuelo();
    dht_11();
    luzSensor();

  }

  requerimientos();
  digitalWrite(VENTILADORPIN, HIGH); 


}



void requerimientos(){
  if( day == 0){
    /* 
    
    digitalWrite(BOMBADEAGUA, LOW);
    digitalWrite(LUZPIN, LOW);
    
    */
   
    digitalWrite(LUZPIN, LOW); 
    Serial.println("ES DE NOCHE"); 
  }
  
  if(day == 1){
    /*
    s
    digitalWrite(BOMBADEAGUA, HIGH);
    digitalWrite(LUZPIN, HIGH);
    
    */
  
    digitalWrite(LUZPIN, HIGH); 
    Serial.println("ES DE DÍA"); 
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
    day = 1;
    Serial.print("Luz: ");
    Serial.print("Dia");
    Serial.print("\n");
  }else{
    day = 0;
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
