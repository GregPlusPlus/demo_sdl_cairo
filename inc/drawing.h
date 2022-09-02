#ifndef DRAWING_H
#define DRAWING_H

#include "ctx.h"

typedef struct {
    double x;
    double y;
} point_t;

typedef struct {
    double w;
    double h;
} size2d_t;

typedef struct {
    double r;
    double g;
    double b;
    double a;
} color_t;

typedef struct {
    cairo_t *cr;
    HB_ctx_t *hb_ctx;
    hb_direction_t dir;
    color_t color;
    point_t pos;
    double fontSize;
    uint32_t fontID;
} text_prop_t;

void update(SDL_ctx_t *sdl_ctx, HB_ctx_t *hb_ctx);
void normalizeCoord(SDL_ctx_t *sdl_ctx, cairo_t *cr);
void resetCoord(SDL_ctx_t *sdl_ctx, cairo_t *cr);
void drawText(const text_prop_t *prop, const char *str);
size2d_t getTextSize(const text_prop_t *prop, const char *str);

#endif