
#define ARDUINO_ARCH_SAMD 1
#include <SPI.h>
#include "./gb/Display-ST7735.h"
#include "FastLED.h"
#include "pmf_player.h"

#define u32 uint32_t
#define u16 uint16_t
#define u8 uint8_t

#define clDeep u16

#define frameTime 0 //40
#define bfLength (20*1024) // u8, 10k

#include "./gdl/gdl.h"
#include "./gfx/gfx16.h"

#include "lapinou.h"

#include "map.h"

#define mapSx 24
#define mapSy 64

uint8_t mapArray[ mapSx*mapSy ] = { 0 };

//SPISettings spiSettings = SPISettings(24000000, MSBFIRST, SPI_MODE0);

#define TFT_CS    (30u)
#define TFT_DC    (31u)
#define SD_CS   (26u)
#define NEOPIX_PIN  (38u)
#define BAT_PIN   (A5)
//#define BTN_CS (25)

Display_ST7735 tft = Display_ST7735( TFT_CS, TFT_DC );

#define NUM_LEDS 8
CRGB leds[NUM_LEDS];

static const uint8_t PROGMEM s_pmf_file[]=
{
  #include "music.h"
};

static pmf_player s_player;
#define effectChannels 4
#define maxEffects 4
uint8_t effectsNb = 0;

uint8_t effectChannel[effectChannels] = {};
u32 effects[ maxEffects ] = { 0 };

void addEffect( u8 sample, u8 note, u8 volume ){
  if( effectsNb == maxEffects ) return;
  u32 * e = effects;
  while( *e ) e++; // search empty
  u8 * e8 = (u8*)e;
  *e8++ = 0xff;
  *e8++ = sample;
  *e8++ = note;
  *e8 = volume;
  effectsNb++;
}

void row_callback_test(void *custom_data_, uint8_t channel_idx_, uint8_t &note_idx_, uint8_t &inst_idx_, uint8_t &volume_, uint8_t &effect_, uint8_t &effect_data_)
{ static u8 currentEffectChannelNb = 0, currentEffectChannel = effectChannel[0];
  if( effectsNb && channel_idx_ == currentEffectChannel )
  {
    u32 * e = effects;
    while( !*e ) e++; // search not empty
    if( e - effects >= maxEffects ) return;
    u8 * e8 = (u8*)e;
    inst_idx_ = e8[1];
    note_idx_ = e8[2];
    volume_ = e8[3];
    *e = 0;
    effectsNb--;
    if( ++currentEffectChannelNb == effectChannels ) currentEffectChannelNb = 0;
    currentEffectChannel = effectChannel[ currentEffectChannelNb ];
  }
}

static void fastLedSongVisualization(void *p)
{
  const pmf_player *player = (const pmf_player*)p;
  //unsigned num_channels = min(8, player->num_playback_channels());
  int c = 0, m = player->num_playback_channels();
  for(unsigned i=0; i<8; ++i)
  { if( c == m ) c=0;

    pmf_channel_info chl=player->channel_info(c);

      if( chl.note_hit ){
        int note = chl.base_note;
        if( note > 48 ){ note -= 48; note*=3; if( note > 255 ) note = 255; }
        leds[i]=CHSV( note, 255, chl.volume*4);
      } else leds[i].nscale8( 234 );
      c++;
  }
}

void setupFastLedSongVisualization(pmf_player &p){
  p.set_tick_callback( &fastLedSongVisualization, &p );
}

#define screenBufferNumber 2
#define screenBufferSize ( bfLength / screenBufferNumber )
#define screenBufferSize32 ( screenBufferSize / 4 )
#define screenLineSize ( 160*2 )

u32 tick;
u32 bf[ bfLength / 4 ] __attribute__ ((aligned (8))) = { 0xffffffff }; // u32

/*
void fillBuffer( u16 color ){
  u32 *bf32 = bf;
  *bf = (color << 16) | color;
  u32 n = bfLength / 16; // u128
  while( n-- ){
    *bf32++ = *bf;
    *bf32++ = *bf;
    *bf32++ = *bf;
    *bf32++ = *bf;
  };
}*/

/*void fillBuffer2( u32 c ){
  u32 color, y;
  u32 *bf32 = bf;
  for( y = 0; y < bfLength/(160* 4 ); y++ ){ // 32 16b lines
    u16 color16 = rgb565( c,0,c );
    u8 * c8 = (u8*)&color16;
    u8 * c32 = (u8*)&color;
    //color = (color16 << 16) | color16;

    *c32 = c32[2] = c8[1];
    c32[1] = c32[3] = *c8;

    if( c-- == 0 ) c = 31;
    u32 n = 160/8;
    while( n-- ){
      *bf32++ = color;
      *bf32++ = color;
      *bf32++ = color;
      *bf32++ = color;
    };
  }
}*/

void dmaInit(void);
void spiDma( uint32_t chnltx, void *txdata, size_t n );

outzone bfOutZone, screenOutZone;

extern void dmaStart( uint32_t channel );
extern void dmaWait( uint32_t );
extern void dmaEnd( uint32_t channel );
extern void memcpy32( uint32_t channel, void *dst, const void *src, size_t n);

