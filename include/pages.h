//
// Created by Grzegorz Jewusiak on 31/01/2022.
//

#ifndef BUDZIKV2_PAGES_H
#define BUDZIKV2_PAGES_H

#include "LiquidCrystal_I2C.h"
#include "virtuabotixRTC.h"
#include "eeprom_man.h"
#include "Keypad.h"

enum Pages{MAIN, AL_LIST, SETTINGS};

extern LiquidCrystal_I2C lcd;
extern virtuabotixRTC myRTC;
extern Keypad kp;
extern Pages pageId;



void mainPage(int, int, int);

void toggleBacklight();
void alarmList();

#endif //BUDZIKV2_PAGES_H
