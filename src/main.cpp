#include <Keypad.h>
#include "virtuabotixRTC.h"
#include "LiquidCrystal_I2C.h"
#include "eeprom_man.h"
#include "pages.h"

/*
 * Klawiatura - od lewej: D2, 3, 5, 9, 10, 11, 12, A0
 * RTC - RST - D8, DAT - D7, CLK - D6, VCC - 5V
 * RELAY - VCC - 5V, IN1 - D4
 * LCD - VCC - 5V, SDA - A4, SCL - A5
 */

const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {2, 3, 5, 9};
byte colPins[COLS] = {10, 11, 12, A0};
byte relayPin=4;

Keypad kp = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

virtuabotixRTC myRTC(6, 7, 8);


void setup() {
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, HIGH);
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    lcd.setCursor(1, 0); // Set the cursor on the third column and first row.
    lcd.print("Budzik Grzesia"); // Print the string "Hello World!"
    lcd.setCursor(1, 1); //Set the cursor on the third column and the second row (counting starts at 0!).
    lcd.print("Wczytywanie...");
    mem.import();
    mem.printAls();
    lcd.clear();


}

Pages pageId = MAIN;



void startAlarm(alarm *a){
    digitalWrite(relayPin,LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ALARM DZWONI");
    lcd.setCursor(0,1);
    lcd.print("Przycisnij C");

    while(true)
        if(kp.getKey()=='C'){
            digitalWrite(relayPin, HIGH);
            a->status=OFF;
            mem.update(*a);
            break;
        }


}

void alarmGuard(){
    int hours=myRTC.hours;
    int minutes=myRTC.minutes;
    for(int i=0;i<mem.nol;i++)
        if(mem.als[i].hours==hours&&mem.als[i].minutes== minutes&&mem.als[i].status==ON) {
            startAlarm(&mem.als[i]);
            lcd.clear();
        }

}


void loop() {
alarmGuard();
    switch (pageId) {
        case MAIN:
            mainPage(mem.get_nr(ON), mem.get_nr(SNOOZE), mem.nol);
            char pKey;
            if ((pKey = kp.getKey())) {
                switch (pKey) {
                    case 'A':
                        pageId = AL_LIST;
                        lcd.clear();
                        break;
                    case 'B':
                        pageId = SETTINGS;
                        lcd.clear();
                        break;
                    case '*':
                        toggleBacklight();
                        break;
                }
            }


            break;

        case AL_LIST:
            alarmList();
            lcd.clear();
            break;
        default:
            pageId = MAIN;
            break;
    }


}


