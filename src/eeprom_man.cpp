//
// Created by Grzegorz Jewusiak on 30/01/2022.
//

#include <EEPROM.h>
#include <HardwareSerial.h>
#include "eeprom_man.h"

 Memory mem;

alarm::alarm(int idi, int hoursi, int minutesi, Status statusi) {
    id = idi;
    minutes = minutesi;
    hours = hoursi;
    status = statusi;
}

alarm::alarm() {
    id = -1;
    minutes = hours = 0;
    status = IGNORE;
}


/*
 * Zwraca liczbę alarmów o zadanym statusie.
 */
int Memory::get_nr(Status status) {
    if (!changed) {
        if (status == ON) {
            return nr_on;
        } else if (status == SNOOZE) {
            return nr_snz;
        }
    }
    int lw = 0;
    for (int i = 0; i < nol; i++)
        if (als[i].status == ON)
            lw++;
    nr_on = lw;
    lw = 0;
    for (int i = 0; i < nol; i++)
        if (als[i].status == SNOOZE)
            lw++;
    nr_snz = lw;
    changed = false;
    return get_nr(status);
}

/*
 * Dodaje nowy alarm na koniec listy alarmów.
 * Zwraca false jeżeli lista pełna.
 */
bool Memory::addNew(alarm a) {
    if (nol >= 20)
        return false;
    EEPROM.write(nol * 3 + 1, a.status);
    EEPROM.write(nol * 3 + 2, a.hours);
    EEPROM.write(nol * 3 + 3, a.minutes);
    a.id = nol++;
    als[nol - 1] = a;

    EEPROM.write(0, nol);
    changed=true;

    return true;
}

/*
 * Aktualizuje alarm na podstawie a.id
 * Zwraca false jeżeli id poza zakresem.
 */
bool Memory::update(alarm a) {
    if (a.id >= nol)
        return false;
    EEPROM.write(a.id * 3 + 1, a.status);
    EEPROM.write(a.id * 3 + 2, a.hours);
    EEPROM.write(a.id * 3 + 3, a.minutes);
    changed=true;

    als[a.id] = a;
    return true;
}

/*
 * Usuwa alarm zidentyfikowany na podstawie a.id
 * Zwraca false jeżeli id poza zakresem.
 */
bool Memory::remove(int id) {
    if (id >= nol)
        return false;
    for (int i = id; i < nol - 1; i++) {
        als[i] = als[i + 1];
        als[i].id--;
        if (als[i].id != -1) {
            Serial.println("upd");
            update(als[i]);
        }
    }
    als[--nol] = alarm();

    EEPROM.write(nol * 3 + 1, 255);
    EEPROM.write(nol * 3 + 2, 255);
    EEPROM.write(nol * 3 + 3, 255);
    EEPROM.write(0, nol);
    changed=true;

    return true;
}

/*
 * Zwraca:
 * 0 - pomyślnie
 * 1 - nol > 20 (brak alarmów)
 */
int Memory::import() {
    for (int i = 0; i < 20; i++)
        als[i] = alarm();
    nol = EEPROM.read(0);
    if (nol > 20) {
        nol = 0;
        return 1;
    }
    for (int i = 0; i < nol; i++) {
        als[i].id = i;
        als[i].status = (Status) EEPROM.read(i * 3 + 1);
        als[i].hours = EEPROM.read(i * 3 + 2);
        als[i].minutes = EEPROM.read(i * 3 + 3);
    }
    changed=true;

    return 0;
}

/*
 * Wypisuje na Serial dostępne alarmy w pamięci.
 */
void Memory::printAls() {
    Serial.print("No of als: ");

    Serial.println(nol);
    for (int i = 0; i < nol; i++) {
        Serial.println(als[i].id);

        Serial.print("stat: ");
        Serial.print(als[i].status);
        Serial.print(", ");
        Serial.print(als[i].hours);
        Serial.print(":");
        Serial.println(als[i].minutes);
    }
}
