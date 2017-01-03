#pragma once
#include "device.h"

extern Device *a_device;


void generate_view();
//static void my_next_click_handler();
//static void my_previous_click_handler();
void click_config_provider(void *context);
Window *getDetailView();
void update_view();