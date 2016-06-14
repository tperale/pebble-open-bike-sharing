#include "graphics.h"

static inline GRect grect_center_rect(const GRect *rect_a, const GRect *rect_b) {
    return (GRect) {
        .origin = {
            .x = rect_a->origin.x + (rect_a->size.w - rect_b->size.w) / 2,
            .y = rect_a->origin.y + (rect_a->size.h - rect_b->size.h) / 2,
        },
        .size = rect_b->size,
    };
}

void graphics_draw_bitmap_centered(GContext *ctx, const GBitmap *bitmap, const GRect frame) {
    GRect bitmap_bounds = gbitmap_get_bounds(bitmap);
    graphics_draw_bitmap_in_rect(ctx, bitmap, grect_center_rect(&frame, &bitmap_bounds));
}
