#include "init.h"

static Uint32 timCallback(Uint32 interval, void *param) {
    SDL_Event event;
    SDL_UserEvent userevent;

    userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = NULL;
    userevent.data2 = NULL;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);

    return interval;
}

int initSDL(SDL_ctx_t *sdl_ctx, uint32_t fps) {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER)) {
        printf("SDL_Init failed: %s\n", SDL_GetError());

        return -1;
    }

    sdl_ctx->fps = fps;
    sdl_ctx->timCb = timCallback;
    sdl_ctx->timID = 0;

    sdl_ctx->window = SDL_CreateWindow("SDL2 & cairo Clock",
                                        SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED,
                                        200,
                                        200,
                                        SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_RESIZABLE);

    if(!sdl_ctx->window) {
        printf("SDL_CreateWindow failed: %s\n", SDL_GetError());

        return -1;  
    }

    sdl_ctx->renderer = SDL_CreateRenderer(sdl_ctx->window,
                                            -1,
                                            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if(!sdl_ctx->renderer) {
        printf("SDL_CreateRenderer failed: %s\n", SDL_GetError());

        return -1;  
    }

    sdl_ctx->sdl_surface = NULL;

    return 0;
}

void startTimer(SDL_ctx_t *sdl_ctx) {
    if(!sdl_ctx->timID) {
        sdl_ctx->timID = SDL_AddTimer(1000 / sdl_ctx->fps, sdl_ctx->timCb, NULL);
    }
}

void stopTimer(SDL_ctx_t *sdl_ctx) {
    if(sdl_ctx->timID) {
        sdl_ctx->timID = !SDL_RemoveTimer(sdl_ctx->timID);
    }
}

int initHB(HB_ctx_t *hb_ctx, font_db_t *db) {
    int err = 0;
    hb_ctx->db = db;

    if((err = FT_Init_FreeType(&hb_ctx->ft_library))) {
        printf("FT_Init_FreeType failed (%d)", err);

        return -2; 
    }

    for(uint32_t i = 0; i < hb_ctx->fontsCount; i ++) {
        printf("Loading font %s\n", hb_ctx->db[i].fn);
        fflush(stdout);

        if((err = FT_New_Face(hb_ctx->ft_library, hb_ctx->db[i].fn, 0, &hb_ctx->db[i].ft_face))) {
            printf("FT_New_Face failed (%d)", err);

            return -2; 
        }

        /*if((FT_Set_Char_Size(hb_ctx->ft_face[i], fontSize * 64, fontSize * 64, 0, 0))) {
            printf("FT_Set_Char_Size failed (%d)", err);

            return -2; 
        }*/

        hb_ctx->db[i].cairo_ft_face = cairo_ft_font_face_create_for_ft_face(hb_ctx->db[i].ft_face, 0);

        hb_ctx->db[i].hb_ft_font = hb_ft_font_create(hb_ctx->db[i].ft_face, NULL);
        hb_ctx->db[i].hb_ft_face = hb_ft_face_create(hb_ctx->db[i].ft_face, NULL);
    }

    return 0;
}

void endSDL(SDL_ctx_t *sdl_ctx) {
    SDL_FreeSurface(sdl_ctx->sdl_surface);
    SDL_DestroyRenderer(sdl_ctx->renderer);
    SDL_DestroyWindow(sdl_ctx->window);
    SDL_Quit();
}

void endHB(HB_ctx_t *hb_ctx) {
    for (uint32_t i = 0; i < hb_ctx->fontsCount; i ++) {
        cairo_font_face_destroy(hb_ctx->db[i].cairo_ft_face);
        hb_font_destroy(hb_ctx->db[i].hb_ft_font);
        hb_face_destroy(hb_ctx->db[i].hb_ft_face);
    }

    FT_Done_FreeType(hb_ctx->ft_library);
}
