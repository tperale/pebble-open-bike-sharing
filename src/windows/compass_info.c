#include "compass_info.h"

#include "../globals.h"
#include "../libs/pebble-assist.h"

static Layer* s_direction_layer = NULL;
static GPath* s_arrow = NULL;
static GPoint center;

static void direction_handler (CompassHeadingData heading_data) {
    double tmp = 0;
    if (Stations) {
        double tmp = ((double) TRIG_MAX_ANGLE) * ((double) Stations[current_index].angle / 360);
        LOG("COMPASS redraw : %ld and angle %ld (%lf)", heading_data.magnetic_heading, Stations[current_index].angle, tmp);
    }
    gpath_rotate_to(s_arrow, heading_data.magnetic_heading + (int) tmp);

    /* if(heading_data.compass_status == CompassStatusDataInvalid) { */
    /* } else if (heading_data.compass_status == CompassStatusCalibrating) { */
    /* } */
    layer_mark_dirty(s_direction_layer);
}

static void direction_update_proc(Layer* layer, GContext* ctx) {
    gpath_draw_filled(ctx, s_arrow);
}

static void draw_needle (Layer* window_layer, GRect bounds) {
    /* Layer for the needle update. */
    s_direction_layer = layer_create(bounds);
    layer_set_update_proc(s_direction_layer, direction_update_proc);
    layer_add_child(window_layer, s_direction_layer);
    /* Needle to show the direction. */
    center = GPoint(bounds.size.w / 2, bounds.size.h / 2);
    s_arrow = gpath_create(&INDICATION_ARROW);
    gpath_move_to(s_arrow, center);
}

void destroy_compass() {

}

void create_compass(Layer* window_layer, GRect bounds) {
    compass_service_set_heading_filter(DEG_TO_TRIGANGLE(2));
    compass_service_subscribe(&direction_handler);


    draw_needle(window_layer, bounds);
}
