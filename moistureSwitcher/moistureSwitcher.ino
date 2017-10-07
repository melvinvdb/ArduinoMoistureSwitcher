#include "DHTadafruit/DHT.h"

//#define DEBUG

#define LEDR_PIN  8
#define DHT22_PIN 9
#define RECV_PIN 10
#define RELAY_PIN A0
#define POTM1_PIN A1
#define POTM2_PIN A2
#define POTM3_PIN A3
#define POTM4_PIN A4

//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHT22_PIN, DHTTYPE);

//mode select 
//mode 0 == dht22 check mode
//mode 1 == always on
//mode 2 == always off
byte mode = 0;

//check byte, when 0 it checks dht22
byte check = 0;

#define HUM_MEASURES 10
float hum[HUM_MEASURES] = { 0 };
byte humIndex = 0;

void setup() {
  pinMode(LEDR_PIN, OUTPUT);
  digitalWrite(LEDR_PIN, LOW);
  pinMode(RECV_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(POTM1_PIN, INPUT);
  pinMode(POTM2_PIN, INPUT);
  pinMode(POTM3_PIN, INPUT);
  pinMode(POTM4_PIN, INPUT);
  
#ifdef DEBUG 
  Serial.begin(9600);
  Serial.println("DHTxx test!");
#endif

  dht.begin();
  float h = dht.readHumidity();
  for (byte i = 0; i < HUM_MEASURES; i++)
  {
    hum[i] = h;
  }
}

float getHumidity()
{
  float h = dht.readHumidity();
  hum[humIndex] = h;
  if (humIndex + 1 >= HUM_MEASURES)
  {
    humIndex = 0;
  }
  else
  {
    ++humIndex;
  }
  h = 0;
  for (byte i = 0; i < HUM_MEASURES; i++)
  {
    h += hum[i];
  }
  h = h / HUM_MEASURES;
  return h;
}

void loop() {
  delay(100);
  if (check > 3)
  {
    check = 0;
  }
  boolean state_recv = digitalRead(RECV_PIN);
  if (state_recv)
  {
    if (mode == 2) 
    {
      mode = 0;
    }
    else 
    {
      ++mode;
    }
    switch (mode) 
    {
      case 0:
        digitalWrite(RELAY_PIN, LOW);
        break;
      case 1:
        digitalWrite(RELAY_PIN, HIGH);
        break;
      case 2:
        digitalWrite(RELAY_PIN, LOW);
        break;
    }
    for (byte i = 0; i < mode + 1; i++)
    {
      digitalWrite(LEDR_PIN, HIGH);
      delay(500);
      digitalWrite(LEDR_PIN, LOW);
      delay(500);
    }
  }
  
  if (check == 0 && mode == 0)
  {
    int pot1 = analogRead(POTM1_PIN);
    int pot2 = analogRead(POTM2_PIN);
    //int pot3 = analogRead(POTM3_PIN);
    //int pot4 = analogRead(POTM4_PIN);
    
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    //float h = dht.readHumidity();
    float h = getHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
  
    if (isnan(h) || isnan(t)) {
      return;
    }  
  
    pot1 = map(pot1, 0, 1023, 0, 100);
    pot2 = map(pot2, 0, 1023, 0, 100);
    int relay = digitalRead(RELAY_PIN);
    
    if (relay == HIGH)
    {
      if (h < pot2)
      {
        digitalWrite(RELAY_PIN, LOW);
        digitalWrite(LEDR_PIN, LOW);
      }
    }
    else
    {
      if (h > pot1)
      {
        digitalWrite(RELAY_PIN, HIGH);
        digitalWrite(LEDR_PIN, HIGH);
      }
    }
#ifdef DEBUG 
    Serial.print("mode: ");
    Serial.println(mode);
    Serial.print("Pot 1 (START IF HIGHER THAN): ");
    Serial.println(pot1);
    Serial.print("Pot 2 (STOP IF LOWER THAN): ");
    Serial.println(pot2);
    /*Serial.print("Pot 3: ");
    Serial.println(pot3);
    Serial.print("Pot 4: ");
    Serial.println(pot4);*/
    Serial.print("Humidity (AVG): ");
    Serial.print(h);
    Serial.print(" %\t");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.println(" *C");
    Serial.println(" ");
#endif
  }
  ++check;
}
