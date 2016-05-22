#include "win_main.h"
#include "../globals.h"
#include "win_navigation.h"

static Window* window;

static MenuLayer* s_menu_layer;

#define MAX_NUMBER_OF_STATIONS 1

static char station_name_buffer[32];
static char station_number_buffer[32];

static char* space_info_title[MAX_NUMBER_OF_STATIONS];
static char* space_info_subtitle[MAX_NUMBER_OF_STATIONS];
/* static char** space_info_icons[MAX_NUMBER_OF_MENU]; */
static void (*space_info_callback[MAX_NUMBER_OF_STATIONS]) () = {};


/* ------------------------------------------------------------------------
 *                      MENU HANDLING
 * ------------------------------------------------------------------------
 */

/* @desc Return number of sections.
 *
 * @param {menu_layer} :
 * @param {data} :
 *
 * @return {uint16_t} : Number of sections.
 */
/* - Header.
 * - Navigation.
 */
#define NUMBER_OF_SECTIONS 1
static uint16_t menu_get_num_sections_callback(MenuLayer* menu_layer, void* data) {
    return NUMBER_OF_SECTIONS;
}

/* @desc Get number of "items" by "section".
 *
 * @param {menu_layer} :
 * @param {section_index} :
 * @param {data} :
 */
/* Only 1 image is shown. */
static uint16_t menu_get_num_rows_callback(MenuLayer* menu_layer, uint16_t section_index, void* data) {
    return MAX_NUMBER_OF_STATIONS;
}

/* @desc Get header size.
 *
 * @param {menu_layer} :
 * @param {section_index} :
 * @param {data} :
 */
static int16_t menu_get_header_height_callback(MenuLayer* menu_layer, uint16_t section_index, void* data) {
    return MENU_CELL_BASIC_HEADER_HEIGHT;
}

/* @desc Draw the headers sections.
 *
 * @param {ctx} :
 * @param {cell_layer} :
 * @param {section_index} :
 * @param {data} :
 */
static void menu_draw_header_callback(GContext* ctx, const Layer* cell_layer, uint16_t section_index, void* data) {
    switch (section_index) {
        case 0:
            menu_cell_basic_header_draw(ctx, cell_layer, "Villo stations.");
            break;
        default:
            break;
    }
}

/* @desc Draw items in the section.
 */
static void menu_draw_row_callback(GContext* ctx, const Layer* cell_layer, MenuIndex* cell_index, void* data) {
    menu_cell_basic_draw(ctx, cell_layer, space_info_title[cell_index->row], space_info_subtitle[cell_index->row],NULL);
}

/* @desc Assign functions callback to items.
 */
static void menu_select_callback(MenuLayer* menu_layer, MenuIndex* cell_index, void* data) {
    if (Stations != NULL) {
        win_navigation_show();
    }
}

static void window_load(Window *window) {
  snprintf(station_name_buffer, 32, "Loading...");
  space_info_title[0] = station_name_buffer;

  snprintf(station_number_buffer, 32, "Loading...");
  space_info_subtitle[0] = station_number_buffer;

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer = menu_layer_create(bounds);
  menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
    .get_num_sections = menu_get_num_sections_callback,
    .get_num_rows = menu_get_num_rows_callback,
    .get_header_height = menu_get_header_height_callback,
    .draw_header = menu_draw_header_callback,
    .draw_row = menu_draw_row_callback,
    .select_click = menu_select_callback,
    .get_cell_height = NULL,
  });

  // Bind the menu layer's click config provider to the window for interactivity
  menu_layer_set_click_config_onto_window(s_menu_layer, window);

  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
}

static void window_unload(Window *window) {
  // Destroy GBitmap
  menu_layer_destroy(s_menu_layer);
}

void win_main_init (void) {
  win_navigation_init();

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
}

void win_main_update (void) {
  if (window_stack_get_top_window() != window) {
      win_navigation_update();
      return;
  }

  snprintf(station_name_buffer, 32, "%s", Stations[0].name);
  space_info_title[0] = station_name_buffer;

  snprintf(station_number_buffer, 32, "%lu", Stations[0].distance);
  space_info_subtitle[0] = station_number_buffer;

  layer_mark_dirty(menu_layer_get_layer(s_menu_layer));
  menu_layer_reload_data(s_menu_layer);
}

void win_main_deinit (void) {
  window_destroy(window);
}
