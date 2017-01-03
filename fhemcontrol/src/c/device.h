#pragma once
#include "room.h"

typedef struct Device Device;
typedef struct Room Room;

struct Device
{
  char name[20];
  char alias[20];
  char room[20];
  char state[10];
  Device *next;
};


Device parseDevice(char* lineBuffer);
void addDevice(Room *room, Device temp);
void deleteDevices(Device *actual);
Device *getDeviceAt(Device *first, int index);
