
#include "./gdl.h"

outzone*createOutzone(outzone * out, u32 x, u32 y, u32 sx, u32 sy)
{	//outzone*out = (outzone*)malloc(sizeof(outzone));
	out->x = x;
	out->y = y;
	out->start  = xy2scr(x,y);
	out->end    = xy2scr(x+sx-1,y+sy-1);
	out->width  = sx;
	out->height = sy;
	return out;
}

void Map::setOutZone(outzone*out)
{	this->out = out;
	this->uncutDrawx = (out->width)>>xTileDec;
	this->pixelMorex = (out->width)-(uncutDrawx<<xTileDec);
	this->uncutDrawy = (out->height)>>yTileDec;
	this->pixelMorey = (out->height)-(uncutDrawy<<yTileDec);
	this->maxScrollx = (tileSizex*sizeInTilex) - out->width  ;
	this->maxScrolly = (tileSizey*sizeInTiley) - out->height ;
}

void Map::moveOutZone(u32 x, u32 y)
{	this->out->x = x;
	this->out->y = y;
	this->out->start = xy2scr(x,y);
	this->out->end   = xy2scr(x+out->width,y+out->height);
}

u32 Map::set(arDeep*array,/*clDeep**tileset*/ struct gfx*gfx,
	u32 tileNumber,u32 tileSizex,u32 tileSizey,
	u32 sizex,u32 sizey,u32 scrollx,u32 scrolly,
	outzone*out, u32 copyArray
)
{	u32 size = sizex*sizey ;
	if(!array || !/*tileset*/gfx || !size) return 0 ;

	// copy the array (so we'll can modify it)
/*	if(copyArray)
	{	this->array	= (arDeep*)malloc(size<<arDeepDec);
		memcpy(this->array,array,size<<arDeepDec);
	} else*/
	this->array = array;

	//this->tileset = tileset	;
	this->gfx = gfx;
	this->tileNumber = tileNumber ;
	this->tileSizex = tileSizex ;
	this->tileSizey = tileSizey ;
	this->scrollx = scrollx ;
	this->scrolly = scrolly ;
	this->sizeInTilex = sizex;
	this->sizeInTiley = sizey;
	this->sizeInTile = sizex * sizey;
	this->drawTile = &::drawGfm;

	// precompute some usefull value
	xTileDec = computeDec(tileSizex); // tile size is multiple of 2, so can use >> or << instead of / or *
	yTileDec = computeDec(tileSizey);

	this->sizeInPixelx = sizex<<xTileDec;
	this->sizeInPixely = sizey<<yTileDec;
	this->firstTileBlitx = (scrollx>>xTileDec);
	this->firstTileBlity = (scrolly>>yTileDec);
	this->currentDecx = scrollx-(firstTileBlitx<<xTileDec);
	this->currentDecy = scrolly-(firstTileBlity<<yTileDec);

	setOutZone(out);

	this->pixelessx = pixelMorex+currentDecx;
	this->pixelessy = pixelMorey+currentDecy;
	this->morex = pixelessx >= tileSizex;
	this->morey = pixelessy >= tileSizey;

	 if(morex) pixelessx -= tileSizex;
	 if(morey) pixelessy -= tileSizey;

	this->tiledrawx = uncutDrawx+morex+(pixelessx!=0);
	this->tiledrawy = uncutDrawy+morey+(pixelessy!=0);

	//this->Animate = (struct anim *)malloc(tileNumber*sizeof(anim*));
	//memset( this->Animate,0,/*tileNumber*/maxTilesNb*sizeof(anim*));
	//memset( this->tileDec,0,maxTilesNb*sizeof(int) );

/*	this->tilesRealSize = (u16*)malloc( tileNumber * 2 * sizeof(u16) );
	u16* s = tilesRealSize;
	*s++ = 0; *s++ = 0;
	for( u32 t=1; t < tileNumber; t++ ){
		int up, down;
		getGfmRealSize( tileset[t], &up, &down ) ;
		*s++ = (u16)up;
		*s++ = (u16)down;
	}
*/
	return 1;
}

