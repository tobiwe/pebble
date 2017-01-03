#include <pebble.h>
#include "detail.h"
#include "config.h"

//Values for details view:
static Window *s_detail_view;

char room_buffer[64];
char name_buffer[64];
char state_buffer[64];

GBitmap *bitmap_on;
GBitmap *bitmap_off;

char msg_buffer[100];

static TextLayer *name_layer;
static TextLayer *room_layer;
static TextLayer *state_layer;
static ActionBarLayer * action_bar;


Window *getDetailView()
  {
  return s_detail_view;
}

void update_view()
{
  snprintf(room_buffer, sizeof(room_buffer), "(%s)",a_device->room);
  snprintf(state_buffer, sizeof(state_buffer), "%s",a_device->state);
  snprintf(name_buffer, sizeof(name_buffer), "%s",a_device->alias);
  

  text_layer_set_text(name_layer, name_buffer);
  text_layer_set_text(room_layer, room_buffer);
  text_layer_set_text(state_layer, state_buffer);

}

void generate_view()
  {
    // Create main Window
  s_detail_view = window_create();

  /* TODO
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });*/
  
  
  Layer *window_layer = window_get_root_layer(s_detail_view);
  //GRect bounds = layer_get_frame(window_layer);
  //bounds.size.w =  bounds.size.w -30;
 
  
 
  snprintf(room_buffer, sizeof(room_buffer), "(%s)",a_device->room);
  snprintf(state_buffer, sizeof(state_buffer), "%s",a_device->state);
  snprintf(name_buffer, sizeof(name_buffer), "%s",a_device->alias);
  
  #if defined(PBL_ROUND)
  name_layer = text_layer_create(GRect(40,20,100,50));
  room_layer = text_layer_create(GRect(0,131,180,30));
state_layer = text_layer_create(GRect(0,81,180,50));
#elif defined(PBL_RECT)
name_layer = text_layer_create(GRect(0,10,114,50));
room_layer = text_layer_create(GRect(0,61,114,30));
state_layer = text_layer_create(GRect(0,101,114,60));
#endif
  
  
  
  text_layer_set_font(name_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(name_layer, GTextAlignmentCenter);
  text_layer_set_text(name_layer, name_buffer);

  
 
  //text_layer_set_background_color(room_layer, GColorBlack);
 // text_layer_set_text_color(room_layer, GColorWhite);
  text_layer_set_text_alignment(room_layer, GTextAlignmentCenter);
  text_layer_set_font(room_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text(room_layer, room_buffer);
  

  text_layer_set_font(state_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(state_layer, GTextAlignmentCenter);
  text_layer_set_text(state_layer, state_buffer);

 
  
 // text_layer_set_text(device_layer, text_buffer);
  layer_add_child(window_layer, text_layer_get_layer(room_layer));
  layer_add_child(window_layer, text_layer_get_layer(state_layer));
  layer_add_child(window_layer, text_layer_get_layer(name_layer));
  
  
  window_stack_push(s_detail_view, true);
  
   // Initialize the action bar:
  action_bar = action_bar_layer_create();
  // Associate the action bar with the window:
  action_bar_layer_add_to_window(action_bar, s_detail_view);
  // Set the click config provider:
  action_bar_layer_set_click_config_provider(action_bar, click_config_provider);
  
  bitmap_on = gbitmap_create_with_resource(RESOURCE_ID_ON);
  bitmap_off = gbitmap_create_with_resource(RESOURCE_ID_OFF);


  // Set the icons:
  // The loading of the icons is omitted for brevity... See gbitmap_create_with_resource()
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_UP, bitmap_on, true);
  action_bar_layer_set_icon_animated(action_bar, BUTTON_ID_DOWN, bitmap_off, true);

  
  action_bar_layer_set_context(action_bar, window_get_root_layer(s_detail_view));
}

static void my_next_click_handler()
{
  
  //send on command
   // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  //snprintf(msg_buffer, sizeof(msg_buffer), "cmd.%s=set %s %s&XHR=1", a_device->name, a_device->name,"off");
  snprintf(msg_buffer, sizeof(msg_buffer), "%s,%s", a_device->name, "off");
  // Add a key-value pair
  //dict_write_uint8(iter, 0, 0);
  dict_write_cstring(iter, SET, msg_buffer);

  // Send the message!
  app_message_outbox_send();
}
  
static void my_previous_click_handler(){
   //send on command
   // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  //snprintf(msg_buffer, sizeof(msg_buffer), "cmd.%s=set %s %s&XHR=1", a_device->name, a_device->name,"on");
  snprintf(msg_buffer, sizeof(msg_buffer), "%s,%s", a_device->name, "on");
  // Add a key-value pair
  //dict_write_uint8(iter, 0, 0);
  dict_write_cstring(iter, SET, msg_buffer);

  // Send the message!
  app_message_outbox_send();
}

void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) my_next_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) my_previous_click_handler);
}