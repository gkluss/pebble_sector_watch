/*
Sector watch (based on Segment Six by Pebble Corp)
 */

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID {0xF6, 0xF6, 0x0D, 0xC6, 0x44, 0xF9, 0x4E, 0xE8, 0x85, 0x27, 0xA3, 0xD9, 0xCA, 0x71, 0xEE, 0x95}
PBL_APP_INFO(MY_UUID, "Sector", "GKluss", 0x1, 0x0, RESOURCE_ID_IMAGE_MENU_ICON, APP_INFO_WATCH_FACE);

Window window;

Layer minute_display_layer;
Layer hour_display_layer;


const GPathInfo MINUTE_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
//    {-5, -77}, // 80 = radius + fudge; 4 ~= 80*tan(3 degrees); 6 degrees per minute;
//    {5,  -77},
    {0, -73},
    {8, -73},
  }
};
GPath minute_segment_path;

const GPathInfo MINUTE5_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
    {0, -77},
    {39, -67},
  }
};
GPath minute5_segment_path;


const GPathInfo MINUTE_MARKER_PATH_POINTS = {
  2,
  (GPoint []) {
    {0, -77},
    {0, 77},
  }
};
GPath minute_marker_path;


const GPathInfo HOUR_SEGMENT_PATH_POINTS = {
  3,
  (GPoint []) {
    {0, 0},
//    {-13, -50}, // 50 = radius + fudge; 13 ~= 50*tan(15 degrees); 30 degrees per hour;
//    {13,  -50},
    {0, -50},
    {25, -43},
  }
};
GPath hour_segment_path;


void minute_display_layer_update_callback(Layer *me, GContext* ctx) {

  PblTm t;
  get_time(&t);
  unsigned int angle = t.tm_min * 6;
  unsigned int hr_angle = (t.tm_min / 5) *30;	
  GPoint center = grect_center_point(&me->frame);

  // minute background white
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 72);

  // five minute sector black 
  graphics_context_set_fill_color(ctx, GColorBlack);
  gpath_rotate_to(&minute5_segment_path, (TRIG_MAX_ANGLE / 360) * hr_angle);  
  gpath_draw_filled(ctx, &minute5_segment_path);
	
  // minute sector white	
  graphics_context_set_fill_color(ctx, GColorWhite);
  gpath_rotate_to(&minute_segment_path, (TRIG_MAX_ANGLE / 360) * angle);	
  gpath_draw_filled(ctx, &minute_segment_path);
  
  // minute markers black	
  graphics_context_set_stroke_color(ctx, GColorBlack);	  
  for (unsigned int i = 0; i < 360; i+=30) {
      gpath_rotate_to(&minute_marker_path, (TRIG_MAX_ANGLE / 360) * i);
      gpath_draw_outline(ctx, &minute_marker_path);
  }

  // black out outer ring	
  graphics_context_set_stroke_color(ctx, GColorBlack);	
  graphics_draw_circle(ctx, center, 73); 	
  graphics_draw_circle(ctx, center, 74); 	
}



void hour_display_layer_update_callback(Layer *me, GContext* ctx) {

  PblTm t;
  get_time(&t);
  unsigned int angle = (t.tm_hour % 12) * 30;
  GPoint center = grect_center_point(&me->frame);
	
  // hour background black
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_circle(ctx, center, 52);
	
  // hour sector white
  graphics_context_set_fill_color(ctx, GColorWhite);
  gpath_rotate_to(&hour_segment_path, (TRIG_MAX_ANGLE / 360) * angle);
  gpath_draw_filled(ctx, &hour_segment_path);

  // rounding hour "sector"	
  graphics_context_set_stroke_color(ctx, GColorBlack);	
  graphics_draw_circle(ctx, center, 50); 	
  graphics_draw_circle(ctx, center, 49); 	
  graphics_draw_circle(ctx, center, 48); 	
  graphics_draw_circle(ctx, center, 47); 	

/*	
 	graphics_context_set_fill_color(ctx, GColorBlack);
 	graphics_fill_circle(ctx, center, 25);	
*/
}




void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {

  layer_mark_dirty(&minute_display_layer);
  layer_mark_dirty(&hour_display_layer);
}


void handle_init(AppContextRef ctx) {

  window_init(&window, "Sector watch");
  window_stack_push(&window, true);
  window_set_background_color(&window, GColorBlack);


  // Init the layer for the minute display
  layer_init(&minute_display_layer, window.layer.frame);
  minute_display_layer.update_proc = &minute_display_layer_update_callback;
  layer_add_child(&window.layer, &minute_display_layer);

  // Init the minute segment path
  gpath_init(&minute_segment_path, &MINUTE_SEGMENT_PATH_POINTS);
  gpath_move_to(&minute_segment_path, grect_center_point(&minute_display_layer.frame));

  // Init the five-minute segment path
  gpath_init(&minute5_segment_path, &MINUTE5_SEGMENT_PATH_POINTS);
  gpath_move_to(&minute5_segment_path, grect_center_point(&minute_display_layer.frame));
	
  // Init the minute marker path
  gpath_init(&minute_marker_path, &MINUTE_MARKER_PATH_POINTS);
  gpath_move_to(&minute_marker_path, grect_center_point(&minute_display_layer.frame));


	// Init the layer for the hour display
  layer_init(&hour_display_layer, window.layer.frame);
  hour_display_layer.update_proc = &hour_display_layer_update_callback;
  layer_add_child(&window.layer, &hour_display_layer);

  // Init the hour segment path
  gpath_init(&hour_segment_path, &HOUR_SEGMENT_PATH_POINTS);
  gpath_move_to(&hour_segment_path, grect_center_point(&hour_display_layer.frame));


}


void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,

    // Handle time updates
    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }

  };
  app_event_loop(params, &handlers);
}