u32 Map::setScroll(u32 x, u32 y)
{	if( x > maxScrollx ) x = maxScrollx;
	if( y > maxScrolly ) y = maxScrolly;
	scrollx = x;
	scrolly = y;
	//printf("%u/%u %u/%u\n", x,maxScrollx,y,maxScrolly );
	return scroll(0,0);
}

u32 Map::scroll(u32 way, u32 pawa)
{	u32 rtn=0;

	if(way && pawa){
		u32 cpt;

		if(way&1)	// up
			for(cpt=0;cpt<pawa;cpt++)
			{	if(scrolly) scrolly--;
				 else rtn |= 1;
			};

		if(way&2)	// down
			for(cpt=0;cpt<pawa;cpt++)
			{	if(scrolly < maxScrolly) scrolly++;
				 else rtn |= 2;
			};

		if(way&4)	// left
			for(cpt=0;cpt<pawa;cpt++)
			{	if(scrollx) scrollx--;
				 else rtn |= 4;
			};

		if(way&8)	// right
			for(cpt=0;cpt<pawa;cpt++)
			{	if(scrollx < maxScrollx) scrollx++;
				 else rtn |= 8;
			};
	}

	this->firstTileBlitx = (scrollx>>xTileDec);
	this->firstTileBlity = (scrolly>>yTileDec);
	this->currentDecx = scrollx-(firstTileBlitx<<xTileDec);
	this->currentDecy = scrolly-(firstTileBlity<<yTileDec);

	this->pixelessx = pixelMorex+currentDecx;
	this->pixelessy = pixelMorey+currentDecy;
	this->morex = pixelessx >= tileSizex;
	this->morey = pixelessy >= tileSizey;

	if(morex) this->pixelessx -= tileSizex;
	if(morey) this->pixelessy -= tileSizey;

	this->tiledrawx = uncutDrawx+morex+(pixelessx!=0);
	this->tiledrawy = uncutDrawy+morey+(pixelessy!=0);

	return rtn;
}

void Map::setAnimatedTile(u32 tile,struct anim **a){
	if(!tile || !a || tile >= tileNumber) return ;
	//Animate[tile] = a;
}

u32 Map::getDx( int x ){ return x - ( ( x >> this->xTileDec ) << this->xTileDec ); }
u32 Map::getDy( int y ){ return y - ( ( y >> this->yTileDec ) << this->yTileDec ); }

