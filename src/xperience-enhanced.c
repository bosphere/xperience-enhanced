#include <pebble.h>

const int SCREEN_WIDTH = 144;
const int SCREEN_HEIGHT = 144; // 168

const int NR_FACES_DEFINED = 2;  // 2
const int NR_FACES_CALCULATED = 4;  // 2
const int NR_DIGITS_DEFINED = 6; // 6
const int NR_LINES_DEFINED = 4;

/*
static double clockFaces[2][6][4][2] =  // SVG coordinates from INKSCAPE
{
	{//5.682203
		// Left
		{{63.762712,69.40678 }, {0.4576271,6.5593219}, {0.4576271,11.745763}, {58.080509,69.40678}},
		{{59.033898,73.830508}, {64.372881,73.830508}, {34.627119,104.0339}, {31.728814,101.28814}},
		{{29.288136,104.18644}, {32.033898,107.23729}, {0,138.50847}, {0.4576271,132.10169}},
		{{7.6271187,77.033898}, {7.6271187,82.830508}, {26.542373,101.74576}, {29.288136,98.847458}},
		{{0.4576271,75.966102}, {3.9661017,78.101695}, {4.2711864,123.55932}, {0.3050848,127.83051}},
		{{0.3050847,16.779661}, {3.9661017,20.59322}, {3.9661017,73.525424}, {0,70.169491}}
	},
	{	
		// Up
		{{6.4067797,1.2203389}, {39.241526,34.665254}, {44.923729,34.627118}, {10.601695,1.2203389}},
		{{16.627119,1.3728812}, {127.0678,1.0677965}, {123.86441,4.2711863}, {19.525424,4.2711863}},
		{{131.94915,1.3728812}, {136.83051,1.3728812}, {104.0339,34.779661}, {99,34.474576}},
		{{49.271186,34.474576}, {93.813559,34.169491}, {90.762712,37.983051}, {53.847458,38.135593}}, 
		{{43.474576,38.440678}, {48.813559,38.440678}, {69.864407,59.644068}, {69.864407,64.983051}},
		{{95.491526,38.288135}, {100.22034,38.288135}, {73.677966,65.135593}, {73.525424,59.644068}}
		
	}
};
*/

static double clockFaces[2][6][4][2] =  // calculated Screen coordinates
{
	{ // Left
    {{65,68},{0,3},{0,9},{59,68}},
    {{59,75},{65,75},{34,106},{31,103}},
    {{27,107},{30,110},{0,140},{0,134}},
    {{7,78},{7,84},{26,103},{29,100}},
    {{0,77},{4,81},{4,125},{0,129}},
    {{0,14},{4,18},{4,74},{0,70}}
	},
	{ // Top
		{{3,0},{38,35},{44,35},{9,0}}, // Top Left
		{{15,0},{127,0},{123,4},{19,4}}, // Top
		{{133,0},{139,0},{105,34},{99,34}}, // Top Right
		{{49,34},{94,34},{90,38},{53,38}}, // Middle
		{{42,39},{48,39},{68,59},{68,65}}, // Bottom Left 
		{{96,38},{102,38},{75,65},{75,59}} // Bottom Right
	}
};



 int hourDigitsTens[3][6] = {
	{-1,  6,  7,  8, 10, 11}, // 0 10er digit
	{-1, -1, -1, -1,  8, 11}, // 1 10er digit 
	{-1, -1,  7,  8,  9, 10}  // 2 10er digit
};

 int hourDigitsOnes[10][6] =	{
	{-1, 18, 19, 20, 22, 23},
	{-1, -1, -1, -1, 20, 23}, // 1 ones digit
	{-1, -1, 18, 19, 21, 23},  // 2 ones digit
	{-1, -1, 19, 20, 21, 23}, // 3
	{-1, -1, -1, 20, 21, 23}, // 4
	{-1, -1, 19, 20, 21, 22}, // 5
	{-1, 18, 19, 20, 21, 22}, // 6
	{-1, -1, -1, 20, 22, 23}, // 7
	{18, 19, 20, 21, 22, 23}, // 8
	{-1, 29, 20, 21, 22, 23} // 9 
};

 int minuteDigitsTens[6][6] = {
	{-1,  0,  1,  2, 4, 5},
	{-1, -1, -1, -1, 4, 5},
	{-1, -1,  0,  1, 3, 4},
	{-1, -1,  0,  1, 2, 3},
	{-1, -1,  1,  2, 3, 5},
	{-1, -1,  0,  2, 3, 5},
};

 int minuteDigitsOnes[10][6] = {
	{-1, 12, 13, 14, 16, 17},
	{-1, -1, -1, -1, 16, 17},
	{-1, -1, 12, 13, 15, 16},
	{-1, -1, 12, 15, 16, 17},
	{-1, -1, -1, 15, 16, 17},
	{-1, -1, 12, 14, 15, 17},
	{-1, 12, 13, 14, 15, 17},
	{-1, -1, 13, 14, 16, 17},
	{12, 13, 14, 15, 16, 17},
	{-1, 12, 14, 15, 16, 17}
};

