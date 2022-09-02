#include <time.h>
#include <math.h>

#include "init.h"
#include "drawing.h"
#include "fontutils.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

#define FPS 30

void evLoop(SDL_ctx_t *sdl_ctx, HB_ctx_t *hb_ctx);

void draw(cairo_t *cr, SDL_ctx_t *sdl_ctx, HB_ctx_t *hb_ctx);

int main(int argc, char *argv[]) {
    int err = 0;

    SDL_ctx_t sdl_ctx;
    HB_ctx_t hb_ctx;

    if((err = initSDL(&sdl_ctx, FPS))) {
        return err;
    }

    font_db_t font_db[] = {
        { .fn = "fonts/DejaVuSerif.ttf",    .name = "DejaVuSerif"},
        { .fn = "fonts/arial.ttf",          .name  = "Arial"}
    };

    hb_ctx.fontsCount = 2;

    if((err = initHB(&hb_ctx, font_db))) {
        return err;
    }

    startTimer(&sdl_ctx);

    evLoop(&sdl_ctx, &hb_ctx);

    endHB(&hb_ctx);
    endSDL(&sdl_ctx);

    return 0;
}

void evLoop(SDL_ctx_t *sdl_ctx, HB_ctx_t *hb_ctx) {
    while(true) {
        SDL_Event event;

        while(SDL_WaitEvent(&event)) {
            if(event.type == SDL_WINDOWEVENT) {
                switch(event.window.event) {
                    case SDL_WINDOWEVENT_MINIMIZED:
                        stopTimer(sdl_ctx);
                        break;
                    case SDL_WINDOWEVENT_EXPOSED:
                        startTimer(sdl_ctx);
                        break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                            SDL_FreeSurface(sdl_ctx->sdl_surface);
                            sdl_ctx->sdl_surface = NULL;
                            update(sdl_ctx, hb_ctx);
                        break;
                    default:
                    break;
                }
            } else {
                switch(event.type) {
                    case SDL_QUIT:
                        return;
                        break;
                    case SDL_USEREVENT:
                        update(sdl_ctx, hb_ctx);
                        break;
                    default:
                        break;
                }
            }

            fflush(stdout);
        }
    }
}

void draw(cairo_t *cr, SDL_ctx_t *sdl_ctx, HB_ctx_t *hb_ctx) {
    time_t t;
    struct tm *tm;
    double seconds;
    double minutes;
    double hours;

    /* In newer versions of Visual Studio localtime(..) is deprecated. */
    /* Use localtime_s instead. See MSDN. */
    t = time (NULL);
    tm = localtime (&t);

    /* compute the angles for the indicators of our clock */
    seconds = tm->tm_sec * M_PI / 30;
    minutes = (tm->tm_min * 60. + tm->tm_sec) / 3600. * (2 * M_PI);
    hours = (tm->tm_hour * 60. + tm->tm_min) / 720. * (2 * M_PI);

    normalizeCoord(sdl_ctx, cr);

    /* Fill the background with white. */
    cairo_set_source_rgb(cr, 1, 1, 1);
    cairo_paint(cr);

    /* who doesn't want all those nice line settings :) */
    cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
    cairo_set_line_width(cr, 0.1);

    /* clock outline */
    cairo_set_source_rgb (cr, 0, 0, 0);
    cairo_arc(cr, 0, 0, 0.4, 0, M_PI * 2);
    cairo_stroke(cr);

    cairo_set_source_rgba(cr, 1, 1, 1, 1);
    for(uint32_t i = 0; i < 60; i += 1) {
        cairo_arc(cr,
            .4 * sin(i * M_PI / 30), .4 * cos(i * M_PI / 30),
            (i % 5)?.007:.015, 0, M_PI * 2);
        cairo_fill(cr);
    }

    /* draw a white dot on the current second. */
    cairo_set_source_rgba(cr, 1, 1, 1, 0.6);
    cairo_rotate(cr, seconds - M_PI / 2);
    cairo_arc(cr,
        .4, 0,
        0.05, 0, M_PI * 2);
    cairo_fill(cr);
    cairo_rotate(cr, M_PI / 2 - seconds);

    /* draw the minutes indicator */
    cairo_set_source_rgba(cr, 0.2, 0.2, 1, 0.6);
    cairo_move_to(cr, 0, 0);
    cairo_line_to(cr, sin(minutes) * 0.4, -cos(minutes) * 0.4);
    cairo_stroke(cr);

    /* draw the hours indicator      */
    cairo_move_to(cr, 0, 0);
    cairo_line_to(cr, sin(hours) * 0.2, -cos(hours) * 0.2);
    cairo_stroke(cr);

    resetCoord(sdl_ctx, cr);

    int renderer_width;
    int renderer_height;
    SDL_GetRendererOutputSize(sdl_ctx->renderer, &renderer_width, &renderer_height);

    text_prop_t prop = {
        .cr = cr,
        .hb_ctx = hb_ctx,
        .dir = HB_DIRECTION_LTR,
        .color = {.r = .2, .g = .2, .b = .4, .a = .8},
        .pos = {.x = 0, .y = 0},
        .fontSize = 20 * renderer_width / 200,
        .fontID = getFontID(hb_ctx, "Arial")
    };

    char str[100];
    snprintf(str, sizeof(str), "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);

    size2d_t ts = getTextSize(&prop, str);
    
    prop.pos.x = renderer_width / 2 - ts.w / 2;
    prop.pos.y = renderer_height / 2 - ts.h / 2 - renderer_height / 10;
    drawText(&prop, str);

    /*cairo_rectangle(cr, prop.pos.x, prop.pos.y - ts.h, ts.w, ts.h);
    cairo_stroke_preserve(cr);
    //cairo_fill(cr);*/
}
