
#include "./gdl.h"

Rate::Rate( u16 px ){
  pxBySecond( px );
  reset();
}

Rate::Rate( void ){
  reset();
}

void Rate::reset( void ){
  last = tick;
  acc = 0;
}

/*void Rate::start( void ){
  this->acc = this->ticks;
}*/

void Rate::ticksByPx( u32 ticks ){
  this->ticks = ticks * 1024;
  this->px = 1024000 / this->ticks;
}

void Rate::pxBySecond( u16 px ){
  this->ticks = 1024000 / px;
  this->px = px;
}

u32 Rate::update(void){
  if( tick == last ) return 0;

  acc += ( tick - last ) * 1024;
  last = tick;

  if( acc < ticks ) return 0;

	u32 px = 0;
  do {
    px++;
    acc -= ticks;
  }	while( acc >= ticks );

  return px;
}

/*
u32 Rate::zupdate(void){
  if( tick == last ) return 0;

  acc += tick - last;
  last = tick;

  if( acc < ticks ) return 0;

	u32 px = 0;
  do {
    px++;
    acc -= ticks;
  }	while( acc >= ticks );

  return px;
}
*/
