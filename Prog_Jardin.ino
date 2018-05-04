
#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

// initialisation modules I2C
RTC_DS1307 RTC; // init module horloge
LiquidCrystal_I2C lcd(0x27, 16, 2); // init LCD > addresse mémoire 0x27
// I2C sur les ports A4(SDA), A5(SCL)

// Table Evénements chaque ligne correspond à une vanne.
// Sur la ligne, on trouve ( H départ, M Départ, H Arrêt court , M Arrêt court , Harret Long , M Arret Long )
/*
  int TabEvt [4][6] =
  {
  {17, 00, 17, 01, 17, 02},
  {17, 8, 18, 2, 18 , 3},
  {10, 46, 10, 47, 10, 48},
  {14, 3, 14, 4, 14, 5},
  };
*/
// Voir si tableau ( H depart, M depart , durée court , durée long ) n'est pas mieux
int TabEvt [4][4] =
{
  {17, 49, 1, 2},
  {17, 8, 1, 2},
  {10, 46, 1, 2},
  {14, 3, 1, 2},
};


// définition des pins de sortie
byte PinSorties[6] = {7, 8, 9, 10, 11, 12};

// detecteur de pluie
const int analogInPin = A0; // sur PIN analogique A0
int sensorValue = 0;
boolean modeActif;


