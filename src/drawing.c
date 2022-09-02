#include "drawing.h"

extern void draw(cairo_t *cr, SDL_ctx_t *sdl_ctx, HB_ctx_t *hb_ctx);

void update(SDL_ctx_t *sdl_ctx, HB_ctx_t *hb_ctx) {
    if(sdl_ctx->sdl_surface == NULL) {
        int renderer_width;
        int renderer_height;
        SDL_GetRendererOutputSize(sdl_ctx->renderer, &renderer_width, &renderer_height);

        sdl_ctx->sdl_surface = SDL_CreateRGBSurface(0,
                                            renderer_width,
                                            renderer_height,
                                            32,
                                            0x00ff0000,
                                            0x0000ff00,
                                            0x000000ff,
                                            0);
    }

    cairo_surface_t *cr_surface = cairo_image_surface_create_for_data((unsigned char *)sdl_ctx->sdl_surface->pixels,
                                                                        CAIRO_FORMAT_RGB24,
                                                                        sdl_ctx->sdl_surface->w,
                                                                        sdl_ctx->sdl_surface->h,
                                                                        sdl_ctx->sdl_surface->pitch);
    cairo_t *cr = cairo_create(cr_surface);

    SDL_SetRenderDrawColor(sdl_ctx->renderer, 0, 0, 0, 0);
    SDL_RenderClear(sdl_ctx->renderer);

    SDL_Texture *texture = SDL_CreateTextureFromSurface(sdl_ctx->renderer, sdl_ctx->sdl_surface);
    SDL_RenderCopy(sdl_ctx->renderer, texture, NULL, NULL);
    SDL_RenderPresent(sdl_ctx->renderer);
    SDL_DestroyTexture(texture);

    draw(cr, sdl_ctx, hb_ctx);

    cairo_destroy(cr);
    cairo_surface_destroy(cr_surface);
}

void normalizeCoord(SDL_ctx_t *sdl_ctx, cairo_t *cr) {
    int renderer_width;
    int renderer_height;
    SDL_GetRendererOutputSize(sdl_ctx->renderer, &renderer_width, &renderer_height);

    cairo_save(cr);
    cairo_scale(cr, (renderer_width<=renderer_height)?renderer_width:renderer_height,
                    (renderer_width<=renderer_height)?renderer_width:renderer_height);

    cairo_translate(cr, (renderer_width>renderer_height)?renderer_width / (double)renderer_height / 2.:.5,
                        (renderer_width<renderer_height)?renderer_height / (double)renderer_width / 2.:.5);
}

void resetCoord(SDL_ctx_t *sdl_ctx, cairo_t *cr) {
    /*int renderer_width;
    int renderer_height;
    SDL_GetRendererOutputSize(sdl_ctx->renderer, &renderer_width, &renderer_height);

    cairo_translate(cr, -0.5, -0.5);
    cairo_scale(cr, 1/(double)((renderer_width<=renderer_height)?renderer_width:renderer_height),
                    1/(double)((renderer_width<=renderer_height)?renderer_width:renderer_height));*/
    
    cairo_restore(cr);
}

void drawText(const text_prop_t *prop, const char *str) {
    if(prop->fontID >= prop->hb_ctx->fontsCount) {
        return;
    }

    hb_buffer_t *buff = hb_buffer_create();

    //alternatively you can use hb_buffer_set_unicode_funcs(buff, hb_glib_get_unicode_funcs());
    hb_buffer_set_unicode_funcs(buff, hb_icu_get_unicode_funcs());
    hb_buffer_set_direction(buff, prop->dir); /* or LTR */
    hb_buffer_set_script(buff, HB_SCRIPT_LATIN); /* see hb-unicode.h */
    //hb_buffer_set_language(buff, hb_language_from_string(languages[i], -1));

    /* Layout the text */
    hb_buffer_add_utf8(buff, str, strlen(str), 0, strlen(str));
    hb_shape(prop->hb_ctx->db[prop->fontID].hb_ft_font, buff, NULL, 0);

    uint32_t glyph_count;
    hb_glyph_info_t     *glyph_info   = hb_buffer_get_glyph_infos(buff, &glyph_count);
    hb_glyph_position_t *glyph_pos    = hb_buffer_get_glyph_positions(buff, &glyph_count);
    cairo_glyph_t       *cairo_glyphs = malloc(sizeof(cairo_glyph_t) * glyph_count);

    double x = prop->pos.x;
    double y = prop->pos.y;

    for(uint32_t i = 0; i < glyph_count; i ++) {
        cairo_glyphs[i].index = glyph_info[i].codepoint;
        cairo_glyphs[i].x = x + (glyph_pos[i].x_offset / 64.0);
        cairo_glyphs[i].y = y - (glyph_pos[i].y_offset / 64.0);
        x += glyph_pos[i].x_advance / 64.0;
        y -= glyph_pos[i].y_advance / 64.0;
    }

    cairo_set_source_rgba(prop->cr, prop->color.r, prop->color.g, prop->color.b, prop->color.a);
    cairo_set_font_face(prop->cr, prop->hb_ctx->db[prop->fontID].cairo_ft_face);
    cairo_set_font_size(prop->cr, prop->fontSize);
    cairo_show_glyphs(prop->cr, cairo_glyphs, glyph_count);

    free(cairo_glyphs);
    hb_buffer_destroy(buff);
}

size2d_t getTextSize(const text_prop_t *prop, const char *str) {
    size2d_t s = {.w = 0, .h = 0};

    if(prop->fontID >= prop->hb_ctx->fontsCount) {
        return s;
    }

    hb_buffer_t *buff = hb_buffer_create();

    //alternatively you can use hb_buffer_set_unicode_funcs(buff, hb_glib_get_unicode_funcs());
    hb_buffer_set_unicode_funcs(buff, hb_icu_get_unicode_funcs());
    hb_buffer_set_direction(buff, prop->dir); /* or LTR */
    hb_buffer_set_script(buff, HB_SCRIPT_LATIN); /* see hb-unicode.h */
    //hb_buffer_set_language(buff, hb_language_from_string(languages[i], -1));

    /* Layout the text */
    hb_buffer_add_utf8(buff, str, strlen(str), 0, strlen(str));
    hb_shape(prop->hb_ctx->db[prop->fontID].hb_ft_font, buff, NULL, 0);

    uint32_t glyph_count;
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(buff, &glyph_count);

    for(uint32_t i = 0; i < glyph_count; i ++) {
        if(prop->dir == HB_DIRECTION_LTR || prop->dir == HB_DIRECTION_RTL) {
            s.w += glyph_pos[i].x_advance / 64;
            s.h = .8 * prop->fontSize;
        } else if(prop->dir == HB_DIRECTION_TTB || prop->dir == HB_DIRECTION_BTT) {
            s.h += glyph_pos[i].y_advance / 64;
            s.w = .8 * prop->fontSize;
        }
    }

    return s;
}
