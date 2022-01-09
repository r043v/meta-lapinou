
#ifndef _Lapin_
#define _Lapin_

#include "./gdl/gdl.h"

#include <functional>

#define TILE_EMPTY 0
#define TILE_CAROT 14
#define TILE_FLOWER 15
#define TILE_ROCK 1

#define TILE_FULL_FAST 4
#define TILE_BROKEN1_FAST 9
#define TILE_BROKEN2_FAST 10
#define TILE_BROKEN3_FAST 13

#define TILE_FULL_MIDDLE 3
#define TILE_BROKEN1_MIDDLE 7
#define TILE_BROKEN2_MIDDLE 8
#define TILE_BROKEN3_MIDDLE 12

#define TILE_FULL_SLOW 2
#define TILE_BROKEN1_SLOW 5
#define TILE_BROKEN2_SLOW 6
#define TILE_BROKEN3_SLOW 11

#define TILE_BROKEN_PART1_FAST 16
#define TILE_BROKEN_PART2_FAST 17
#define TILE_BROKEN_PART3_FAST 18
#define TILE_BROKEN_PART4_FAST 13

#define TILE_BROKEN_PART1_MIDDLE 16
#define TILE_BROKEN_PART2_MIDDLE 17
#define TILE_BROKEN_PART3_MIDDLE 18
#define TILE_BROKEN_PART4_MIDDLE 12

#define TILE_BROKEN_PART1_SLOW 16
#define TILE_BROKEN_PART2_SLOW 17
#define TILE_BROKEN_PART3_SLOW 18
#define TILE_BROKEN_PART4_SLOW 11

#define BREAK_TIME_LANDING 200
#define BREAK_TIME_FAST 100
#define BREAK_TIME_MIDDLE 180
#define BREAK_TIME_SLOW 300

#define CAROT_POWER_LVL100 1
#define CAROT_POWER_LVL500 2
#define CAROT_POWER_LVL1000 3

#define lapinouMinJump 150
#define lapinouJumpFactor 3
#define lapinouInitialVelocity 111
#define lapinouInitialJump 200
//#define lapinouJump 210
#define maxDebris 10
#define powerMax 33

struct tileTransition {
  u8 debrisTile;
  u8 newTile;
  u16 breakTime;
};

const PROGMEM struct tileTransition tileTransitions[ 12 ] = {
  // full slow
  { TILE_BROKEN_PART1_SLOW,
    TILE_BROKEN1_SLOW,
    BREAK_TIME_SLOW
  },
  // full middle
  { TILE_BROKEN_PART1_MIDDLE,
    TILE_BROKEN1_MIDDLE,
    BREAK_TIME_MIDDLE
  },
  // full fast
  { TILE_BROKEN_PART1_FAST,
    TILE_BROKEN1_FAST,
    BREAK_TIME_FAST
  },
  // breaked 1 slow
  { TILE_BROKEN_PART2_SLOW,
    TILE_BROKEN2_SLOW,
    BREAK_TIME_SLOW
  },
  // breaked 2 slow
  { TILE_BROKEN_PART3_SLOW,
    TILE_BROKEN3_SLOW,
    BREAK_TIME_SLOW
  },
  // breaked 1 middle
  { TILE_BROKEN_PART2_MIDDLE,
    TILE_BROKEN2_MIDDLE,
    BREAK_TIME_MIDDLE
  },
  // breaked 2 middle
  { TILE_BROKEN_PART3_MIDDLE,
    TILE_BROKEN3_MIDDLE,
    BREAK_TIME_MIDDLE
  },
  // breaked 1 fast
  { TILE_BROKEN_PART2_FAST,
    TILE_BROKEN2_FAST,
    BREAK_TIME_FAST
  },
  // breaked 2 fast
  { TILE_BROKEN_PART3_FAST,
    TILE_BROKEN3_FAST,
    BREAK_TIME_FAST
  },
  // breaked 3 slow, grass
  { TILE_BROKEN_PART4_SLOW,
    TILE_EMPTY,
    BREAK_TIME_SLOW
  },
  // breaked 3 middle, grass
  { TILE_BROKEN_PART4_MIDDLE,
    TILE_EMPTY,
    BREAK_TIME_MIDDLE
  },
  // breaked 3 fast, grass
  { TILE_BROKEN_PART4_FAST,
    TILE_EMPTY,
    BREAK_TIME_FAST
  }
};

