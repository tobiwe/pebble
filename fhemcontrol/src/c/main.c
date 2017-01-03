/*
 * main.c
 * Sets up a Window object and pushes it onto the stack.
 */
#include "main.h"
#include "config.h"


//Buffers
char text_buffer[64];
char device_buffer[64];


// Set the TextLayer's contents depending on locale
char error_DE [] = "Es konnte keine Verbindung zu FHEM hergestellt werden.\nIst Bluetooth aktiviert?\nSind die Einstellungen der App richtig?";
char error_EN [] = "Unable to connect to FHEM.\nIs bluetooth enabbled?\nAre the app settings correct?";


//Pointer to rooms and devices
Device *a_device;
Room *firstRoom;

int room_num = 0;

//Bitmaps
static GBitmap *hue;

//Window
static Window *s_main_window;

//Layers
static MenuLayer *s_menu_layer;
static TextLayer *s_error_layer;

void resetAllDevices()
  {
  Room *start_room = firstRoom;
  while (start_room!=NULL)
  {
    deleteDevices(start_room->firstDevice);
    start_room=start_room->next;
  }
  deleteRooms(start_room);
  firstRoom=NULL;
}

void addDeviceToList(Device temp)
{
  //Raum schon da?
  
  //JA
  //Gerät zu Raum hinzufügen
  bool roomExists = false;
  APP_LOG(APP_LOG_LEVEL_INFO, "1");
  
  Room *start_room = firstRoom;
  Room *result = NULL;
  while (start_room!=NULL)
  {
    APP_LOG(APP_LOG_LEVEL_INFO, "Compare %s to %s", temp.room, start_room->name);
    if(strcmp(temp.room, start_room->name) == 0)  
      {
      roomExists = true;
      result = start_room;
    }
    start_room=start_room->next;
  }
  
  APP_LOG(APP_LOG_LEVEL_INFO, "2");
  
  if(roomExists == false)
    {
      APP_LOG(APP_LOG_LEVEL_INFO, "Kopiere %s an stelle", temp.room);
    
    Room tempRoom;
    strcpy(tempRoom.name,temp.room);
    
    tempRoom.deviceNum = 0;
    
    result = addRoom(firstRoom, tempRoom);
      APP_LOG(APP_LOG_LEVEL_INFO, "Kopiert %s an stelle %d", result->name, room_num);
     
    }
  

  //NEIN
  bool deviceExists=false;
  
   Device *startDevice = result->firstDevice;
  while (startDevice!=NULL)
  {
    APP_LOG(APP_LOG_LEVEL_INFO, "Compare %s to %s", temp.name, startDevice->name);
    if(strcmp(temp.name, startDevice->name) == 0)  
      {
      deviceExists = true;
      strcpy(startDevice->state, temp.state);    
    }
    startDevice=startDevice->next;
  }
  
  
  if(deviceExists==false)
    {
    addDevice(result, temp) ;
    result->deviceNum++;     
  }
}


static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
  return room_num;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
    Room *room = getRoomAt(firstRoom,section_index);
    if(room!=NULL) 
    {
      return room->deviceNum;
    }
    else
    { 
      return 0;
    }
}


/*static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
  if(getRoomAt(firstRoom,section_index)!=NULL)
    {
      menu_cell_basic_header_draw(ctx, cell_layer, getRoomAt(firstRoom,section_index)->name);
  }
}*/

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
  Room *tmp = getRoomAt(firstRoom,cell_index->section);
   if(tmp!=NULL)
    {
  // This is a basic menu item with a title and subtitle
     Device *first =  tmp->firstDevice;
     Device *device = getDeviceAt(first, cell_index->row);
   menu_cell_basic_draw(ctx, cell_layer, device->alias, device->room, NULL);        
  }
}