void setup () {

  lcd.init();
  lcd.backlight();
  //lcd.setBacklight(LOW);// extinction LED affichage
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
  pinMode(3, INPUT); // BP changement mode arrosage Long/Court

  modeActif = 0;

}
void loop () {

  // test si pluie (sensorValue peut etre entre 0 et 1024)
  sensorValue = analogRead(analogInPin);
  Serial.print("Sensor = " );
  Serial.print(sensorValue * 100 / 1024);
  Serial.println("%");
  lcd.setCursor(11, 0);
  if ( sensorValue > 500) {
    lcd.print("Pluie"); // affichage PLUIE sur LCD
  }
  else {
    lcd.print("*Sec*");
  }


  // pression bouton changement Mode*****************************

  byte changMode = digitalRead(3);
  if (changMode == HIGH && modeActif == 0) {
    Serial.print("changement de mode");
    modeActif = 1 ;
  }
  else if (changMode == HIGH && modeActif == 1) {
    modeActif = 0 ;
  }
  if (modeActif == 0) {
    Serial.println("Long");
    /*// affichage horaires long
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Arrosage Long");
    lcd.setCursor(0,1);
    lcd.print("Depart a :");
    lcd.setCursor(11, 1);
    lcd.print(TabEvt [0][0]);
     */ //
    lcd.setCursor(11, 1);
    lcd.print("Long");
  }
  else {
    Serial.println("Court");
    lcd.setCursor(11, 1);
    lcd.print("Court");
  }

  // pression bouton démarage manuel********************************
  byte demManuel = digitalRead(2);
  if (demManuel == HIGH)
  {
    // démarage d'une séquence manuelle d'arrosage
    Serial.println("PB appuyé");
    lcd.setBacklight(HIGH); // allumage LED affichage
    lcd.clear();// raffraichissement LCD
    lcd.setCursor(0, 0);
    lcd.print("Demarage Manuel"); // mode en cours
    byte j;
    for (j = 0; j < 4; j++) {
      digitalWrite(j + 7, LOW);
      // Afficher Voies actives sur la deuxième ligne du LCD
      lcd.setCursor(0, 1);
      lcd.print("Vanne:");
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
  //lcd.print("Heure: ");
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

  // Programmation********************************************************************************
/*
  // Afficher Voies actives sur la deuxième ligne du LCD
  lcd.setCursor(0, 1);
  lcd.print("Vanne:");

  // Rechercher si heure actuelle est dans dans la plage horaire de la vanne 1 & pluie non detectée
  // valeur "sensorValue" à étaloner
  lcd.setCursor(6, 1);
  if (now.hour() >= TabEvt [0][0] && now.minute() >= TabEvt [0][1] &&  now.hour() <= TabEvt [0][4] && now.minute() < TabEvt [0][5] && sensorValue < 500  && modeActif == 0) {
    lcd.print(1);
    digitalWrite (PinSorties[0], LOW);
  }
  else {
    lcd.print("-");
    digitalWrite (PinSorties[0], HIGH);
  }

  // Rechercher si heure actuelle est dans dans la plage horaire de la voie 2
  lcd.setCursor(6, 1);
  if (now.hour() >= TabEvt [1][0] && now.minute() >= TabEvt [1][1] &&  now.hour() <= TabEvt [1][4] && now.minute() < TabEvt [1][5]  && sensorValue < 500 && modeActif == 0 ) {
    lcd.print(2);
    digitalWrite (PinSorties[1], LOW);
  }
  else {
    lcd.print("-");
    digitalWrite (PinSorties[1], HIGH);
  }

  // Rechercher si heure actuelle est dans dans la plage horaire de la voie 3
  lcd.setCursor(6, 1);
  if (now.hour() >= TabEvt [2][0] && now.minute() >= TabEvt [2][1] &&  now.hour() <= TabEvt [2][4] && now.minute() < TabEvt [2][5]  && sensorValue < 500 && modeActif == 0 ) {
    lcd.print(3);
    digitalWrite (PinSorties[2], LOW);
  }
  else {
    lcd.print("-");
    digitalWrite (PinSorties[2], HIGH);
  }

  // Rechercher si heure actuelle est dans dans la plage horaire de la voie 4
  lcd.setCursor(6, 1);
  if (now.hour() >= TabEvt [3][0] && now.minute() >= TabEvt [3][1] &&  now.hour() <= TabEvt [3][4] && now.minute() < TabEvt [3][5]  && sensorValue < 500 && modeActif == 0 ) {
    lcd.print(4);
    digitalWrite (PinSorties[3], LOW);
  }
  else {
    lcd.print("-");
    digitalWrite (PinSorties[3], HIGH);
  }

  // Pareil pour le mode Court (modeActif==1)***************************************************

  lcd.setCursor(6, 1);
  if (now.hour() >= TabEvt [0][0] && now.minute() >= TabEvt [0][1] &&  now.hour() <= TabEvt [0][2] && now.minute() < TabEvt [0][3] && sensorValue < 500 && modeActif == 1) {
    lcd.print(1);
    digitalWrite (PinSorties[0], LOW);
  }
  else {
    lcd.print("-");
    digitalWrite (PinSorties[0], HIGH);
  }

  // Rechercher si heure actuelle est dans dans la plage horaire de la voie 2
  lcd.setCursor(6, 1);
  if (now.hour() >= TabEvt [1][0] && now.minute() >= TabEvt [1][1] &&  now.hour() <= TabEvt [1][2] && now.minute() < TabEvt [1][3]  && sensorValue < 500 && modeActif == 1 ) {
    lcd.print(2);
    digitalWrite (PinSorties[1], LOW);
  }
  else {
    lcd.print("-");
    digitalWrite (PinSorties[1], HIGH);
  }

  // Rechercher si heure actuelle est dans dans la plage horaire de la voie 3
  lcd.setCursor(6, 1);
  if (now.hour() >= TabEvt [2][0] && now.minute() >= TabEvt [2][1] &&  now.hour() <= TabEvt [2][2] && now.minute() < TabEvt [2][3]  && sensorValue < 500 && modeActif == 1 ) {
    lcd.print(3);
    digitalWrite (PinSorties[2], LOW);
  }
  else {
    lcd.print("-");
    digitalWrite (PinSorties[2], HIGH);
  }

  // Rechercher si heure actuelle est dans dans la plage horaire de la voie 4
  lcd.setCursor(6, 1);
  if (now.hour() >= TabEvt [3][0] && now.minute() >= TabEvt [3][1] &&  now.hour() <= TabEvt [3][2] && now.minute() < TabEvt [3][3]  && sensorValue < 500 && modeActif == 1 ) {
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
// Nouvelle programmation ------------------------------------------------------------------------

// Afficher Voies actives sur la deuxième ligne du LCD
  lcd.setCursor(0, 1);
  lcd.print("Vanne:");

  // Rechercher si heure actuelle est dans dans la plage horaire de la vanne 1 & pluie non detectée
  // valeur "sensorValue" à étaloner
  // avec modeActif Long
  lcd.setCursor(6, 1);
  if (now.hour() == TabEvt [0][0] && now.minute() == TabEvt [0][1] && sensorValue < 500  && modeActif == 0) {
    lcd.print(1);
    digitalWrite (PinSorties[0], LOW);
    delay(60000* int(TabEvt [0][3])); // déterminer mode court ou long
    lcd.print("-");
    digitalWrite (PinSorties[0], HIGH);
  }
 // Rechercher si heure actuelle est dans dans la plage horaire de la vanne 1 & pluie non detectée
  // valeur "sensorValue" à étaloner
  // avec modeActif court
  lcd.setCursor(6, 1);
  if (now.hour() == TabEvt [0][0] && now.minute() == TabEvt [0][1] && sensorValue < 500  && modeActif == 1) {
    lcd.print(1);
    digitalWrite (PinSorties[0], LOW);
    delay(60000* int(TabEvt [0][2])); // déterminer mode court ou court
    lcd.print("-");
    digitalWrite (PinSorties[0], HIGH);
  }
  // Rechercher si heure actuelle est dans dans la plage horaire de la vanne 2 & pluie non detectée
  // valeur "sensorValue" à étaloner
  // avec modeActif Long
  lcd.setCursor(6, 1);
  if (now.hour() == TabEvt [1][0] && now.minute() == TabEvt [1][1] && sensorValue < 500  && modeActif == 0) {
    lcd.print(1);
    digitalWrite (PinSorties[0], LOW);
    delay(60000* int(TabEvt [1][3])); // déterminer mode court ou long
    lcd.print("-");
    digitalWrite (PinSorties[0], HIGH);
  }
 // Rechercher si heure actuelle est dans dans la plage horaire de la vanne 2 & pluie non detectée
  // valeur "sensorValue" à étaloner
  // avec modeActif court
  lcd.setCursor(6, 1);
  if (now.hour() == TabEvt [1][0] && now.minute() == TabEvt [1][1] && sensorValue < 500  && modeActif == 1) {
    lcd.print(1);
    digitalWrite (PinSorties[0], LOW);
    delay(60000* int(TabEvt [1][2])); // déterminer mode court ou court
    lcd.print("-");
    digitalWrite (PinSorties[0], HIGH);
  }
  // Rechercher si heure actuelle est dans dans la plage horaire de la vanne 3 & pluie non detectée
  // valeur "sensorValue" à étaloner
  // avec modeActif Long
  lcd.setCursor(6, 1);
  if (now.hour() == TabEvt [2][0] && now.minute() == TabEvt [2][1] && sensorValue < 500  && modeActif == 0) {
    lcd.print(1);
    digitalWrite (PinSorties[0], LOW);
    delay(60000* int(TabEvt [2][3])); // déterminer mode court ou long
    lcd.print("-");
    digitalWrite (PinSorties[0], HIGH);
  }
 // Rechercher si heure actuelle est dans dans la plage horaire de la vanne 3 & pluie non detectée
  // valeur "sensorValue" à étaloner
  // avec modeActif court
  lcd.setCursor(6, 1);
  if (now.hour() == TabEvt [2][0] && now.minute() == TabEvt [2][1] && sensorValue < 500  && modeActif == 1) {
    lcd.print(1);
    digitalWrite (PinSorties[0], LOW);
    delay(60000* int(TabEvt [2][2])); // déterminer mode court ou court
    lcd.print("-");
    digitalWrite (PinSorties[0], HIGH);
  }
  // Rechercher si heure actuelle est dans dans la plage horaire de la vanne 4 & pluie non detectée
  // valeur "sensorValue" à étaloner
  // avec modeActif Long
  lcd.setCursor(6, 1);
  if (now.hour() == TabEvt [3][0] && now.minute() == TabEvt [3][1] && sensorValue < 500  && modeActif == 0) {
    lcd.print(1);
    digitalWrite (PinSorties[0], LOW);
    delay(60000* int(TabEvt [3][3])); // déterminer mode court ou long
    lcd.print("-");
    digitalWrite (PinSorties[0], HIGH);
  }
 // Rechercher si heure actuelle est dans dans la plage horaire de la vanne 4 & pluie non detectée
  // valeur "sensorValue" à étaloner
  // avec modeActif court
  lcd.setCursor(6, 1);
  if (now.hour() == TabEvt [3][0] && now.minute() == TabEvt [3][1] && sensorValue < 500  && modeActif == 1) {
    lcd.print(1);
    digitalWrite (PinSorties[0], LOW);
    delay(60000* int(TabEvt [3][2])); // déterminer mode court ou court
    lcd.print("-");
    digitalWrite (PinSorties[0], HIGH);
  }
//now.hour() <=  && now.minute() < TabEvt [0][5] 

  // Delais de rafrachissement*************************************
  delay(3000); // retour aubout de 3 s ?
  lcd.clear();// raffraichissement LCD
}
