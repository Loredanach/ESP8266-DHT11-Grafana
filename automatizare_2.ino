// Librarie folosita pentru DHT11
#include <DHT.h>

//definire pini utilizati
#define soilMoistureSensor D6// Pin senzor de umiditate in sol
#define relay D7 // Pin releu pompa
#define waterLevelSensor 0 // Pin senzor de nivel al apei
#define DHTPIN D5 // Pin senzor de temperatura si umiditate

// Defineste tipul senzorului folosit ("DHT11")
#define DHTTYPE DHT11

// Initiere senzor DHT11
DHT dht(DHTPIN, DHTTYPE);


float timpUdarePlanta = 20; 

void setup() {
  //## Setari Senzor umiditate si temperatura
  // Pornire senzorul
  dht.begin();
  //##

  //## Setari NodeMCU
  // Initiere interfata serial
  Serial.begin(9600);
  // Setare pin inclus ca pin de iesire
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  //##
}

void loop() {
  int umiditateSol = citesteUmiditateSol();
  int nivelApaInVas = citesteNivelApaInVas();
  float umiditateExterior = citesteUmiditateExterior();
  float temperaturaExterior = citesteTempExterior();

  Serial.print("|Status sistem: ");
  Serial.print("Pregatit");

// Serial.println(analogRead(waterLevelSensor));

  Serial.print("|Umiditate in sol: ");
  Serial.print(umiditateSol);

  Serial.print("|Umiditate in exterior: ");
  Serial.print(umiditateExterior);
  Serial.print(" %");

  Serial.print("|Temperatura in exterior: ");
  Serial.print(temperaturaExterior);
  Serial.print(" °C");

  Serial.print("|nivelApaInVas: ");
  Serial.print(nivelApaInVas);


  //Daca umiditate din sol este sub pragul acceptat
  if (umiditateSol == 1) { //senzorul are ca output valoarea 1 cand solul nu este umed si valoarea 0 cand solul este umed
    Serial.println();
    Serial.println("Umiditate redusa. Initializez procesul de udare a plantei.");
    //Verifica daca in vas este apa
    if (nivelApaInVas >200) {
      Serial.println("Este prezenta apa in vas. Pornire pompa");
      // Stinge ledul se status de pe NodeMcu
      digitalWrite(LED_BUILTIN, HIGH);
      //Porneste pompa pentru intervalul de timp de udare
      startPompa(timpUdarePlanta);
//    
    }
    else {
      Serial.println("Nu pot uda planta, pentru ca nu este apa in vas");
      //Altfel porneste ledul de pe NodeMcu in semn de alerta ca in vas nu mai este apa
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
  else {
          Serial.println("Solul este destul de umed. Nu necesita apa");

    }
  // Reia procesul odata la 5 secunde
  delay(5000);
}

void startPompa(int timpUdarePlanta) {

    //## Setari RELEU
  // Defineste pinul relay ca fiind unul de iesire
  pinMode(relay, OUTPUT);
  // pentru ca este un releu activ doar daca simte semnal pe pinul de semnal, atunci aplica tensiune maxima pe pin relay

  //-> MODIFICARE DIN LOW IN HIGH CA SA INCHIDEM CONTACTUL
  // Pune pompa in functiune
  digitalWrite(relay, HIGH );

  // Functioneaza cat timp primeste ca parametru
  delay(1000);
//  pinMode(relay, INPUT);
  digitalWrite(relay, LOW );

}

int citesteUmiditateSol() {
  // Citeste valoare areturnata de pin-ul analog A0 si transpune-o in intervalul 0, 100, deoarece ea este citita in intervalul 0, 1024
  return (digitalRead(soilMoistureSensor));
}

int citesteNivelApaInVas() {
  // Citeste valoarea returnata de pin-ul senzorului
  int nivelApaInVas = analogRead(waterLevelSensor);
  return (analogRead(waterLevelSensor));

}

float citesteTempExterior() {
  return dht.readTemperature();
}

float citesteUmiditateExterior() {
  return dht.readHumidity();
}
