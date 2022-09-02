#ifndef CTX_H
#define CTX_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>
#include <cairo/cairo.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftadvanc.h>
#include <freetype/ftsnames.h>
#include <freetype/tttables.h>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb-icu.h> /* Alternatively you can use hb-glib.h */
#include <cairo/cairo-ft.h>

typedef struct {
    char const *fn;
    char const *name;
    FT_Face ft_face;
    cairo_font_face_t *cairo_ft_face;
    hb_font_t *hb_ft_font;
    hb_face_t *hb_ft_face;
} font_db_t;

typedef struct {
    uint32_t fps;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Surface *sdl_surface;
    SDL_TimerID timID;
    Uint32 (*timCb)(Uint32 interval, void *param);
} SDL_ctx_t;

typedef struct {
    font_db_t *db;
    uint32_t fontsCount;
    FT_Library ft_library;
} HB_ctx_t;

#endif