extern void memcpyTest( void );
extern void dmaWait2( uint32_t channel );
extern void memset32( uint32_t channel, void *dst, uint32_t * value, size_t n);
extern void spiMemset32( uint32_t channel, uint32_t * value, size_t n );
extern void spiMemset16( uint32_t channel, uint32_t * value, size_t n );
extern void spiMemset8( uint32_t * value, size_t lines, size_t line );

Map m;

  extern volatile uint32_t dmastatus;
  extern volatile uint32_t dmacount;
  extern void dmaStart( uint32_t channel );
  //extern uint32_t getDmaState( uint32_t channel );
  extern void dmaWait( uint32_t );
  extern void dmaEnd( uint32_t channel );
  extern void dataMode(void);
  extern void idleMode(void);

uint32_t bgColor __attribute__ ((aligned (8))) = 0x00001111;//0x0ff000ff;

#define dmaWait0 while( DMAC->BUSYCH.bit.BUSYCH0 )

#define setBlitLimit(n) setOutBuffer( (clDeep*)screenBuffer[n], 160, /*screenBufferSize/screenLineSize*/ 32 )
#define sendBuffer(n) spiDma( 0, screenBuffer[n], screenBufferSize )
#define waitSendBuffer /*dmaWait2( 0 );*/dmaWait0;dmaEnd( 0 )
#define clearBuffer(n) memset32( 3, screenBuffer[n], &bgColor, screenBufferSize )
#define waitClearBuffer dmaWait2( 3 );dmaEnd( 3 )

struct gfx tilesGfx[] = {
  {tileset0,0,0,0,&udrawGfx,&drawGfx},
  {tileset1,0,0,0,&udrawGfx,&drawGfx},
  {tileset2,0,0,0,&udrawGfx,&drawGfx},
  {tileset3,0,0,0,&udrawGfx,&drawGfx},
  {tileset4,0,0,0,&udrawGfx,&drawGfx},
  {tileset5,0,0,0,&udrawGfx,&drawGfx},
  {tileset6,0,0,0,&udrawGfx,&drawGfx},
  {tileset7,0,0,0,&udrawGfx,&drawGfx},
  {tileset8,0,0,0,&udrawGfx,&drawGfx},
  {tileset9,0,0,0,&udrawGfx,&drawGfx},
  {tileset10,0,0,0,&udrawGfx,&drawGfx},
  {tileset11,0,0,0,&udrawGfx,&drawGfx},
  {tileset12,0,0,0,&udrawGfx,&drawGfx},
  {tileset13,0,0,0,&udrawGfx,&drawGfx},
  {tileset14,0,0,0,&udrawGfx,&drawGfx},
  {tileset15,0,0,0,&udrawGfx,&drawGfx},
  //{tileset15,0,-32,-32*160,&udrawGfxD,&drawGfxD},
  {tileset16,0,0,0,&udrawGfx,&drawGfx},
  {tileset17,0,0,0,&udrawGfx,&drawGfx},
  {tileset18,0,0,0,&udrawGfx,&drawGfx},
  {tileset19,0,0,0,&udrawGfx,&drawGfx},
  {tileset20,0,0,0,&udrawGfx,&drawGfx},
  {tileset21,0,0,0,&udrawGfx,&drawGfx},
  {tileset22,0,0,0,&udrawGfx,&drawGfx},
  {tileset23,0,0,0,&udrawGfx,&drawGfx},
  {tileset24,0,0,0,&udrawGfx,&drawGfx},
  {tileset25,0,0,0,&udrawGfx,&drawGfx},
  {tileset26,0,0,0,&udrawGfx,&drawGfx},
  {tileset27,0,0,0,&udrawGfx,&drawGfx}
};

void setup()
{
  WDT->CTRL.bit.ENABLE = 0;

  tft.init();
  tft.setRotation( ST7735_ROTATE_DOWN );

  FastLED.addLeds<NEOPIXEL, NEOPIX_PIN>(leds, NUM_LEDS);

  initButtons();

  s_player.load(s_pmf_file);

  for( effectsNb=0; effectsNb<effectChannels; effectsNb++ ){
    u8 n = s_player.num_playback_channels();
    effectChannel[effectsNb] = n;
    s_player.enable_playback_channels( n + 1 );
  }

  effectsNb = 0;

  s_player.set_row_callback( &row_callback_test );
  setupFastLedSongVisualization( s_player );
  s_player.start(22050);

  tick = millis();

  dmaInit();

  setOutBuffer( (clDeep*)bf, 160, bfLength/(160*2) );
  createOutzone( &bfOutZone, 0, 0, 160, 64 );
  createOutzone( &screenOutZone, 0, 0, 160, 128 );

  setAnim( &anims[0],(clDeep**)up,upFrmNb,200,0,animSwitch) ;
  setAnim( &anims[1],(clDeep**)fall,fallFrmNb,150,0,animSwitch) ;
  setAnim( &anims[2],(clDeep**)walk,walkFrmNb,120) ;
  setAnim( &anims[3],(clDeep**)stance,stanceFrmNb,200,0,animSwitch) ;
  setAnim( &anims[4],(clDeep**)salto,saltoFrmNb,110,1,animSwitch,0,0) ;

  for( u32 n=0;n<24*64;n++ ){
    arDeep t = maparray[n];
    mapArray[n] = t ? t-1 : 0;
  }

  m.set( (arDeep*)mapArray, /*(clDeep**)tileset*/(struct gfx *)tilesGfx, tilesetFrmNb, 32, 32, mapSx, mapSy, 0, 0, &bfOutZone, 0 );
  m.setOutZone( &bfOutZone ); m.maxScrolly -= 64;
  m.setScroll( 8, m.maxScrolly - 8 );

  setGdlfont( (clDeep**)font );

  lapinouInit();
}

