#include <pebble.h>
#include "device.h"

Device parseDevice(char* lineBuffer)
{
        Device device;
  
        char * pch;
        pch = strtok(lineBuffer,";");
        strcpy(device.name, pch);
        pch = strtok(NULL,";");
        strcpy(device.alias, pch);
        pch = strtok(NULL,";");
        strcpy (device.state, pch);
        pch = strtok(NULL,";");
        strcpy (device.room, pch);
      return device;
}

void deleteDevices(Device *actual)
{
  while(actual!=NULL)
  {
    Device *temp = actual->next;
    free(actual);
    actual =temp;
  }
}

void addDevice(Room *room, Device temp)
{
  //Go to last Item
  Device **lastItem = &room->firstDevice;
  
  Device *deviceToAdd = malloc(sizeof(Device));
  strcpy(deviceToAdd->name, temp.name);
  strcpy(deviceToAdd->alias, temp.alias);
  strcpy(deviceToAdd->room, temp.room);
  strcpy(deviceToAdd->state, temp.state);
  deviceToAdd->next = NULL;
  
  if(*lastItem==NULL)
  {
      *lastItem = deviceToAdd;     
  }

  else
  {
    while((*lastItem)->next!= NULL)
    {
      lastItem = &((*lastItem)->next);
    }
  
  (*lastItem)->next = deviceToAdd; 
  }
}

Device *getDeviceAt(Device *first, int index)
{
  Device *result = first;
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
