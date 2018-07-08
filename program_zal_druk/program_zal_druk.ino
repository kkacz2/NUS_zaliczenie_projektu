// Limit Switches
#include <LiquidCrystal.h>
LiquidCrystal lcd(16,17,23,25,27,29);
#define HEATER_0 10
#define FAN2 44
#define TEMP_0 13   // Analog Input
#define a0   292.686165608748
#define a1  -0.877649103217767
#define a2   0.00140796160025791
#define a3  -8.15110931061918e-07
#define heater_OFF digitalWrite(HEATER_0, LOW);
#define heater_ON digitalWrite(HEATER_0, HIGH);
#define fan_OFF digitalWrite(FAN2, LOW);
#define fan_ON digitalWrite(FAN2, HIGH);
double temp; // odczytywanie temperatury z czujnika analogowego
double i = 0, Ki; // sumowanie blędu oraz nastawa Ki do regulatora PI
double ZadanaTemperatura = 40; // Temperatura grzałki ustawiona jako poczatkowa
int zadtemp = 0; // zmienna dodatkowa aby na lcd wyswietlac temp zadana
double e, u; // bład, sterowanie
int licznik = 0; // licznik ten zlicza do 1000 po czym wyswietla zawartosc bufora na lcd ( co 1000 iteracji programu)
int licznik2 = 0; // licznik ten zlicza do 6. jest on po to aby krok czwartego silnika od ekstrudera wykonywal sie co 6 petle programu
int SilnikiOff; // zmienna do dodatkowej krancowki aby silniki wylaczac
double czas = 0, nowyczas = 0, deltaT = 0; // Sprawdzanie czasu programu itp.
double x1 = 0; // pomocnicza zmienna  do regulatora temperatury
// Silniki
#define X_MIN_PIN           3
#define X_MAX_PIN           2
#define Y_MIN_PIN          14
#define Y_MAX_PIN          15
#define Z_MIN_PIN          18
#define Z_MAX_PIN          19

#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38
#define X_CS_PIN           53

#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56
#define Y_CS_PIN           49

#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62
#define Z_CS_PIN           40

#define E0_STEP_PIN        26
#define E0_DIR_PIN         28
#define E0_ENABLE_PIN      24
#define E0_CS_PIN          42
// Sterowanie silnikami, dodatkowa krańcówka
double x;
int y;
int z;
int krokix = 0;
int krokiy = 0;
int krokiz = 0;
int ekstruder = 0; 
int flaga = 1;
int tmp;
String s;


void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(X_MAX_PIN, INPUT);
  digitalWrite(X_MAX_PIN, HIGH);
  
  pinMode(Y_MAX_PIN, INPUT);
  digitalWrite(Y_MAX_PIN, HIGH);
  
  pinMode(Z_MAX_PIN, INPUT);
  digitalWrite(Z_MAX_PIN, HIGH);

  pinMode(X_STEP_PIN , OUTPUT);
  pinMode(X_ENABLE_PIN , OUTPUT);
  pinMode(X_DIR_PIN , OUTPUT);
  digitalWrite(X_ENABLE_PIN , LOW);
  digitalWrite(X_DIR_PIN , LOW);

  pinMode(Y_STEP_PIN , OUTPUT);
  pinMode(Y_ENABLE_PIN , OUTPUT);
  pinMode(Y_DIR_PIN , OUTPUT);
  digitalWrite(Y_ENABLE_PIN , LOW);
  digitalWrite(Y_DIR_PIN , LOW);

  pinMode(Z_STEP_PIN , OUTPUT);
  pinMode(Z_ENABLE_PIN , OUTPUT);
  pinMode(Z_DIR_PIN , OUTPUT);
  digitalWrite(Z_ENABLE_PIN , LOW);
  digitalWrite(Z_DIR_PIN , LOW);

  pinMode(E0_STEP_PIN , OUTPUT);
  pinMode(E0_ENABLE_PIN , OUTPUT);
  pinMode(E0_DIR_PIN , OUTPUT);
  digitalWrite(E0_ENABLE_PIN , LOW);
  digitalWrite(E0_DIR_PIN , LOW);

  pinMode(FAN2, OUTPUT);
  digitalWrite(FAN2, HIGH);
  pinMode(HEATER_0, OUTPUT);
  digitalWrite(HEATER_0,LOW);
  lcd.begin(20,4);

}

