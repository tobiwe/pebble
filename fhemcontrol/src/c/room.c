#include <pebble.h>
#include "room.h"

Room *addRoom(Room *first, Room temp)
{
    //Go to last Item
  Room **lastItem = &first;
  
  Room *roomToAdd = malloc(sizeof(Room));
  strcpy(roomToAdd->name, temp.name);
  roomToAdd->deviceNum = temp.deviceNum;
  roomToAdd->next = NULL;
  roomToAdd->firstDevice = NULL;
  
  
  
  if(*lastItem==NULL)
  {
      *lastItem = roomToAdd;
      firstRoom = roomToAdd;
  }

  else
  {
    while((*lastItem)->next!= NULL)
    {
      lastItem = &((*lastItem)->next);
    }
  
  (*lastItem)->next = roomToAdd;
  }
  
   Room *start = first;
    while(start!=NULL)
      {
       start = start->next;
    } 
  room_num++;
  return roomToAdd;
}

void deleteRooms(Room *actual)
{
    
  //Delete room
  while(actual!=NULL)
  {
    //Delete all devices in room
    deleteDevices(actual->firstDevice);
    Room *temp = actual->next;
    free(actual);
    actual =temp;
  }  
  
  room_num=0;
}

Room *getRoomAt(Room *first, int index)
{
   Room *result = first;
  for(int i=0; i<index; i++)
  {
    if(result == NULL)
      {
      return result;
    }
    else
      {
    result = result->next;
    }  
  }
  return result;
}