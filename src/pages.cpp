#include "pages.h"

String lzero(int t) {
    if (t < 10)
        return "0" + (String) t;
    return (String) t;
}


char *dof[] = {"Nd", "Pon", "Wt", "Sr", "Czw", "Pt", "Sob"};

bool b_stat = true;

void toggleBacklight() {
    if (b_stat)
        lcd.noBacklight();
    else
        lcd.backlight();
    b_stat = !b_stat;
}

String getVerbStatus(Status status) {
    switch (status) {
        case ON:
            return "ON";
        case OFF:
            return "OFF";

        case SNOOZE:
            return "DRZM";

        default:
            return "(DEF)";
    }
}

String alarmSummary(alarm a) {

    return (String) lzero(a.hours) + ":" + (String) lzero(a.minutes) + "  " + getVerbStatus(a.status);
}


void mainPage(int act, int snz, int nol) {
    myRTC.updateTime();
    lcd.setCursor(0, 0);
    lcd.print(dof[myRTC.dayofweek]);
    lcd.print(" ");
    lcd.print(lzero(myRTC.hours));
    lcd.print(":");
    lcd.print(lzero(myRTC.minutes));
    lcd.print(" ");
    lcd.print(lzero(myRTC.dayofmonth));
    lcd.print(".");
    lcd.print(lzero(myRTC.month));

    lcd.setCursor(0, 1);
    lcd.print(act);
    lcd.print(" A ");
    if (snz != 0) {
        lcd.print(snz);
        lcd.print(" D ");
    }
    lcd.print("/ ");
    lcd.print(nol);


}

int scrPos;
String lines[21];
int no_lines = 0;

void scrollPos(int pos) {
    scrPos += pos;
    if (scrPos <= 0)
        scrPos = 0;
    else if (scrPos >= no_lines-1)
        scrPos--;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(" " + lines[scrPos]);
        lcd.setCursor(0, 1);
        if (no_lines == 1)
            lcd.print("BRAK ALARMOW");
        else
            lcd.print("*" + lines[scrPos + 1]);

}

void reloadList() {
    scrPos = 0;
    no_lines = 1;
    lines[0] = "Lista alarmow: ";
    for (int i = 1; i <= mem.nol; i++, no_lines++)
        lines[i] = " " + alarmSummary(mem.als[i - 1]);

    scrollPos(0);
}

void deleteAlarm(alarm a) {
    mem.remove(a.id);
    reloadList();
}

void editAlarm(alarm *a) {
    bool back = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alarm: ");
    lcd.print(alarmSummary(*a));
    lcd.setCursor(0, 1);
    lcd.print("Nowa godz: ");
    char nowagodz[4];
    int dl_ng = 0;
    Status ns = a->status;
    while (true) {
        char pKey;
        if ((pKey = kp.getKey())) {
            if (pKey >= '0' && pKey <= '9') {
                if (dl_ng < 4) {
                    lcd.setCursor(12+dl_ng, 1);
                    lcd.print(pKey);
                    nowagodz[dl_ng++] = pKey;
                }
            } else
                switch (pKey) {
                    case '#':
                        if (dl_ng == 4) {
                            a->hours = ((String) nowagodz).substring(0, 2).toInt();
                            a->minutes = ((String) nowagodz).substring(2, 4).toInt();
                        }
                            a->status = ns;
                            mem.update(*a);

                        back = true;
                        break;
                    case '*':
                        ns = ns == ON ? OFF : ns == OFF ? SNOOZE : ON;
                        lcd.setCursor(14, 0);
                        lcd.print(getVerbStatus(ns));
                        break;
                    case 'A':
                        back = true;
                        break;
                }
        }
        if (back)
            break;

    }

}

void enterOnAlarmList(alarm a) {
    if(mem.nol==0)
        return;
    bool back = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Alarm: ");
    lcd.print(alarmSummary(a));
    lcd.setCursor(0, 1);
    lcd.print("C-edytuj  D-usun");
    while (true) {
        char pKey;
        if ((pKey = kp.getKey())) {
            switch (pKey) {
                case 'C':
                    editAlarm(&a);

                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("Alarm: ");
                    lcd.print(alarmSummary(a));
                    lcd.setCursor(0, 1);
                    lcd.print("C-edytuj  D-usun");

                    break;
                case 'D':
                    deleteAlarm(a);
                    back = true;
                    break;
                case 'A':
                    back = true;
                    break;
            }
        }
        if (back)
            break;

    }
}

void addNewAlarm() {
    bool back = false;
    alarm na=alarm(-1,0,0,ON);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Nowy alarm:   ");
    lcd.print(getVerbStatus(na.status));
    lcd.setCursor(0, 1);
    lcd.print("Ustaw godz: ");
    char nowagodz[4];
    int dl_ng = 0;
    while (true) {
        char pKey;
        if ((pKey = kp.getKey())) {
            if (pKey >= '0' && pKey <= '9') {
                if (dl_ng < 4) {
                    lcd.setCursor(12+dl_ng,1);
                    lcd.print(pKey);
                    nowagodz[dl_ng++] = pKey;
                }
            } else
                switch (pKey) {
                    case '#':
                        if (dl_ng == 4) {
                            na.hours = ((String) nowagodz).substring(0, 2).toInt();
                            na.minutes = ((String) nowagodz).substring(2, 4).toInt();
                            mem.addNew(na);
                        }
                        back = true;
                        break;
                    case '*':
                        na.status = na.status == ON ? OFF : na.status == OFF ? SNOOZE : ON;
                        lcd.setCursor(14, 0);
                        lcd.print(getVerbStatus(na.status));
                        break;
                    case 'A':
                        back = true;
                        break;
                }
        }
        if (back)
            break;

    }
}


void alarmList() {
    reloadList();

    while (true) {
        char pKey;
        if ((pKey = kp.getKey())) {
            switch (pKey) {
                case 'C'://up
                    scrollPos(-1);
                    break;
                case 'D'://down
                    scrollPos(1);
                    break;
                case '#':
                    enterOnAlarmList(mem.als[scrPos]);
                    reloadList();
                    break;
                case 'B':
                    addNewAlarm();
                    reloadList();
                    break;
                case 'A':
                    pageId = MAIN;
                    break;
            }
        }
        if (pageId != AL_LIST)
            break;

    }
}