void Map::draw(void)
{	internalSaveBlitLimit();

//	prints( 2,10, "%u %u", out->x, out->width );

	if(out) setBlitLimit(out->start,out->end,out->x,out->x+out->width);
	 else fullBlitLimit();

//	prints( 2,10, "%u %u", out->x, out->width );

			clDeep *screen = out->start;
			clDeep *scr ;

			u32 cptx,cpty;
			int blitPosx,blitPosy ;

			u32 tile;

			// left clipped colon (from up to down)
				blitPosx = (out->x) - currentDecx;
				blitPosy = (out->y) - currentDecy;

//	    prints( 2,2, "%i %i %u %u %u %u", blitPosx, blitPosy, firstTileBlitx, firstTileBlity, sizeInTilex, sizeInTiley );

//				prints( 2,0, "%u %u %u %u %i", tiledrawx, uncutDrawx, morex, pixelessx, currentDecx );
//				prints( 2,10, "%u %u %u %u %i", tiledrawy, uncutDrawy, morey, pixelessy, currentDecy );

				for(cpty=0;cpty<tiledrawy;cpty++) // show first colon in clipped
				{	tile = array[firstTileBlitx+sizeInTilex*(firstTileBlity+cpty)];
					if( tile >= tileNumber ){ tile = 0; }

					//prints( 2,1, "%u, %i %i", tile, blitPosx, blitPosy );

					if( tile )
					{	/*if(!Animate[tile])
							::drawGfm(tileset[tile],blitPosx,blitPosy);
						else
							::playAnim(Animate[tile],blitPosx,blitPosy);
*/
						gfx[ tile ].draw( &gfx[ tile ], blitPosx,blitPosy );

					}	blitPosy += tileSizey;
				};


			// right clipped colon
				blitPosx = out->x + out->width - pixelessx;
				blitPosy = out->y - currentDecy;

				for(cpty=0;cpty<tiledrawy;cpty++)
				{	tile = array[firstTileBlitx+tiledrawx-1+sizeInTilex*(firstTileBlity+cpty)];
					if( tile >= tileNumber ){ tile = 0; }

						//prints( 2,9, "%u, %i %i", tile, blitPosx, blitPosy );

					if(tile)
					{	/*if(!Animate[tile])
							::drawGfm(tileset[tile],blitPosx,blitPosy);
						else
							::playAnim(Animate[tile],blitPosx,blitPosy);
*/
						gfx[ tile ].draw( &gfx[ tile ], blitPosx,blitPosy );

					}	blitPosy += tileSizey;
				};

			// up clipped line
				blitPosx = (out->x) - currentDecx + tileSizex;
				blitPosy = (out->y) - currentDecy;

				for(cptx=1;cptx<uncutDrawx+morex;cptx++) // show first colon in clipped
				{
					tile = array[firstTileBlitx+cptx+sizeInTilex*firstTileBlity];
					if( tile >= tileNumber ){ tile = 0; }
					if(tile)
					{	/*if(!Animate[tile])
							::drawGfm(tileset[tile],blitPosx,blitPosy);
						else
							::playAnim(Animate[tile],blitPosx,blitPosy);
*/
						gfx[ tile ].draw( &gfx[ tile ], blitPosx,blitPosy );

					}	blitPosx += tileSizex;
				};

			// down clipped line
				blitPosx = (out->x) - currentDecx + tileSizex;
				blitPosy = (out->y) + out->height - pixelessy;

				for(cptx=1;cptx<uncutDrawx+morex;cptx++)
				{
					tile = array[firstTileBlitx+cptx+sizeInTilex*(firstTileBlity+tiledrawy - 1)];
					if( tile >= tileNumber ){ tile = 0; }
					if(tile)
					{	/*if(!Animate[tile])
							::drawGfm(tileset[tile],blitPosx,blitPosy);
						else
							::playAnim(Animate[tile],blitPosx,blitPosy);
*/
						gfx[ tile ].draw( &gfx[ tile ], blitPosx,blitPosy );

					}	blitPosx += tileSizex;
				};

			// show map center, unclipped
				screen -= currentDecy*bufWidth;
				screen -= currentDecx;

			for(cpty=1;cpty<uncutDrawy+morey;cpty++)
			{	screen += bufWidth<<yTileDec;
				scr = screen;
				//u32 tileOff = firstTileBlitx+1;
				for(cptx=1;cptx<uncutDrawx+morex;cptx++)
				{	scr += tileSizex;
					tile = array[firstTileBlitx+cptx+sizeInTilex*(firstTileBlity+cpty)];
					if( tile >= tileNumber ){ tile = 0; }
					if( tile ){
/*						if(!Animate[tile])
							udrawGfm(tileset[tile],scr);
						else
							::playAnim(Animate[tile],scr);
*/
						//udrawGfm( (clDeep*)(gfx[tile]->gfx), scr);
						gfx[ tile ].udraw( &gfx[ tile ], scr );

					}	//tileOff++;
				};
			};

	internalLoadBlitLimit();
}

