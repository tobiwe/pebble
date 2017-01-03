#include <pebble.h>

#define KEY_STATION 0
#define KEY_LINE1 1
#define KEY_LINE2 2
#define KEY_LINE3 3
#define KEY_LINE4 4
#define KEY_LINE5 5
  
struct Line
{
  int type; 
  char lineNumber[5];
  char lineName[20];
  char time[7]; 
  char realtime[6];
  char countDown[6];
};

struct Line line[5];

struct Line parseLine(char* lineBuffer)
{
        struct Line line;
  
        char * pch;
        pch = strtok(lineBuffer,";");
        line.type = atoi(pch);
        pch = strtok(NULL,";");
        strncpy(line.lineNumber, pch, 4);
        pch = strtok(NULL,";");
        strncpy (line.lineName, pch, 16);
        pch = strtok(NULL,";");
        strncpy(line.countDown, pch, 5);
        pch = strtok(NULL,";");
        strncpy(line.time, pch, 6);
        pch = strtok(NULL,";");
        strncpy(line.realtime, pch, 5);
  
        line.lineNumber[4] = '\0';
        line.lineName[16] = '\0';
        line.time[6] = '\0';
        line.realtime[5] = '\0';
        line.countDown[5] = '\0';
      return line;
}

//Static variables for widow and layers
static Window *s_main_window; 
static TextLayer *s_clock_layer;
static TextLayer *s_station_layer;
static TextLayer *s_update_layer;
static TextLayer *s_line_layer[5];
static TextLayer *s_time_layer[5];

  // Store incoming information
static char station_buffer[64];
static char station_layer_buffer[64];

static char recieve_buffer[128];
static char line_buffer[5][64];
static char line_layer_buffer[5][64];

static char time_buffer[6][7];
static char time_layer_buffer[6][7];



//Bitmap
static BitmapLayer *s_background_layer;
static BitmapLayer *s_linebitmap_layer[5];
static GBitmap *s_background_bitmap;
static GBitmap *bus_bitmap;
static GBitmap *tram_bitmap;

void updateAtLine(int pos, Tuple *t)
{
  snprintf(recieve_buffer, sizeof(recieve_buffer), "%s", t->value->cstring);
        line[pos] = parseLine(recieve_buffer);
     snprintf(line_buffer[pos], sizeof(line_buffer[pos]), "%s %s", line[pos].lineNumber, line[pos].lineName);
        if(atoi(line[pos].countDown)<10)
        {
          if(strcmp(line[pos].time,line[pos].realtime)==0)
            {  snprintf(time_buffer[pos], sizeof(time_buffer[pos]), "%smin", line[pos].countDown);}
          else
            {
             snprintf(time_buffer[pos], sizeof(time_buffer[pos]), "%smin!", line[pos].countDown);}
        
        }
      else
        {
        if(strcmp(line[pos].time,line[pos].realtime)==0)
          {
        snprintf(time_buffer[pos], sizeof(time_buffer[pos]), "%s", line[pos].time);
        }
        else
          {
           snprintf(time_buffer[pos], sizeof(time_buffer[pos]), "%s!", line[pos].time);
        }
      }  
       //set_container_image(&s_line1_bitmap, RESOURCE_ID_IMAGE_TRAM,GRect(0, 66,16, 20));
      
      if(line[pos].type == 5 || line[pos].type == 6 ||line[pos].type == 7 )
      {
         bitmap_layer_set_bitmap(s_linebitmap_layer[pos], bus_bitmap);
      }
      else if( line[pos].type == 0 || line[pos].type == 1 ||line[pos].type == 2 ||line[pos].type == 3 ||line[pos].type == 4 )
        {
          bitmap_layer_set_bitmap(s_linebitmap_layer[pos], tram_bitmap);
      }
}

//Inbox reciever for messages
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  
  // Read first item
  Tuple *t = dict_read_first(iterator);

  // For all items
  while(t != NULL) {
    // Which key was received?
    switch(t->key) {
      case KEY_STATION:
        snprintf(station_buffer, sizeof(station_buffer), "%s", t->value->cstring);
        APP_LOG(APP_LOG_LEVEL_INFO, "KEY_STATION arrived");
        break;
    case KEY_LINE1:
        updateAtLine(0, t);    
        APP_LOG(APP_LOG_LEVEL_INFO, "Line1: %s, %s", line[0].lineNumber, line[0].lineName);
      break;
   case KEY_LINE2:
       updateAtLine(1, t);
       APP_LOG(APP_LOG_LEVEL_INFO, "Line2: %s, %s", line[1].lineNumber, line[1].lineName);
      break;
  case KEY_LINE3:
       updateAtLine(2, t); 
       APP_LOG(APP_LOG_LEVEL_INFO, "Line3: %s, %s", line[2].lineNumber, line[2].lineName);
      break;
   case KEY_LINE4:
       updateAtLine(3, t);    
       APP_LOG(APP_LOG_LEVEL_INFO, "Line4: %s, %s", line[3].lineNumber, line[3].lineName);
      break;
    case KEY_LINE5:
       updateAtLine(4, t);    
       APP_LOG(APP_LOG_LEVEL_INFO, "Line5: %s, %s", line[4].lineNumber, line[4].lineName);
      break;
    default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
      break;
    }

    // Look for next item
    t = dict_read_next(iterator);
  }
  
  // Assemble full string and display
  snprintf(station_layer_buffer, sizeof(station_layer_buffer), "%s", station_buffer);
  text_layer_set_text(s_station_layer, station_layer_buffer);
  
 static char buffer[] = "00:00";
   time_t temp = time(NULL); 
   struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

 static char buffer2[] = "Last update: 00:00";
  snprintf(buffer2, sizeof(buffer2), "Last update: %s", buffer);
  text_layer_set_text(s_update_layer,buffer2);
  
  
  for(int i = 0; i<5; i++)
    {
  snprintf(line_layer_buffer[i], sizeof(line_layer_buffer[i]), "%s", line_buffer[i]);
  text_layer_set_text(s_line_layer[i], line_layer_buffer[i]);
  snprintf(time_layer_buffer[i], sizeof(time_layer_buffer[i]), "%s", time_buffer[i]);
  text_layer_set_font(s_time_layer[i], fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_time_layer[i], time_layer_buffer[i]);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void in_recv_handler(DictionaryIterator *iterator, void *context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Something happend!");
    
  
}

//update the time
static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display this time on the TextLayer
  text_layer_set_text(s_clock_layer, buffer);
}

