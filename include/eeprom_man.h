//
// Created by Grzegorz Jewusiak on 31/01/2022.
//

#ifndef BUDZIKV2_EEPROM_MAN_H
#define BUDZIKV2_EEPROM_MAN_H



enum Status{OFF=0, ON=1, SNOOZE=2, ERR=50, IGNORE=255};
struct alarm{
    alarm(int idi, int hoursi, int minutesi, Status statusi);
    alarm();
    int id = -1; // 0-based
    int hours = 0, minutes = 0;
    Status status = OFF;//0 - OFF, 1 - ON, 2 - SNOOZE, 50 - ERR, 255 - IGNORE

};
struct Memory{
    int nol = 0, nr_on = 0, nr_snz = 0;
    bool changed = true;
    alarm als[20]{};

    void printAls();

    int import();

    bool remove(int id);

    bool update(alarm a);

    bool addNew(alarm a);

    int get_nr(Status status);
};

extern Memory mem;


#endif //BUDZIKV2_EEPROM_MAN_H