uint32_t dmaloop = 0;
uint32_t dmaout = 0;

void printKeys(void){
  prints(2,10,"u%u d%u l%u r%u", buttons.current.up, buttons.current.down, buttons.current.left, buttons.current.right);
  prints(2,18,"a%u b%u m%u h%u", buttons.current.a, buttons.current.b, buttons.current.menu, buttons.current.home);
}

/*
u32 zgetTile(u32 x, u32 y, u32 method)
{	if(	x >= m.sizeInPixelx
	||	y >= m.sizeInPixely
	) return 0xffff;

	if( method == GetTile_Tile ){
		x >>= m.xTileDec ;
		y >>= m.yTileDec ;

		return m.array[y*m.sizeInTilex+x];
	}

	// GetTile_Size
	u32 xx = x >> m.xTileDec ;
	u32 yy = y >> m.yTileDec ;
	u32 tile = m.array[yy*m.sizeInTilex+xx];
	if( !tile ) return 0;

	u32 dy = y - ( yy << m.yTileDec ); // y pos in tile

	u8 *g = (u8*)( m.gfx[ tile ].gfx ),
	*up = g+2,
	*down = g+3
	;

	return ( dy < *down || dy > *up ) ? 0 : tile;
}
*/

void loop(){
  s_player.update();
  FastLED.show();
  readButtons();

  if( !buttons.current.home && buttons.last.home ) ((void(*)(void))(*((uint32_t*)0x3FF4)))();
  if( !buttons.current.b && buttons.last.b ) addEffect(2,5*12,63);
  if( !buttons.current.a && buttons.last.a ) addEffect(3,5*12,63);
  if( !buttons.current.menu && buttons.last.menu ) addEffect(4,5*12,63);
  if( !buttons.current.up && buttons.last.up ) addEffect(4,5*12,63);
  if( !buttons.current.down && buttons.last.down ) addEffect(3,4*12,63);
  if( !buttons.current.left && buttons.last.left ) addEffect(1,5*12,63);
  if( !buttons.current.right && buttons.last.right ) addEffect(3,2*12,63);

  tick = millis();

  static u32 c = 31;
  static u32 timenext = tick + frameTime;

  if( tick > timenext ){
    if( c++ == 31 ) c = 0;

    if( m.scrolly >= 64 ) m.scrolly -= 64; else m.scrolly = 0;
    m.scroll(0,0); // ??

    m.setOutZone( &screenOutZone );
    lapinouUpdate();
    m.follow(
     lapinouPx, lapinouPy, 32, 32,
     &mapScrollRate,
      32, 30, 55, 55,
      16, 10, 24, 24
    );

    m.setOutZone( &bfOutZone ); m.maxScrolly -= 64;

    uint32_t chn = 0;
    uint32_t l = 160*64*2;

    memset32(3, bf, &bgColor, 20*1024 );
    dmaWait2( 3 );
    dmaEnd( 3 );

    m.scroll(0,0);
    // happy!
    // assassination and classroom
    drawDebris();
    m.draw();

    prints( 2,2, "%u", countFps() );
    playAnim( &lapinou, lapinouPx - m.scrollx, lapinouPy - m.scrolly, lapinouWay );//(m.scrolly + 64), /*lapinouWay*/ 0 ) ;

    tft.fullScreenSpi();
    spiDma( chn, bf, l );
    dmaWait( chn );
    dmaEnd( chn );

    memset32(3, bf, &bgColor, 20*1024 );
    dmaWait2( 3 );
    dmaEnd( 3 );

    m.scrolly += 64; m.scroll(0,0);
    drawDebris();
    m.draw();

    playAnim( &lapinou, lapinouPx - m.scrollx, lapinouPy - (m.scrolly), lapinouWay) ;

    clDeep * pscr = &pixel[ (122) + 160*48 ], *ptrs = pscr + 1;
    u32 n = 10;
    while( n-- ){
      ptrs += 160;
      memset( ptrs, 0xff, powerDraw*2 );
    };
    udrawGfm( (clDeep*)powerbar0, pscr );

    spiDma( chn, bf, l );
    dmaWait( chn );
    dmaEnd( chn );

    tft.idleMode();
    SPI.endTransaction();

    timenext = tick + frameTime;
  }

}
