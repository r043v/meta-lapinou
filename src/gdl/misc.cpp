
#include "./gdl.h"

u16 pc2gpColor(u32 color)
{      register u32 r,g,b;
       r = (color>>19)&0x1f;
       g = (color>>11)&0x1f;
       b = (color>> 3)&0x1f;
       return (r<<11)|(g<<6)|(b<<1)|1;
}

// count frame per second .. only launch this routine one time per frame !
u32 countFps(void){
  //static u32 tm=0, fps=0, fps_count=0 ;
  	//fps++ ;  if(tm + 500 < tick) { fps_count=fps<<1; fps=0; tm=tick; }
    //fps++ ;  if(tm + 1000 < tick) { fps_count=fps<<1; fps=0; tm=tick; }
    //return fps_count ;
    static u32 next = tick + 1000, fps = 0, fpsCount = 0;
    fps++;
    if( tick >= next ){
      fpsCount = fps; fps = 0; next = tick+1000;
    }
    return fpsCount;
}

u32 computeDec(u32 value)
{	u32 v=1, dec=0;
	while(v<value) { v<<=1; dec++; }
	return dec ;
}

u32 divide(u32 divadeWhat, u32 byHowMany)
{	if(!byHowMany) return 0;
	u32 nb=0;
	while(divadeWhat>=byHowMany)
		{	divadeWhat-=byHowMany;
			nb++;
		};
	return nb;
}
