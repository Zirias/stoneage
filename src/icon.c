#include "icon.h"
#include "icondat.h"

#include <SDL.h>

void
setupSdlApplicationIcon()
{
    SDL_RWops *rw;
    SDL_Surface *icon;
    Uint32 colkey;

    rw = SDL_RWFromMem(resbuild_stoneage_bmp, resbuild_stoneage_bmp_len);
    icon = SDL_LoadBMP_RW(rw, 0);
    SDL_FreeRW(rw);
    colkey = 0;
    memcpy(&colkey, (char *)icon->pixels, icon->format->BytesPerPixel);
    SDL_SetColorKey(icon, SDL_SRCCOLORKEY, colkey);
    SDL_WM_SetIcon(icon, 0);
    SDL_FreeSurface(icon);
}