static bool selectedDigits[24];

static GPathInfo facesDigits[4][6];
static GPath *calculatedFaceDigitPaths[4][6];
static GPoint facesDigitsPoints[4][6][5];

static void determineScreenCoordinates() {
  // Find out minimum and maximum values of coordinate set
  double minX = 0;
  double maxX = 0;
  double minY = 0;
  double maxY = 0;

  bool minMaxInitialized = false;

  for (int screenFace = 0; screenFace < NR_FACES_DEFINED; screenFace++) {
	  for (int digits = 0; digits < NR_DIGITS_DEFINED; digits++) {
		  for (int lines = 0; lines< NR_LINES_DEFINED; lines++) {
			  double x = clockFaces[screenFace][digits][lines][0];
			  double y = clockFaces[screenFace][digits][lines][1];
			  if (!minMaxInitialized) {
				  minMaxInitialized = true;
				  minX = x;
				  maxX = x;
				  minY = y;
				  maxY = y;
			  }
		  
		    if (x<minX) minX = x;
		    if (x>maxX) maxX = x;

		    if (y<minY) minY = y;
		    if (y>maxY) maxY = y;
		  }
	  }
  }


  maxX = 144;
  maxY = 144;
  // Convert coordinates into integers and adjust them to screen size and coordinate system...
  // Source Coordinate system is right and up positive
  // Dest Coordinate system is right and down positive

  // Flip first set horizontal and vertical,
  // Flip second horizontal only
  // double xRangeCoordinateSource = maxX - minX;
  // double yRangeCoordinateSource = maxY - minY;

  for (int screenFace = 0; screenFace < NR_FACES_DEFINED; screenFace++) {
	  for (int digits = 0; digits < NR_DIGITS_DEFINED; digits++) {
  	  facesDigits[screenFace][digits].num_points = 5;
  	  facesDigits[screenFace+2][digits].num_points = 5;
  			  
  	  for (int lines = 0; lines < NR_LINES_DEFINED; lines++) {
  		  double x = clockFaces[screenFace][digits][lines][0];
  		  double y = clockFaces[screenFace][digits][lines][1];
  	  
  		  x-=minX;
  		  y-=minY;
  	  
  		  int iX = x; // (int) ((x * (double)SCREEN_WIDTH) / xRangeCoordinateSource);
  		  int iY = y; //(int) ((y * (double)SCREEN_HEIGHT) / yRangeCoordinateSource);
  	  
  	    // APP_LOG(APP_LOG_LEVEL_DEBUG, "{%d %d}", iX, iY);
  	  
  	    facesDigitsPoints[screenFace][digits][lines].x = iX;
  	    facesDigitsPoints[screenFace][digits][lines].y = iY;		
  	  	
        int iXFlipped = SCREEN_WIDTH-1 - iX;
  	    int iYFlipped = SCREEN_HEIGHT-1 - iY;
  	  			  
  		  if (screenFace == 0)
          facesDigitsPoints[screenFace+2][digits][lines].x = iXFlipped;
  	  	else
          facesDigitsPoints[screenFace+2][digits][lines].x = iX;
  	  
  	    facesDigitsPoints[screenFace+2][digits][lines].y = iYFlipped;			  
  	  }

  	  facesDigitsPoints[screenFace][digits][NR_LINES_DEFINED].x = facesDigitsPoints[screenFace][digits][0].x;
  	  facesDigitsPoints[screenFace][digits][NR_LINES_DEFINED].y = facesDigitsPoints[screenFace][digits][0].y;	
  	  facesDigitsPoints[screenFace+2][digits][NR_LINES_DEFINED].x = facesDigitsPoints[screenFace+2][digits][0].x;
  	  facesDigitsPoints[screenFace+2][digits][NR_LINES_DEFINED].y = facesDigitsPoints[screenFace+2][digits][0].y;	  
  	  
  	  facesDigits[screenFace][digits].points = facesDigitsPoints[screenFace][digits];
  	  facesDigits[screenFace+2][digits].points = facesDigitsPoints[screenFace+2][digits];
  	  
  	  calculatedFaceDigitPaths[screenFace][digits] = gpath_create(&facesDigits[screenFace][digits]);
  	  calculatedFaceDigitPaths[screenFace+2][digits] = gpath_create(&facesDigits[screenFace+2][digits]);
	  }
  }
}

