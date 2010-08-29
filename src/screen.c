#include <SDL_rotozoom.h>
#include <SDL_image.h>

#include "screen.h"
#include "level.h"
#include "resfile.h"
#include "resource.h"
#include "board.h"
#include "app.h"

static Screen _instance = NULL;

static const char *tileNameStrings[] =
{
    "tile_empty",
    "tile_empty_1",
    "tile_empty_2a",
    "tile_empty_2f",
    "tile_empty_3",
    "tile_empty_4",
    "tile_corner",
    "tile_earth",
    "tile_wall",
    "tile_wall_v",
    "tile_wall_h",
    "tile_rock",
    "tile_cabbage",
    "tile_willy"
};

struct Screen_impl
{
    Resource res[SATN_NumberOfTiles];
    SDL_Surface * tiles[SATN_NumberOfTiles][4];

    Board board;

    int tile_width;
    int tile_height;
    int off_x;
    int off_y;
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
    SDL_Surface **tile;
    SDL_Surface **tile_end;

    tile_end = (SDL_Surface **) s->tiles + SATN_NumberOfTiles;
    for (tile = (SDL_Surface **) s->tiles; tile != tile_end; ++tile)
    {
	if (*tile) SDL_FreeSurface(*tile);
	*tile = NULL;
    }
}

static const SDL_Surface *
m_getTile(THIS, enum TileName tile, int rotation)
{
    METHOD(Screen);

    struct Screen_impl *s = this->pimpl;
    if (! s->tiles[tile][rotation])
    {
	if (rotation)
	{
	    s->tiles[tile][rotation] = scaleSurface(loadPngSurface(
		    s->res[tile]), s->tile_width, s->tile_height, rotation);
	}
	else
	{
	    s->tiles[tile][rotation] = createScaledSurface(
		    s->res[tile], s->tile_width, s->tile_height);
	}
    }
    return s->tiles[tile][rotation];
}

static Board
m_getBoard(THIS)
{
    METHOD(Screen);

    return this->pimpl->board;
}

static int
m_coordinatesToPixel(THIS, int x, int y, Sint16 *px, Sint16 *py)
{
    METHOD(Screen);

    if ((x<0)||(x>LVL_COLS-1)||(y<0)||(y>LVL_ROWS-1)) return -1;
    *px = this->pimpl->tile_width * x + this->pimpl->off_x;
    *py = this->pimpl->tile_height * y + this->pimpl->off_y;
    return 0;
}

static void
drawBoardFrame(Screen this)
{
    int max_x, max_y, control_top;
    SDL_Rect drawArea;
    const SDL_Surface *frameTile;

    struct Screen_impl *s = this->pimpl;
    SDL_Surface *sf = SDL_GetVideoSurface();

    max_x = LVL_COLS * s->tile_width + s->off_x;
    max_y = LVL_ROWS * s->tile_height + s->off_y;

    drawArea.w = s->tile_width;
    drawArea.h = s->tile_height / 2;
    frameTile = this->getTile(this, SATN_Wall_h, 0);

    drawArea.y = 0;
    for (drawArea.x = 0; drawArea.x < max_x; drawArea.x += s->tile_width)
	SDL_BlitSurface((SDL_Surface *)frameTile, 0, sf, &drawArea);
    drawArea.y = max_y;
    for (drawArea.x = 0; drawArea.x < max_x; drawArea.x += s->tile_width)
	SDL_BlitSurface((SDL_Surface *)frameTile, 0, sf, &drawArea);

    drawArea.w = s->tile_width / 2;
    drawArea.h = s->tile_height;
    frameTile = this->getTile(this, SATN_Wall_v, 0);

    drawArea.x = 0;
    for (drawArea.y = s->off_y; drawArea.y < max_y;
	    drawArea.y += s->tile_height)
	SDL_BlitSurface((SDL_Surface *)frameTile, 0, sf, &drawArea);
    drawArea.x = max_x;
    for (drawArea.y = s->off_y; drawArea.y < max_y;
	    drawArea.y += s->tile_height)
	SDL_BlitSurface((SDL_Surface *)frameTile, 0, sf, &drawArea);

    control_top = (LVL_ROWS + 1) * s->tile_height;
    max_y = control_top + 4 * s->tile_height;

    drawArea.w = s->tile_width;
    frameTile = this->getTile(this, SATN_Wall, 0);

    drawArea.y = max_y;
    for (drawArea.x = 0; drawArea.x < max_x; drawArea.x += s->tile_width)
	SDL_BlitSurface((SDL_Surface *)frameTile, 0, sf, &drawArea);

    drawArea.x = 0;
    for (drawArea.y = control_top; drawArea.y < max_y;
	    drawArea.y += s->tile_height)
	SDL_BlitSurface((SDL_Surface *)frameTile, 0, sf, &drawArea);
    drawArea.x = max_x - s->off_x;
    for (drawArea.y = control_top; drawArea.y < max_y;
	    drawArea.y += s->tile_height)
	SDL_BlitSurface((SDL_Surface *)frameTile, 0, sf, &drawArea);

    SDL_UpdateRect(sf, 0, 0, 0, 0);
}

static void
m_initVideo(THIS)
{
    METHOD(Screen);

    struct Screen_impl *s = this->pimpl;
    SDL_Surface *screen = SDL_GetVideoSurface();

    freeSurfaces(s);

    if (!s->board) s->board = NEW(Board);
    s->tile_width = screen->w / (LVL_COLS + 1);
    s->tile_height = screen->h / (LVL_ROWS + 6);
    s->off_x = s->tile_width / 2;
    s->off_y = s->tile_height / 2;
    s->board->setGeometry(s->board,
	    s->tile_width, s->tile_height, s->off_x, s->off_y);
    s->board->redraw(s->board, 0);
    drawBoardFrame(this);
}

static void
m_startGame(THIS)
{
    METHOD(Screen);

    struct Screen_impl *s = this->pimpl;
    s->board->loadLevel(s->board, 0);
}

CTOR(Screen)
{
    struct Screen_impl *s;
    Resfile rf;
    int i;
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

    this->getTile = &m_getTile;
    this->getBoard = &m_getBoard;
    this->coordinatesToPixel = &m_coordinatesToPixel;
    this->initVideo = &m_initVideo;
    this->startGame = &m_startGame;

    rf = NEW(Resfile);
    rf->setFile(rf, RES_GFX);
    if (rf->open(rf, 0) < 0)
    {
	DELETE(Resfile, rf);
	log_err("Error loading graphics from `" RES_GFX "'.\n");
	mainApp->abort(mainApp);
    }
    for (i = 0; i < SATN_NumberOfTiles; ++i)
    {
	rf->load(rf, tileNameStrings[i], &(s->res[i]));
	if (!s->res[i])
	{
	    DELETE(Resfile, rf);
	    log_err("Error loading tiles.\n");
	    mainApp->abort(mainApp);
	}
    }
    DELETE(Resfile, rf);

    return this;
}

DTOR(Screen)
{
    struct Screen_impl *s = this->pimpl;
    DELETE(Board, s->board);
    freeSurfaces(s);

#ifndef SDL_IMG_OLD
    IMG_Quit();
#endif

    XFREE(s);
    BASEDTOR(Object);
}

Screen
getScreen(void)
{
    if (!_instance) _instance = NEW(Screen);
    return _instance;
}
