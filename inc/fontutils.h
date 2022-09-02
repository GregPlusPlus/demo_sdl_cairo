#ifndef FONTUTILS_H
#define FONTUTILS_H

#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftadvanc.h>
#include <freetype/ftsnames.h>
#include <freetype/tttables.h>

#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include <harfbuzz/hb-icu.h> /* Alternatively you can use hb-glib.h */
#include <cairo/cairo-ft.h>

#include "ctx.h"

uint32_t getFontID(HB_ctx_t *hb_ctx, const char *name);

#endif