//handler for time service
static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  update_time();
  
  // Get weather update every 60 seconds
if(tick_time->tm_sec % 60 == 0) {
  // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair
  dict_write_uint8(iter, 0, 0);

  // Send the message!
  app_message_outbox_send();
}
}

//Open when new window is created
static void main_window_load(Window *window){
  //configure station layer for heading
  s_station_layer = text_layer_create(GRect(20,32,122,36));  //Start: 0,20 -> size(x,y)
  text_layer_set_background_color(s_station_layer, GColorClear);
  text_layer_set_text_color(s_station_layer, GColorBlack);
  text_layer_set_font(s_station_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
  text_layer_set_text(s_station_layer, "Loading...");
  
  s_update_layer = text_layer_create(GRect(20,150,122,18));  //Start: 0,20 -> size(x,y)
  text_layer_set_background_color(s_update_layer, GColorClear);
  text_layer_set_text_color(s_update_layer, GColorBlack);
  text_layer_set_font(s_update_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_update_layer, "Last update: ");
  
  //configure s_text_layer for line1 information
  //s_line1_layer = text_layer_create(GRect(20,66,124,20));  //Start: 0,20 -> size(x,y)
  
  for(int i= 0; i<5; i++)
    {
  s_line_layer[i] = text_layer_create(GRect(20,64+i*17,87,20));  //Start: 0,20 -> size(x,y)
  text_layer_set_background_color(s_line_layer[i], GColorClear);
  text_layer_set_text_color(s_line_layer[i], GColorBlack);
  text_layer_set_font(s_line_layer[i], fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_line_layer[i], "");
  s_time_layer[i] = text_layer_create(GRect(108,64+i*17,34,20));  //Start: 0,20 -> size(x,y)
  text_layer_set_background_color(s_time_layer[i], GColorClear);
  text_layer_set_text_color(s_time_layer[i], GColorBlack);
  text_layer_set_font(s_time_layer[i], fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(s_time_layer[i], "");
  }
  
  //configure s_clock_layer for actual time
  s_clock_layer = text_layer_create(GRect(0,0,144,30));
  text_layer_set_background_color(s_clock_layer, GColorClear);
  text_layer_set_text_color(s_clock_layer, GColorBlack);
  text_layer_set_font(s_clock_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  text_layer_set_text_alignment(s_clock_layer, GTextAlignmentCenter);
  text_layer_set_text(s_clock_layer, "00:00");
  
  //configure bitmap
  //Create GBitmap, then set to created BitmapLayer
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAND);
  s_background_layer = bitmap_layer_create(GRect(0, 40, 16, 20));
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);

  
  bus_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BUS);
  tram_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_TRAM);
  
  
  for(int i = 0; i<5; i++)
  {
    s_linebitmap_layer[i] = bitmap_layer_create(GRect(2, 64+i*17,16, 20));
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_linebitmap_layer[i]));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_line_layer[i]));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer[i]));
  }
 
  
  //Adding Layers
  //Text
   layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_clock_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_station_layer));
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_update_layer));
  
}

//opemd when window will be unloaded
static void main_window_unload(Window *window)
{
 text_layer_destroy(s_clock_layer); 
 text_layer_destroy(s_station_layer); 
  text_layer_destroy(s_update_layer);
  
 // Destroy GBitmap
 gbitmap_destroy(s_background_bitmap);
 gbitmap_destroy( bus_bitmap);
gbitmap_destroy(tram_bitmap);
  

 // Destroy BitmapLayer
 bitmap_layer_destroy(s_background_layer);

 for(int i=0; i<5;i++)
   {
bitmap_layer_destroy(s_linebitmap_layer[i]);
   text_layer_destroy(s_line_layer[i]);
  text_layer_destroy(s_time_layer[i]);
 }
  
}

//init components
static void init()
  {
  //create a new window
  s_main_window = window_create();
  
  //append handlers to the window
  window_set_window_handlers(s_main_window, (WindowHandlers)
                            {
                              .load = main_window_load,
                              .unload = main_window_unload
                            });
  //show the window (animated = true)
  window_stack_push(s_main_window, true);
  
  //subscripe to the time service
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  //Register callbacks for messages
  // Register callbacks
app_message_register_inbox_received(inbox_received_callback);
app_message_register_inbox_dropped(inbox_dropped_callback);
app_message_register_outbox_failed(outbox_failed_callback);
app_message_register_outbox_sent(outbox_sent_callback);

// Open AppMessage
app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());


}

//deinit program
static void deinit(){
  //memory for window will be released
  window_destroy(s_main_window);
  
}

int main(void){
  init();
  app_event_loop();
  deinit();
}