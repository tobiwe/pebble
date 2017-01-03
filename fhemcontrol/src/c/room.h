#pragma once
#include "device.h"

typedef struct Room Room;
typedef struct Device Device;


struct Room
{
  char name[20];
  int deviceNum ;
  Device *firstDevice;
  Room *next;
};

extern int room_num;
extern Room *firstRoom;

Room *addRoom(Room *first, Room temp);
void deleteRooms(Room *actual);
Room *getRoomAt(Room *first, int index);