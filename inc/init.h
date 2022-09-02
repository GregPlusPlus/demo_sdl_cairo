#ifndef INC_H
#define INC_H

#include "ctx.h"

int initSDL(SDL_ctx_t *sdl_ctx, uint32_t fps);
int initHB(HB_ctx_t *hb_ctx, font_db_t *db);

void startTimer(SDL_ctx_t *sdl_ctx);
void stopTimer(SDL_ctx_t *sdl_ctx);

void endSDL(SDL_ctx_t *sdl_ctx);
void endHB(HB_ctx_t *hb_ctx);

#endif