struct Debris
{ clDeep *Gfm ;
  char enable ;
  //int lastTime ;
  int16_t x, y ;
  //int count ;
  Rate velocity;
};

//extern struct buttons buttons;

//extern const PROGMEM clDeep ** tileset;

#define animNb 5
struct anim anims[ animNb ];
struct anim *lapinou;
#define animAr anims

#define Up     (&animAr[0])
#define Fall   (&animAr[1])
#define Walk   (&animAr[2])
#define Stance (&animAr[3])
#define Salto  (&animAr[4])

u32 power = 20, powerDraw = 0;

u32 landingTime=0 ;

Rate mapScrollRate( 66 );
Rate lapinSpeed( 174 );
Rate powerSpeed( 20 );
Rate powerBarSpeed( 60 );

void animSwitch(struct anim **b);
void animFlip(struct anim **b);

int lapinouWay = 0  ; // 0 right 1 left
int lapinouWayY = 0  ; // 0 down 1 up

int lapinouPx = 142 ;
int lapinouPy = 64 ;

Rate lapinouVelocity;

int canDown(void) ;
int canLeft(void) ;
int canRight(void) ;

struct Debris debris[maxDebris] ;
int    debrisNb=0     ;

int score=0, dscore=0 ;

//extern const PROGMEM clDeep **up, **salto, **fall, **walk, **stance;

extern Map m;
/*
class Lapin {// : public Entity {
  public:
    Lapin( void );
    ~Lapin( void );
    void update( void );
    void applyVelocity( void );
    void startFalling( void );
    void startJumping( u32 px );
    void switch2anim(struct anim **a, struct anim *d);
    void animSwitch(struct anim **b);
    void animFlip(struct anim **b);
};
*/
void lapinouStartFalling(void){
  lapinouWayY = 0;
  lapinouVelocity.pxBySecond( lapinouInitialVelocity );
  lapinouVelocity.reset();
}

inline void switch2anim(struct anim **a, struct anim *d)
{  *a=d ; resetAnim(a) ; if(d == Fall) lapinouStartFalling();/* lapinouDwnSpeed=2 ;*/ }

void checkLeftRight( u32 reset = 0 ){
  static u32 lapinouMove = 0;
  if( reset ) lapinouMove = lapinSpeed.update();
  u32 n = lapinouMove;
  while( n-- ){
    if( keyLeft ){//keyArray[kleft] ){
      if( lapinouMove && canLeft() ){
        lapinouPx--;
        lapinouMove--;
      }
      lapinouWay=1;
    }

    if( keyRight ){//keyArray[kright] ){
      if( lapinouMove && canRight() ){
        lapinouPx++;
        lapinouMove--;
      }
      lapinouWay=0;
    }
  }
}

void lapinouApplyVelocity( void ){
  u32 r = lapinouVelocity.update();
  if( !r ) return;

  if( !lapinouWayY ){ // down
    while(r--){
      checkLeftRight();
      if( !canDown() ) return;
      lapinouPy += 1 ;
    };
    lapinouVelocity.pxBySecond( lapinouVelocity.px + 3 );
  } else { // up
    while(r--){
      checkLeftRight();
      lapinouPy -= 1 ;
    };

    checkLeftRight();

    u32 acc = 4;
    u32 newVelocity = lapinouVelocity.px > acc ? lapinouVelocity.px - acc : 0;

    if( newVelocity < 30 ){ // revert way
      lapinouWayY = 0; // down
      lapinouVelocity.pxBySecond( 40 );// lapinouInitialVelocity );
      return;
    }

    lapinouVelocity.pxBySecond( newVelocity );
  }
}

void lapinouStartJumping( u32 px ){
  lapinouWayY = 1;
  lapinouVelocity.pxBySecond( px );
  lapinouVelocity.reset();
}

