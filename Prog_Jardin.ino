
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

RTC_DS1307 RTC; // init module horloge
LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD addresse mémoire 0x27
// I2C sur les ports A4(SDA), A5(SCL)

// Table Evénements chaque ligne correspond à une voie. Sur la ligne, on trouve H départ, M Départ, H Arrêt, M Arrêt..
int TabEvt [6][4] =
{
  {10, 18, 10, 19},
  {14, 1, 14, 2},
  {10, 46, 10, 47},
  {14, 3, 14, 4},
  {25, 0, 0, 0},
  {25, 0, 0, 0}
};

// définition des pins de sortie
byte PinSorties[6] = {7, 8, 9, 10, 11, 12};

// detecteur de pluie
const int analogInPin = A0; //sur PIN analogique A0
int sensorValue = 0;


void setup () {

  lcd.init();
  lcd.backlight();
  lcd.setBacklight(LOW);// extinction LED affichage
  Serial.begin(9600);
  Wire.begin();
  RTC.begin();
  if (! RTC.isrunning()) {
    Serial.println("RTC is NOT running!");
    // la ligne suivante définit le RTC à la date et l'heure à laquelle cette esquisse a été compilée
    //RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  // Initialisation 7, 8, 9, 10, 11, 12 en sorties
  byte i;
  for ( i = 0; i < 6; i++ ) {
    pinMode(PinSorties[i], OUTPUT);
    digitalWrite (PinSorties[i], HIGH);
  }

  // Definition des pins des BP
  pinMode(2, INPUT); // BP démarage Manuel
  pinMode(3, INPUT); // BP changement mode ?

}
void loop () {

  // test si pluie (sensorValue peut etre entre 0 et 1024)
  sensorValue = analogRead(analogInPin);
  Serial.print("Sensor = " );
  Serial.print(sensorValue * 100 / 1024);
  Serial.println("%");
  // affichage PLUIE sur LCD
  // désactivation de l'arrosage
  // if sensorValue>??

  // pression bouton démarage manuel********************************
  byte demManuel = digitalRead(2);
  if (demManuel == HIGH)
  {
    // démarage d'une séquence manuelle d'arrosage
    Serial.println("PB appuyé");
    lcd.setBacklight(HIGH);
    byte j;
    for (j = 0; j < 4; j++) {
      digitalWrite(j + 7, LOW);
      // Afficher Voies actives sur la deuxième ligne du LCD
      lcd.setCursor(0, 1);
      lcd.print("Voies:");
      lcd.print(j + 1);
      delay(3000); // durée de l'arrosage en manuel
      digitalWrite(j + 7, HIGH);
    }
    lcd.setBacklight(LOW);
    demManuel = 0;
    j = 0;

  }


  // Affichage heure LCD****************************************

  DateTime now = RTC.now();


  // suivi débogage sur console serie
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  // Affichage sur LCD
  // Afficher l'heure sur la premiêre ligne du LCD
  lcd.setCursor(0, 0);
  lcd.print("Heure: ");
  if (now.hour() < 10) {
    lcd.print("0");
  }
  lcd.print(now.hour());
  lcd.print(':');
  if (now.minute() < 10) {
    lcd.print("0");
  }
  lcd.print(now.minute());
  //Serial.print(':');
  //lcd.print(':');
  //Serial.print(now.second(), DEC);
  //lcd.print(now.second());
  /*
    lcd.setCursor(7, 0);
    lcd.print(now.day(), DEC);
    lcd.print('/');
    lcd.print(now.month());
    lcd.print('/');
    lcd.print(now.year());
  */
  // Afficher Voies actives sur la deuxième ligne du LCD
  lcd.setCursor(0, 1);
  lcd.print("Voies:");

  // Rechercher si heure actuelle est dans dans la plage horaire de la voie 1
  lcd.setCursor(6, 1);
  if (now.hour() >= TabEvt [0][0] && now.minute() >= TabEvt [0][1] &&  now.hour() <= TabEvt [0][2] && now.minute() < TabEvt [0][3] ) {
    lcd.print(1);
    digitalWrite (PinSorties[0], LOW);
  }
  else {
    lcd.print("-");
    digitalWrite (PinSorties[0], HIGH);
  }

  // Rechercher si heure actuelle est dans dans la plage horaire de la voie 2
  lcd.setCursor(7, 1);
  if (now.hour() >= TabEvt [1][0] && now.minute() >= TabEvt [1][1] &&  now.hour() <= TabEvt [1][2] && now.minute() < TabEvt [1][3] ) {
    lcd.print(2);
    digitalWrite (PinSorties[1], LOW);
  }
  else {
    lcd.print("-");
    digitalWrite (PinSorties[1], HIGH);
  }

  // Rechercher si heure actuelle est dans dans la plage horaire de la voie 3
  lcd.setCursor(8, 1);
  if (now.hour() >= TabEvt [2][0] && now.minute() >= TabEvt [2][1] &&  now.hour() <= TabEvt [2][2] && now.minute() < TabEvt [2][3] ) {
    lcd.print(3);
    digitalWrite (PinSorties[2], LOW);
  }
  else {
    lcd.print("-");
    digitalWrite (PinSorties[2], HIGH);
  }

  // Rechercher si heure actuelle est dans dans la plage horaire de la voie 4
  lcd.setCursor(9, 1);
  if (now.hour() >= TabEvt [3][0] && now.minute() >= TabEvt [3][1] &&  now.hour() <= TabEvt [3][2] && now.minute() < TabEvt [3][3] ) {
    lcd.print(4);
    digitalWrite (PinSorties[3], LOW);
  }
  else {
    lcd.print("-");
    digitalWrite (PinSorties[3], HIGH);
  }

  /*
        // Rechercher si heure actuelle est dans dans la plage horaire de la voie 5
            lcd.setCursor(10,1);
            if (now.hour() >= TabEvt [4][0] && now.minute() >= TabEvt [4][1] &&  now.hour() <= TabEvt [4][2] && now.minute() < TabEvt [4][3] ) {
              lcd.print(5);
              digitalWrite (PinSorties[4], LOW);
            }
            else {
              lcd.print("-");
              digitalWrite (PinSorties[4], HIGH);
            }

       // Rechercher si heure actuelle est dans dans la plage horaire de la voie 6
            lcd.setCursor(11,1);
            if (now.hour() >= TabEvt [5][0] && now.minute() >= TabEvt [5][1] &&  now.hour() <= TabEvt [5][2] && now.minute() < TabEvt [5][3] ) {
              lcd.print(6);
              digitalWrite (PinSorties[5], LOW);
            }
            else {
              lcd.print("-");
              digitalWrite (PinSorties[5], HIGH);
            }
  */


  // Delais de rafrachissement*************************************
  delay(1000); // retour aubout de 5 s ?
  lcd.clear();// raffraichissement LCD
}
