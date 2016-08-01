#include "fonts.h"

GFont GOTHIC_14 = NULL;
GFont GOTHIC_18 = NULL;
GFont ROBOTO_21 = NULL;

void font_init() {
  GOTHIC_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  GOTHIC_14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  ROBOTO_21 = fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21);
}

void font_free() {

}