void drawDebris( void )// int dy )
{ u32 nb = debrisNb ;
  int dy = 0;
  if( !nb ) return;
  struct Debris * d = debris;
  int maxY = m.scrolly + dy + HEIGHT + /*64*/ 128;
  while( 1 ){
    if( !d->enable ){ d++; continue; }
    u32 r = d->velocity.update();
    if( !r ){ m.drawGfm( d->Gfm, d->x, d->y + dy ); goto next; }
    d->y += r;

    if( d->y > maxY ){
      d->enable = 0;
      debrisNb--;
      goto next;
    }

    d->velocity.pxBySecond( d->velocity.px + 3 );

    m.drawGfm( d->Gfm, d->x, d->y + dy );

    next:

    if( --nb == 0 || ++d == &debris[maxDebris] ) return ;
  };
}

void addDebris(int x, int y, const clDeep *Gfm){
  struct Debris * d = debris, *last = &debris[maxDebris];
  while( d->enable && d != last ) d++;

  if( d == last ){
    d = debris ;
    if( debrisNb )
      debrisNb-- ;
  }

  d->Gfm = (clDeep*)Gfm ;
  d->x = (x >> 5) << 5 ;
  d->y = (y >> 5) << 5 ;
  d->velocity.pxBySecond( 32 ) ;
  d->velocity.reset() ;
  debrisNb += 1 ;
  d->enable = 1 ;
}

void catchCarot( u32 x, u32 y ){
  static u32 last = 0 ;
  static u8 level = 0 ;

  if(last + 424 > tick){
    if(level < 2) ++level ;
  } else level = 0 ;

   switch( level ){
     case 0 : score+=100 ; power += CAROT_POWER_LVL100; break ;
     case 1 : score+=500 ; power += CAROT_POWER_LVL500; break ;
     case 2 : score+=1000; power += CAROT_POWER_LVL1000; break ;
   };

   if( power > powerMax ) power = powerMax;

   last = tick ;
   m.setTile( 0, x, y );
   addDebris( x, y, tileset[25+level] ) ;
}

u32 getTile( int x, int y, u32 method = GetTile_Size ){
  x += lapinouPx;
  y += lapinouPy;
  u32 tile = m.getTile( x, y, method );
  if( !tile || tile == 0xffff ) return 0;
  switch( tile ){
    case TILE_CAROT: // carots
      catchCarot( (u32)x, (u32)y );
      return 0;
    break;
    case TILE_FLOWER:
      return 0;
    break;
  };
  return tile;
}

int canLeft(void){
  if( lapinou == Up || lapinou == Salto ) return 1;
  if( lapinou == Fall && ( getTile( 4,  31) ||  getTile( 27, 31) )){ // inside a tile
    return 1;
  }
  return !( getTile( 0, 0) > 1 ||  getTile( 0, 31) > 1 );
}

int canRight(void){
  if( lapinou == Up || lapinou == Salto ) return 1;
  if( lapinou == Fall && ( getTile( 4,  31) ||  getTile( 27, 31) )){ // inside a tile
    return 1;
  }
  return !( getTile( 31, 0) > 1 ||  getTile( 31, 31) > 1 );
}

int canDown(void){
//  if( getTile( 4,  31) || getTile( 27, 31) )//  return 1;
//    if( !( getTile( 4,  32) ||  getTile( 27, 32) ) )
//      return 1;

    if( getTile( 4,  31) ||  getTile( 27, 31) ){ // inside a tile
      //return 1;

      if( ( lapinouPy )&31 ) return 1 ; // if position not multiple of 32 just fall

      // perfect alignment, check tile under foot
//      if( !( (lapinouPy - 1)&31 ) ) // !m.getDy( lapinouPy ) )
        return !( getTile( 4,  32) == TILE_ROCK || getTile( 27, 32) == TILE_ROCK );

//      return 1; // fall
    }

    return !( getTile( 4,  32) ||  getTile( 27, 32) );

  //return !( getTile( 4,  32) ||  getTile( 27, 32) );
  //return 0;
}

u32 isDestructible(void)
{  u32 t ;
   t = getTile( /*4*/ 4 ,  32 ) ; if(t > 1 && t < 14) return 1 ;
   t = getTile( /*27*/ 27, 32 ) ; if(t > 1 && t < 14) return 1 ;
   return 0 ;
}

