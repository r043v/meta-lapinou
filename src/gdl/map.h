
#ifndef _GdlMap_
#define _GdlMap_

#include "./gdl.h"

#define arDeep u8//u16
#define arDeepDec 0

#define GetTile_Tile 0
#define GetTile_Size 1
#define GetTile_Px 2

#define scrollUp 1
#define scrollDown 2
#define scrollLeft 4
#define scrollRight 8

//#define maxMapAnims 4
#define maxTilesNb 16

struct gfx {
	const void * gfx;
	int8_t dx, dy;
	int16_t d;
	void (*udraw)(struct gfx *, clDeep *);
	void (*draw)(struct gfx *, int x, int y);
};

void udrawGfx( struct gfx * g, clDeep * s );
void uplayGfx( struct gfx * g, clDeep * s );
void drawGfx( struct gfx * g, int x, int y );
void playGfx( struct gfx * g, int x, int y );
void udrawGfxD( struct gfx * g, clDeep * s );
void uplayGfxD( struct gfx * g, clDeep * s );
void drawGfxD( struct gfx * g, int x, int y );
void playGfxD( struct gfx * g, int x, int y );
void setGfxDrawFn( struct gfx * g, u8 animated, u8 dec );

struct outzone {
	u32 height,width;
	u32 x,y;
	clDeep *start, *end;
};

class Map
{ public:
	outzone * out;
	u32 scrollx, scrolly;
	u32 tileSizex, tileSizey;
	u32 xTileDec, yTileDec;
	u32 sizeInTilex, sizeInTiley, sizeInTile;
	u32 sizeInPixelx, sizeInPixely;
	u32 firstTileBlitx, firstTileBlity;
	u32 uncutDrawx, uncutDrawy;
	u32 pixelMorex, pixelMorey;
	u32 currentDecx, currentDecy;
	u32 maxScrollx, maxScrolly;

	void (*drawTile)( clDeep*, int, int );

//	clDeep ** tileset;
	struct gfx * gfx;
//	struct anim ** Animate[ maxTilesNb ];
//	u16 tileDec[ maxTilesNb ];
	u16 * tilesRealSize;
	u32 tileNumber;
	arDeep * array;

	u32 pixelessx, pixelessy;
	u32 morex, morey;
	u32 tiledrawx ,tiledrawy;

	u32  getTile(u32 x, u32 y, u32 method=GetTile_Tile);
	void setTile(u32 tile, u32 x, u32 y);
	u32  getOutZoneTile(u32 x, u32 y);
	void setOutZoneTile(u32 tile, u32 x, u32 y);
	u32  getScreenTile(u32 x, u32 y);
	void setScreenTile(u32 tile, u32 x, u32 y);

	u32  setScroll(u32 x, u32 y);
	u32  scroll(u32 way, u32 pawa);
	void setOutZone(outzone*out);
	void moveOutZone(u32 x, u32 y);

	void playAnim(struct anim **b, int x, int y, u32 way=0);
	void drawGfm(clDeep *Gfm, int x, int y);

	u32  getDx( int x );
	u32  getDy( int y );

	void setAnimatedTile(u32 tile, struct anim **a);
	void draw(void);
	u32  set(arDeep*array,/*clDeep**tileset*/struct gfx*gfx,u32 tileNumber,u32 tileSizex,u32 tileSizey,u32 sizex,u32 sizey,u32 scrollx,u32 scrolly, outzone*out, u32 copyArray);
	void follow(
		 int x, int y, // follow pos
		 u32 sx, u32 sy, // follow size
		 //u32 speed, // tick for a px
		 Rate * rate,
		 u32 maxUp, u32 maxDown, u32 maxLeft, u32 maxRight,
		 u32 minUp, u32 minDown, u32 minLeft, u32 minRight
	);
};

outzone*createOutzone(outzone* out,u32 x, u32 y, u32 sx, u32 sy);
#endif