u32 Map::getTile(u32 x, u32 y, u32 method)
{	if(	x >= sizeInPixelx
	||	y >= sizeInPixely
	) return 0xffff;

	if( method == GetTile_Tile ){
		x >>= xTileDec ;
		y >>= yTileDec ;

		return array[y*sizeInTilex+x];
	}

	// GetTile_Size

	u32 xx = x >> xTileDec ;
	u32 yy = y >> yTileDec ;
	u32 tile = array[yy*sizeInTilex+xx];
//	if( !tile ) return 0;


	return tile;
/*

	u32 dy = y - ( yy << yTileDec ); // y pos in tile

	u8 *g = (u8*)( gfx[ tile ].gfx ),
	*up = g+2,
	*down = g+3
//	*sy = g+1;
	;

	return ( dy < *down || dy > *up ) ? 0 : tile;

*/

/*	u16 * up = &tilesRealSize[ tile * 2 ];//, *down=up+1
//	if( dy < *down || dy > *up ) tile = 0;
//	printf("%u [%u.%u] => %u\n",dy,*up,*down,tile);
//	return tile;

	return ( dy < up[1] || dy > *up ) ? 0 : tile;
*/
}

void Map::setTile(u32 tile, u32 x, u32 y){
	if(	tile >= this->tileNumber
	 ||	x >= sizeInPixelx
	 ||	y >= sizeInPixely
	) return;

	x >>= this->xTileDec ;
	y >>= this->yTileDec ;

	u32 p = y*sizeInTilex+x;
	if( p >= sizeInTile ) return;

	array[ p ] = tile ;
}

u32 Map::getOutZoneTile(u32 x, u32 y){
	return getTile(x+scrollx,y+scrolly);
}

void Map::setOutZoneTile(u32 tile, u32 x, u32 y)
{	if(tile >= this->tileNumber) return ;
	setTile(tile,x+scrollx,y+scrolly);
}

u32 Map::getScreenTile(u32 x, u32 y)
{	if( (x < out->x) || (y < out->y) ) return 0xffff;
	if(   (x >= out->x + out->width)
	   || (y >= out->y + out->height)) return 0xffff;

	int xx = x+scrollx;
	int yy = y+scrolly;
	xx -= currentDecx; xx -= out->x;
	yy -= currentDecy; yy -= out->y;
	return getTile(xx,yy);
}

void Map::setScreenTile(u32 tile, u32 x, u32 y)
{	if(tile >= this->tileNumber) return ;
	setTile(tile,x+scrollx-currentDecx-out->x,y+scrolly-currentDecy-out->y);
}

void Map::playAnim(struct anim **b, int x, int y, u32 way){
	::playAnim( b, x - scrollx, y - scrolly, way ) ;
}

void Map::drawGfm(clDeep *Gfm, int x, int y){
	::drawGfm( Gfm, x - scrollx, y - scrolly ) ;
}

