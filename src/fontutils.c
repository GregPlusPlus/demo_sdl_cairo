#include "fontutils.h"

uint32_t getFontID(HB_ctx_t *hb_ctx, const char *name) {
    for(uint32_t i = 0; i < hb_ctx->fontsCount; i ++) {
        if(!strcmp(hb_ctx->db[i].name, name)) {
            return i;
        }
    }

    return 0;
}
