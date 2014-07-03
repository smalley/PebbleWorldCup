#include <pebble.h>

Window *window;
TextLayer *txtlayer;
	
// Key values for AppMessage Dictionary
enum {
	STATUS_KEY = 0,	
	MESSAGE_KEY = 1,
  GET_UPDATE_KEY = 2,
  SCORESTRING_KEY = 3,
  VIBE_KEY = 4,
};

// Write message to buffer & send
void send_message(void){
	DictionaryIterator *iter;
	
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, STATUS_KEY, 0x1);
  dict_write_uint8(iter, GET_UPDATE_KEY, 0x2);
	
	dict_write_end(iter);
  	app_message_outbox_send();
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *tuple;
	
	tuple = dict_find(received, STATUS_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Status: %d", (int)tuple->value->uint32); 
	}
	
	tuple = dict_find(received, MESSAGE_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received Message: %s", tuple->value->cstring);
	}

 	tuple = dict_find(received, SCORESTRING_KEY);
	if(tuple) {
		APP_LOG(APP_LOG_LEVEL_DEBUG, "Received SCORE: %s", tuple->value->cstring);
    text_layer_set_text(txtlayer, tuple->value->cstring);
	}

 	tuple = dict_find(received, VIBE_KEY);
	if(tuple) {
		vibes_long_pulse();
	}} 

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

void requestScoreUpdate() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Request Score Update\n"); 
  DictionaryIterator *iter;
	Tuplet messType = TupletInteger(GET_UPDATE_KEY, 5);
	app_message_outbox_begin(&iter);
  dict_write_uint8(iter, GET_UPDATE_KEY, 0x2);
  dict_write_tuplet(iter, &messType);
	
	dict_write_end(iter);
  app_message_outbox_send();
}

void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Minute Tick Received\n"); 
  requestScoreUpdate();
}

void init(void) {
	window = window_create();
	window_stack_push(window, true);
	
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
		
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
	send_message();
  
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);
  
  txtlayer = text_layer_create((GRect){ .origin={5,60}, .size={bounds.size.w-5,15}});
  text_layer_set_text(txtlayer, "Getting Update");
  text_layer_set_text_alignment(txtlayer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(txtlayer));
  
  tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) &handle_minute_tick);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Init Done\n"); 
}

void deinit(void) {
	app_message_deregister_callbacks();
	window_destroy(window);
  text_layer_destroy(txtlayer);
  tick_timer_service_unsubscribe();
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}