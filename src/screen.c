#include "screen.h"
#include "resfile.h"
#include "resource.h"
#include "app.h"

static Screen _instance;

struct Screen_impl
{
    Resfile graphics;
    Resource res[SATN_NumberOfTiles];
    SDL_Surface * tiles[SATN_NumberOfTiles][4];

    int off_x;
    int off_y;

    int tile_width;
    int tile_height;
};

static SDL_Surface *
loadPngSurface(Resource r)
{
    SDL_Surface *png;
    SDL_RWops *rw = SDL_RWFromMem((void *)r->getData(r), r->getDataSize(r));
    png = IMG_LoadPNG_RW(rw);
    SDL_FreeRW(rw);
    return png;
}

static SDL_Surface *
rotateSurface(SDL_Surface *s, int rot)
{
    int x,y;
    uint32_t *srcpix, *dstpix;

    SDL_Surface *dest = SDL_CreateRGBSurface(SDL_SWSURFACE,
	    (rot % 2) ? s->w : s->h,
	    (rot % 2) ? s->h : s->w, 
	    32,
	    s->format->Rmask,
	    s->format->Gmask,
	    s->format->Bmask,
	    s->format->Amask);
    SDL_LockSurface(s);
    SDL_LockSurface(dest);
    srcpix = s->pixels;
    dstpix = dest->pixels;
    if (rot == 1)
    {
	for (x=dest->w-1;x>=0;--x) for (y=0;y<dest->h;++y)
	    dstpix[y*dest->w+x] = *srcpix++;
    }
    else if (rot == 2)
    {
	for (y=dest->h-1;y>=0;--y) for (x=dest->w-1;x>=0;--x)
	    dstpix[y*dest->w+x] = *srcpix++;
    }
    else
    {
	for (x=0;x<dest->w;++x) for (y=dest->h-1;y>=0;--y)
	    dstpix[y*dest->w+x] = *srcpix++;
    }
    SDL_UnlockSurface(s);
    SDL_UnlockSurface(dest);
    return dest;
}

static SDL_Surface *
scaleSurface(SDL_Surface *s, int width, int height, int rot)
{
    SDL_Surface *tile;
    SDL_Surface *tmp;

    if (rot)
    {
	tile = rotateSurface(s, rot);
	tmp = zoomSurface (tile, (double)width/64,
		(double)height/64, SMOOTHING_ON);
	SDL_FreeSurface(tile);
    }
    else
    {
	tmp = zoomSurface (s, (double)width/64,
		(double)height/64, SMOOTHING_ON);
    }
    tile = SDL_DisplayFormatAlpha(tmp);
    SDL_FreeSurface(tmp);
    return tile;
}

static SDL_Surface *
createScaledSurface(Resource r, int width, int height)
{
    SDL_Surface *png;
    SDL_Surface *tile;

    png = loadPngSurface(r);
    tile = scaleSurface(png, width, height, 0);
    SDL_FreeSurface(png);
    return tile;
}

static void
freeSurfaces(struct Screen_impl *s)
{
    SDL_surface **tile;
    SDL_surface **tile_end

    tile_end = (SDL_surface **) s->tiles + SATN_NumberOfTiles;
    for (tile = (SDL_surface **) s->tiles; tile != tile_end; ++tile)
    {
	if (*tile) SDL_FreeSurface(*tile);
    }
}

static struct Resfile *
getGraphics(THIS)
{
    METHOD(Screen);

    return this->pimpl->graphics;
}

static const SDL_Surface *
getTile(THIS, int tile, int rotation)
{
    METHOD(Screen);

    struct Screen_impl *s = this->pimpl;
    if (! s->tiles[tile][rotation])
    {
	if (rotation)
	{
	    s->tiles[tile][rotation] = rotateSurface(
		    this->getTile(this, tile, 0), rotation);
	}
	else
	{
	}
    }
    return s->tiles[tile][rotation];
}

CTOR(Screen)
{
    struct Screen_impl *s;
#ifndef SDL_IMG_OLD
    int imgflags;
#endif

    BASECTOR(Screen, Object);

#ifndef SDL_IMG_OLD
    imgflags = IMG_Init(IMG_INIT_PNG);
    if (!(imgflags & IMG_INIT_PNG))
    {
	log_err("SDL_image library is missing PNG support!\n"
		"Try installing libpng and zlib.\n");
	mainApp->abort(mainApp);
    }
#endif

    s = XMALLOC(struct Screen_impl, 1);
    memset(s, 0, sizeof(struct Screen_impl));
    this->pimpl = s;

    s->graphics = NEW(Resfile);
    s->graphics->setFile(s->graphics, RES_GFX);
    if (s->graphics->open(s->graphics, 0) < 0)
    {
	DELETE(Resfile, s->graphics);
	log_err("Error loading graphics from `" RES_GFX "'.\n");
	mainApp->abort(mainApp);
    }

    return this;
}

DTOR(Screen)
{
    freeSurfaces(this->pimpl);
    this->pimpl->graphics->close(this->graphics);
    DELETE(Resfile, this->pimpl->graphics);

#ifndef SDL_IMG_OLD
    IMG_Quit();
#endif

    XFREE(this->pimpl);
    BASEDTOR(Object);
}

Screen
getScreen(void)
{
    if (!_instance) _instance = NEW(Screen);
    return _instance;
}