void lapinouInit( void ){
  lapinou = Up ;
  lapinouStartJumping( lapinouInitialJump );
  lapinouWay = 0 ;
  lapinouPy = m.sizeInPixely + 16;

  for(int c=0;c<maxDebris;c++) debris[c].enable=0 ;
}

void animSwitch(struct anim **b)
{  if(*b == Salto) switch2anim(b,Fall); // salto 2 fall
   else if(*b == Up) switch2anim(b,Salto);
}

void animFlip(struct anim **b){
  /*if(*b != Salto) return;

  static int last=0 ;
  if(last + 42 < tick)
  { int v =((9-(Salto->curentFrm))-4) ;
    if(v<0) { for(int c=0;c>v;c--) if(canDown()) --lapinouPy ; }
    else lapinouPy += v ;
    last = tick ;
  }*/
}

struct tileTransition * currenttr = 0;
int currenttrid = 0;

void lapinouUpdate( void ){
  if( keyDown ){// keyArray[kdown]){
    lapinouPy++;// += lapinouMove ;
  }

  if(power && lapinou != Up && keyA ){
    switch2anim(&lapinou,Up) ;
    u32 jump = lapinouMinJump + lapinouJumpFactor*power;

    if( power > 10 )
      power -= 10;
    else
      power = 0;

    lapinouStartJumping( jump );
  }

  checkLeftRight( 1 );

  if( lapinou == Fall
  ||  lapinou == Salto
  ||  lapinou == Up
  ) lapinouApplyVelocity();

  if(  lapinou == Fall // falling
  || ( lapinou == Salto && Salto->curentFrm > 5 )
  ){
    if(! canDown() ){
      switch2anim(&lapinou,Stance) ;
      landingTime = tick ;
      powerSpeed.reset();
    }
  } else { // walking
    if(lapinou == Stance || lapinou == Walk){
      if( canDown() ){
        switch2anim(&lapinou,Fall) ;
      } else {
        if( keyLeft || keyRight ){
          if(lapinou == Stance)
            switch2anim(&lapinou,Walk) ;
        } else {
          if(lapinou == Walk)
            switch2anim(&lapinou,Stance) ;
        }

        u32 p = powerSpeed.update();
        power += p;
        if( power > powerMax ) power = powerMax;
      }

      if(landingTime + BREAK_TIME_LANDING < tick){
        static u32 lastDestructTime = tick;
        static u8 foot = 1 ;

        u32 time = tick - lastDestructTime;
        if( time > 1024 ) foot = tick & 1;

        int x = 4 + 23 * foot;
        u32 t = getTile( x,  32 ) ;
        if( t < 2 || t > 13 ){ // not destructible, check other foot
          foot ^= 1;
          x = 4 + 23 * foot;
          t = getTile( x,  32 );
          if( t < 2 || t > 13 ) t = 0; // not destructible
        }

        if( t ){ // destructible tile found
          const struct tileTransition * tr = &tileTransitions[ t - 2 ];
          if( time > tr->breakTime ){ // break time reach
            lastDestructTime = tick;
            foot ^= 1;
            x += lapinouPx;
            int y = lapinouPy + 32;
            m.setTile( tr->newTile, x, y );
            if( tr->debrisTile && tr->debrisTile < tilesetFrmNb )
              addDebris( x, y, tileset[ tr->debrisTile ] ) ;
          }
        }
      }
    }
  }

  u32 dx = m.getDx( lapinouPx );
  u32 dy = m.getDy( lapinouPy );

  getTile( 0, 0, GetTile_Tile );

  if( dy ) getTile( 0, 31, GetTile_Tile );
  if( dx ){
    getTile( 31, 0, GetTile_Tile );
    if( dy ) getTile( 31, 31, GetTile_Tile );
  }

  if( power != powerDraw ){
    u8 barUpdate = powerBarSpeed.update();
    int diff = powerDraw - power;
    if( diff < 0 ){ // raise drawed power bar
      if( -diff < barUpdate )
        powerDraw = power;
      else
        powerDraw += barUpdate;
    } else { // reduce drawed power bar
      if( diff < barUpdate )
        powerDraw = power;
      else
        powerDraw -= barUpdate;
    }
  } else powerBarSpeed.reset();
}

#endif