static Window *window;
static Layer *graphicslayer, *bluetooth_layer, *battery_layer;

static Layer *debugLayer;
static GContext *debugCtx;

static bool last_bluetooth;
static int last_battery;

static void draw_graphics(Layer *layer, GContext *ctx) {

  GRect bounds = layer_get_bounds(layer);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
//    graphics_fill_rect(ctx, GRect(0, 0, bounds.size.w, bounds.size.h), 4, GCornersAll);

	debugLayer = layer;
  debugCtx = ctx;

//	APP_LOG(APP_LOG_LEVEL_DEBUG, "DRAWING");
  graphics_context_set_stroke_color(ctx, GColorWhite);

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorWhite);
	
	// GPoint MidStart1 = (GPoint) {144/2,0};
	// GPoint MidEnd1 = (GPoint) {144/2,144/2-1};
	// GPoint MidStart2 = (GPoint) {0, 144/2-1};
	// GPoint MidEnd2 = (GPoint) {144,144/2-1};
	// graphics_draw_line	(ctx, MidStart1 , MidEnd1);
	// graphics_draw_line	(ctx, MidStart2 , MidEnd2);
		
	int count = 0;
	
  for (int screenFace = 0; screenFace < NR_FACES_CALCULATED; screenFace++) {
	  for (int digits = 0; digits < NR_DIGITS_DEFINED ; digits++) {
      gpath_draw_outline(ctx, calculatedFaceDigitPaths[screenFace][digits]);
		  if (selectedDigits[count]) {
			  gpath_draw_filled(ctx, calculatedFaceDigitPaths[screenFace][digits]);  
		  }
		  
		  count++;
	  }
  }
}