void Map::follow(
	 int x, int y, // follow pos
	 u32 sx, u32 sy, // follow size
	 //u32 speed, // tick for a px
	 Rate * rate,
	 u32 maxUp, u32 maxDown, u32 maxLeft, u32 maxRight, // screen move box
	 u32 minUp, u32 minDown, u32 minLeft, u32 minRight // min screen box
)
{
	//u32 speed = rate->ticks;
	//static TickRate rate( speed );

	//static int last = tick;
	//static u32 acc = 0;

	int scrollx = this->scrollx;
	int scrolly = this->scrolly;

	if( x < 0 ) x = 0;
	if( y < 0 ) y = 0;
	if( x > (int)sizeInPixelx ) x = sizeInPixelx;
	if( y > (int)sizeInPixely ) y = sizeInPixely;

	u32 screenX = (u32)x - ( out->x + scrollx );
	u32 screenY = (u32)y - ( out->y + scrolly );

//	pixel[ screenX + screenY * 320 ] = 0xff00ff;

	u32 screenEndX = screenX + (sx-1);
	u32 screenEndY = screenY + (sy-1);

	minRight = out->width - minRight;
	maxRight = out->width - maxRight;

	minDown = out->height - minDown;
	maxDown = out->height - maxDown;

//	printf("min [%u.%u.%u.%u]\n",minUp,minDown,minLeft,minRight);
//	printf("max [%u.%u.%u.%u]\n",maxUp,maxDown,maxLeft,maxRight);
//	printf("%i,%i -> scroll %u,%u -> screen [%u,%u -> %u,%u]\n",x,y,scrollx,scrolly,screenX,screenY,screenEndX,screenEndY);

	// scroll screen crop X
	if( screenX < minLeft ){
//		printf("left box\n");
		scrollx -= minLeft - screenX;
	} else {
		if( screenEndX > minRight ){
			scrollx += screenEndX - minRight ;
//			printf("right box\n");
		}
	}

	// scroll screen crop Y
	if( screenY < minUp ){
		scrolly -= minUp - screenY ;
//		printf("up box\n");
	} else {
		if( screenEndY > minDown ){
			scrolly += screenEndY - minDown ;
//			printf("down box\n");
		}
	}

//printf("loop : %u\n", rate->update());

u32 px = rate->update();

//acc += tick - last;
//last = tick;

if( px ){// acc >= speed ){
//	u32 px = 0;
//	while( acc >= speed ){ px++; acc -= speed; }

//	printf("scroll %u px\n", px);

		if( screenX < maxLeft ){
//			printf("left slow box\n");
			scrollx -= px ;
		} else {
			if( screenEndX > maxRight ){
//				printf("right slow box\n");
				scrollx += px ;
			}
		}

		if( screenY < maxUp ){
			scrolly -= px ;
//			printf("up slow box\n");
		} else {
			if( screenEndY > maxDown ){
//				printf("down slow box\n");
				scrolly += px ;
			}
		}
	}

	// scroll crop
	if( scrollx < 0 ) scrollx = 0 ;
	if( scrolly < 0 ) scrolly = 0 ;

//	printf("%u scroll %i.%i\n",tick,scrollx,scrolly);

	setScroll( scrollx, scrolly );
//	if(this->scrollx >= this-> msx*32 - 320) this->scrollx =  msx*32 - 321 ;
//  if(this->scrolly > msy*32 - 240) this->scrolly = msy*32 - 240 ;
}


void udrawGfx( struct gfx * g, clDeep * s ){
	udrawGfm( (clDeep*)g->gfx, s );
}

void uplayGfx( struct gfx * g, clDeep * s ){
	playAnim( (struct anim **)g->gfx, s );
}

void drawGfx( struct gfx * g, int x, int y ){
	drawGfm( (clDeep*)g->gfx, x, y );
}

void playGfx( struct gfx * g, int x, int y ){
	playAnim( (struct anim **)g->gfx, x, y );
}

void udrawGfxD( struct gfx * g, clDeep * s ){
	udrawGfm( (clDeep*)g->gfx, s + g->d );
}

void uplayGfxD( struct gfx * g, clDeep * s ){
	playAnim( (struct anim **)g->gfx, s + g-> d );
}

void drawGfxD( struct gfx * g, int x, int y ){
	drawGfm( (clDeep*)g->gfx, x + g->dx, y + g->dy );
}

void playGfxD( struct gfx * g, int x, int y ){
	playAnim( (struct anim **)g->gfx, g->dx, g->dy );
}

void setGfxDrawFn( struct gfx * g, u8 animated, u8 dec ){
	if( dec ){
		if( animated ){
			g->udraw = &uplayGfxD;
			g->draw = &playGfxD;
		} else {
			g->udraw = &udrawGfxD;
			g->draw = &drawGfxD;
		}
	} else {
		if( animated ){
			g->udraw = &uplayGfx;
			g->draw = &playGfx;
		} else {
			g->udraw = &udrawGfx;
			g->draw = &drawGfx;
		}
	}
}