void loop()
{
  nowyczas = micros(); // oblicza czas programu na bazie taktowania procesora/ potrzebne do calkowania
  // wyłączanie silników poprzez zewnętrzą krańcówkę
  SilnikiOff = digitalRead(X_MIN_PIN);
  x = digitalRead(X_MAX_PIN );
  y = digitalRead(Y_MAX_PIN );
  z = digitalRead(Z_MAX_PIN );
  if(SilnikiOff == 0)
  {
    digitalWrite(X_ENABLE_PIN,HIGH);
    digitalWrite(Y_ENABLE_PIN,HIGH);
    digitalWrite(Z_ENABLE_PIN,HIGH);
  }
  else
  {
    digitalWrite(X_ENABLE_PIN,LOW);
    digitalWrite(Y_ENABLE_PIN,LOW);
    digitalWrite(Z_ENABLE_PIN,LOW);
  }
  
    if ( x == 0 && krokix > 0 )
    {
      digitalWrite(X_STEP_PIN, HIGH);
      krokix--;
    }
    if ( y == 0 && krokiy > 0 )
    {
      digitalWrite(Y_STEP_PIN, HIGH);
      krokiy--;
    }
    if ( z == 0 && krokiz > 0 )
    {
      digitalWrite(Z_STEP_PIN, HIGH);
      krokiz--;
    }
    if(ekstruder >0) // wykonywanie ktoków przez silnik ekstrudera
    {
      if ( licznik2 >= 6)
      {
      digitalWrite(E0_STEP_PIN,HIGH);
      ekstruder--;
      licznik2 = 0;
      }  
    }

  while ( Serial.available() > 0)
  {
    s = Serial.readStringUntil('\n');
    // tutaj w programie podajemy 5 parametrow: liczba krokow na silniki, kolejno: x y z ekstruder, oraz temperature jaka ma osiągnać grzałka
    sscanf(s.c_str(), "x %d y %d z %d e %d t %d \n", &krokix, &krokiy, &krokiz, &ekstruder, &zadtemp);
    flaga = 0;
    ZadanaTemperatura = zadtemp;
    
    if(ekstruder > 0)
    {
      digitalWrite(E0_DIR_PIN , LOW);
    }
    else
    {
      ekstruder = -ekstruder;
      digitalWrite(E0_DIR_PIN , HIGH);
    }
    
    if (krokix > 0)
    {
      digitalWrite(X_DIR_PIN , LOW);
    }
    else
    {
      krokix = -krokix;
      digitalWrite(X_DIR_PIN , HIGH);
    }


    if (krokiy > 0)
    {
      digitalWrite(Y_DIR_PIN , LOW);
    }
    else
    {
      krokiy = -krokiy;
      digitalWrite(Y_DIR_PIN , HIGH);
    }


    if (krokiz > 0)
    {
      digitalWrite(Z_DIR_PIN , LOW);
    }
    else
    {
      krokiz = -krokiz;
      digitalWrite(Z_DIR_PIN , HIGH);
    }
  }
  // Koniec while
  // Konieczne opoznienia bo wysyłamy przebieg prostokatny jako sygnaly do silnikow
    delayMicroseconds(10);
    digitalWrite(X_STEP_PIN, LOW);
    digitalWrite(Y_STEP_PIN, LOW);
    digitalWrite(Z_STEP_PIN, LOW);
    digitalWrite(E0_STEP_PIN,LOW);
    delayMicroseconds(10);
if(krokix == 0 && krokiy == 0 && krokiz == 0 && ekstruder == 0 && flaga == 0)
  {
   Serial.println(y);
   flaga=1;
  }

 
//    regulator temperatury
  temp = analogRead(TEMP_0);
  x1 = temp;
  //  a0   a1*x       a2*x^2     a3*x^3
  y = a0 + a1*x1 + a2*pow(x1,2)+ a3*pow(x1,3); // akt temperatura
  Ki = 0.08; // nastawa Ki
  deltaT = nowyczas - czas; // wyznaczanie czasu wykonywania się jednej petli programu, potrzebne do całkowania w członie I
  e = ZadanaTemperatura - y;
  i = i + deltaT/1000000*e; // obliczanie calki (deltaT jest podana w us, wiec trzeba podzielic przez milijon aby przejsc na sekundy)
  u= 255*(e/(ZadanaTemperatura - 20)) + Ki*i; nastawa P (poczatkowa) to 255/20. 
  
 // Sterowanie grzalka
  if(u<=255 && u >= 0)
  {
    analogWrite(HEATER_0,u);
 }
  else
  {
   if(u>255)
   {
   analogWrite(HEATER_0, u=255);
   }
   else
   {
     analogWrite(HEATER_0, u=0);  
   }
  }  
  if( licznik > 1000) // wyswietlanie na lcd
  {
    lcd.setCursor(0,0);
    lcd.print("Temp: ");
    lcd.setCursor(6,0);
    lcd.print(y);
    lcd.setCursor(0,1);
    lcd.print("Goal: ");
    lcd.setCursor(6,1);
    lcd.print(ZadanaTemperatura);
    
    licznik = 0;
  }
  // liczniki oraz nadpisywanie czasu programu 
  licznik++; 
  licznik2++;
  czas = nowyczas;
 
}