static void bluetooth_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(0, 0, 1, 7), 0, GCornerNone);
  graphics_fill_rect(ctx, GRect(21, 2, 1, 7), 0, GCornerNone);

  graphics_fill_rect(ctx, GRect(2, 2, 4, 2), 0, GCornerNone);
  graphics_fill_rect(ctx, GRect(16, 5, 4, 2), 0, GCornerNone);
  
  if(last_bluetooth) {
    graphics_fill_rect(ctx, GRect(2, 4, 18, 1), 0, GCornerNone);
  } else {
    graphics_fill_rect(ctx, GRect(2, 4, 8, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(12, 4, 8, 1), 0, GCornerNone);

    graphics_fill_rect(ctx, GRect(9, 3, 1, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(12, 5, 1, 1), 0, GCornerNone);
  }
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(0, 3, 1, 4), 0, GCornerNone);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_rect(ctx, GRect(1, 0, 21, 10));
  
  if(last_battery >= 10) {
    graphics_fill_rect(ctx, GRect(18, 2, 2, 6), 0, GCornerNone);
  }
  if(last_battery >= 30) {
    graphics_fill_rect(ctx, GRect(12, 2, 5, 6), 0, GCornerNone);
  }
  if(last_battery >= 60) {
    graphics_fill_rect(ctx, GRect(3, 2, 8, 6), 0, GCornerNone); 
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  // GRect bounds = layer_get_bounds(window_layer);

  // Add graphics layer for watchface drawing...
  graphicslayer = layer_create(GRect(0, 0, 144, 144));
  layer_set_update_proc(graphicslayer, draw_graphics);	 
  layer_add_child(window_layer, graphicslayer );

  // init bluetooth state layer
  bluetooth_layer = layer_create(GRect(4, 155, 22, 10));
  layer_set_update_proc(bluetooth_layer, bluetooth_update_proc);
  layer_add_child(window_layer, bluetooth_layer);

  // init battery state layer
  battery_layer = layer_create(GRect(118, 155, 22, 10));
  layer_set_update_proc(battery_layer, battery_update_proc);
  layer_add_child(window_layer, battery_layer);
}

static void window_unload(Window *window) {
  layer_destroy(graphicslayer);
  layer_destroy(bluetooth_layer);
  layer_destroy(battery_layer);
}


void timeChanged(struct tm *tick_time, TimeUnits units_changed) {
//	 APP_LOG(APP_LOG_LEVEL_DEBUG, "%d:%d:%d", tick_time->tm_hour, tick_time->tm_min, tick_time->tm_sec);
	 for (int i=0; i<24; i++) {
		 selectedDigits[i] = false;
	 }
	 
	 int tensHour = tick_time->tm_hour / 10;
	 int onesHour = tick_time->tm_hour - tensHour*10;
	 
	 int tensMins = tick_time->tm_min / 10;
	 int onesMins = tick_time->tm_min - tensMins * 10;
	 
	 for (int i=0; i<6; i++) {
		 int sel = hourDigitsTens[tensHour][i];
		 if (sel!=-1) selectedDigits[sel] = true;
	 }
	 
	 for (int i=0; i<6; i++) {
		 int sel = hourDigitsOnes[onesHour][i];
		 if (sel!=-1) selectedDigits[sel] = true;
	 }
	 
	 for (int i=0; i<6; i++) {
		 int sel = minuteDigitsTens[tensMins][i];
		 if (sel!=-1) selectedDigits[sel] = true;
	 }

	 for (int i=0; i<6; i++) {
		 int sel = minuteDigitsOnes[onesMins][i];
		 if (sel!=-1) selectedDigits[sel] = true;
	 }
	 
	 layer_mark_dirty(debugLayer);
	 
//	 APP_LOG(APP_LOG_LEVEL_DEBUG,"HT: %d HO: %d MT: %d MO: %d", tensHour, onesHour, tensMins, onesMins);
}

static void handle_battery(BatteryChargeState charge_state) {
  last_battery = charge_state.charge_percent;
  layer_mark_dirty(battery_layer);
}

static void handle_bluetooth(bool connected) {
  last_bluetooth = connected;
  layer_mark_dirty(bluetooth_layer);
}

static void init(void) {
	
  determineScreenCoordinates();
	
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_set_fullscreen(window, true);

  window_stack_push(window, animated);

  window_set_background_color(window, GColorBlack);

  handle_battery(battery_state_service_peek());
  last_bluetooth = false;
  handle_bluetooth(bluetooth_connection_service_peek());
  
  tick_timer_service_subscribe(MINUTE_UNIT, timeChanged);
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bluetooth);
}

static void deinit(void) {
  window_destroy(window);

  for (int screenFace = 0; screenFace < NR_FACES_CALCULATED; screenFace++) {
	   for (int digits = 0; digits < NR_DIGITS_DEFINED; digits++) {
   	   	gpath_destroy(calculatedFaceDigitPaths[screenFace][digits]);
	   }
  }
	  
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
}

int main(void) {
  init();
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
