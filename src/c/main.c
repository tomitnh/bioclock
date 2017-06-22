#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer;

static void update_time() {
  
  // Time of Starting Countdown - June 7th, 2017 00:00 UTC
  // http://www.cplusplus.com/reference/ctime/tm/
  //   static struct tm start = {0};
  //   start.tm_hour = 0;   start.tm_min = 0; start.tm_sec = 0;
  //   start.tm_year = 117; start.tm_mon = 6; start.tm_mday = 7;
  //   start.tm_yday = 31 + 28 + 31 + 30 + 31 + 6;    // 157 days
  
  // NumOfYears * DayPerYear + DaySinceJan1
  long start_days = 117 * 365 + 157;
  // Acct for NumOfLeapDay
  start_days += 117/4 + 1;
  const long start_seconds = start_days * SECONDS_PER_DAY; 
  // const long start_seconds = 3705868800;
    
  // Remaining seconds at Start of Countdown
  // calculated after I got out of jail
  static const int start_yspan = 2324;   // Million seconds
  static const int start_dspan = 160000;
  
  // Time of the Presence
  time_t now = time(NULL);
  struct tm *end = localtime(&now);    
  long end_seconds = (end->tm_year * 365 + end->tm_yday + (end->tm_year/4 + 1)) * SECONDS_PER_DAY;
  
  // Seconds since Countdown - painfully calculate remaining 
  // seconds using Integers because of the constrains of 
  // Pebble firmware
  long duration = end_seconds - start_seconds;
  int duration_yspan = duration/1000000;
  int duration_dspan = duration%1000000;
  
  int remains_dspan = 0; int remains_yspan = 0;
  if (start_dspan < duration_dspan) {
    remains_dspan = start_dspan * 10 - duration_dspan;
    remains_yspan = start_yspan - 1 - duration_yspan;
  } else {
    remains_dspan = start_dspan - duration_dspan;
    remains_yspan = start_yspan - duration_yspan;
  }
  // Write the current hours and minutes into a buffer
  static char s_buffer[12];
  snprintf(s_buffer, sizeof(s_buffer), "%d %d", 
             remains_yspan, remains_dspan);
  
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(78, 52), bounds.size.w, 50));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "Tomit's Bioclock");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
}

static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}