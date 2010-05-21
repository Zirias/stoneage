#include "icon.h"

#ifdef WIN32

#include <windows.h>
#include <SDL_syswm.h>

void
setupSdlApplicationIcon()
{
    HINSTANCE handle;
    HWND window;
    HICON icon;
    SDL_SysWMinfo wminfo;

    SDL_VERSION(&wminfo.version);
    if (SDL_GetWMInfo(&wminfo) != 1) return;
    handle = GetModuleHandle(0);
    icon = LoadIcon(handle, "SAICO");
    window = wminfo.window;
    SetClassLong(window, GCL_HICON, (LONG) icon);
}

#else

#include <SDL.h>
#include "icondat.h"

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

#endif