static void main_window_load(Window *window) {
  // Create Window's child Layers here
   APP_LOG(APP_LOG_LEVEL_INFO, "Start window load");
  
  //Wait until data is here
 resetAllDevices();
    
  // And also load the background
  hue = gbitmap_create_with_resource(RESOURCE_ID_LAMP);
  // Now we prepare to initialize the menu layer
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

   APP_LOG(APP_LOG_LEVEL_INFO, "Before menue layer");
  
  // Create the menu layer
   s_menu_layer = menu_layer_create(bounds);
   APP_LOG(APP_LOG_LEVEL_INFO, "1");
   menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks){
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
//    .get_header_height = PBL_IF_RECT_ELSE(menu_get_header_height_callback, NULL),
//    .draw_header = PBL_IF_RECT_ELSE(menu_draw_header_callback, NULL),
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
   // .get_cell_height = PBL_IF_ROUND_ELSE(get_cell_height_callback,NULL),
  });
  
  //menu_layer_set_highlight_colors(s_menu_layer, GColorFromHEX(0x11FF11), GColorFromHEX(0x000000));
  APP_LOG(APP_LOG_LEVEL_INFO, "2");
  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu_layer, window);
  APP_LOG(APP_LOG_LEVEL_INFO, "3");
  
   // Create the TextLayer with specific bounds
  s_error_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(50, 10), bounds.size.w, PBL_IF_ROUND_ELSE(98,160)));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_error_layer, GColorClear);
  text_layer_set_text_color(s_error_layer, GColorBlack);
  //text_layer_enable_screen_text_flow_and_paging(s_error_layer, 3);
  text_layer_set_overflow_mode(s_error_layer, GTextOverflowModeWordWrap);
  
  // Use selocale() to obtain the system locale for translation
char *sys_locale = setlocale(LC_ALL, "");

  
  if (strcmp("de_DE", sys_locale) == 0) {
 text_layer_set_text(s_error_layer, error_DE);
} else {
  // Fall back to English
  text_layer_set_text(s_error_layer, error_EN);
}

  
  
 
  text_layer_set_font(s_error_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_error_layer, GTextAlignmentCenter);

   
  layer_set_hidden(menu_layer_get_layer(s_menu_layer), true);
   layer_set_hidden(text_layer_get_layer(s_error_layer), false);
  layer_add_child(window_layer, text_layer_get_layer(s_error_layer));
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
  
   APP_LOG(APP_LOG_LEVEL_INFO, "End load");

}

static void main_window_unload(Window *window) {
  // Destroy Window's child Layers here
  APP_LOG(APP_LOG_LEVEL_INFO, "Unload Main Windows");
  resetAllDevices();

}


static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
  int index = cell_index->row;
  int section = cell_index->section;
  
  Room *room = getRoomAt(firstRoom, section);
  Device *first = room->firstDevice;
  Device *device = getDeviceAt(first, index); 
  
  a_device = device;
  generate_view();
}


//Inbox reciever for messages
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
   APP_LOG(APP_LOG_LEVEL_INFO, "Message arrived");
   // Read first item
  Tuple *t = dict_read_first(iterator);
  
  //Reset the actual device number
 // resetAllDevices();
  

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
      case DEVICE:
        APP_LOG(APP_LOG_LEVEL_INFO, "Key DEVICE arrived");
        snprintf(device_buffer, sizeof(device_buffer), "%s", t->value->cstring);
        Device temp = parseDevice(device_buffer);
      
        addDeviceToList(temp);
     
        break;
    default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  Room *start_room = firstRoom;
  int a = 1;
  while (start_room!=NULL)
  {
    a++;
      Device *start_device = start_room->firstDevice;
    while (start_device!=NULL)
  {
      
      APP_LOG(APP_LOG_LEVEL_ERROR, " %s (Name)",start_device->name);
       start_device=start_device->next;
    }
    
    start_room=start_room->next;
  }
  

 layer_set_hidden(menu_layer_get_layer(s_menu_layer), false);
 layer_set_hidden(text_layer_get_layer(s_error_layer), true);
 menu_layer_reload_data(s_menu_layer);
  
  //update
  if(window_stack_get_top_window() == getDetailView())
  {
    update_view();
  }
  //snprintf(state_buffer, sizeof(state_buffer), "%s",a_device->state);
  
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped for reason %i!", reason);
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}



static void init() {
  
  APP_LOG(APP_LOG_LEVEL_INFO, "Start init");
  
  a_device = NULL;
  firstRoom = NULL;
  
   APP_LOG(APP_LOG_LEVEL_INFO, "Devices Null");
  
  //Register input recievers
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

  app_message_open(APP_MESSAGE_INBOX_SIZE_MINIMUM, APP_MESSAGE_OUTBOX_SIZE_MINIMUM);

   APP_LOG(APP_LOG_LEVEL_INFO, "Create windows");
  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  
   APP_LOG(APP_LOG_LEVEL_INFO, "next init");
  window_stack_push(s_main_window, true);
  
  APP_LOG(APP_LOG_LEVEL_INFO, "Reset devices");
  //reset devices
 resetAllDevices();
}

static void deinit() {
  // Destroy main Window
  window_destroy(s_main_window);
 
  resetAllDevices();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
