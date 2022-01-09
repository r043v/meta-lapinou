
#include <Arduino.h>

typedef struct {
	uint16_t btctrl;
	uint16_t btcnt;
	uint32_t srcaddr;
	uint32_t dstaddr;
	uint32_t descaddr;
} dmacdescriptor ;

#define dmaMaxChannels 4

volatile dmacdescriptor wrb[dmaMaxChannels] __attribute__ ((aligned (16)));
dmacdescriptor descriptor_section[dmaMaxChannels] __attribute__ ((aligned (16)));
dmacdescriptor descriptor __attribute__ ((aligned (16)));
//volatile uint32_t dmadone;//[12];
//volatile uint8_t dmaState[12] = { 0 };
//volatile uint32_t dmastatus = 0xff;

#define dmaSelectChannel( channel ) DMAC->CHID.reg = DMAC_CHID_ID( channel )
#define dmaEnable DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE
#define dmaDisable DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE
#define dmaEnableChannel( channel ) dmaSelectChannel( channel );dmaEnable
#define dmaDisableChannel( channel ) dmaSelectChannel( channel );dmaDisable
#define dmaReset DMAC->CHCTRLA.reg = DMAC_CHCTRLA_SWRST
//#define dmaReset {}

volatile uint32_t dmadone;
//volatile uint32_t dmacount;
//volatile uint32_t dmablocklength;

#define memsetChannel 11

void DMAC_Handler() {
	__disable_irq();
	uint32_t channel = DMAC->INTPEND.reg & DMAC_INTPEND_ID_Msk; // get channel number
	uint8_t isr = DMAC->CHINTFLAG.reg;

	if(isr & DMAC_CHINTENCLR_TCMPL){ // done
		DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TCMPL;
	} else if(isr & DMAC_CHINTENCLR_SUSP){
		DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_SUSP;
	} else if(isr & DMAC_CHINTENCLR_TERR){
		DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TERR;
	}

/*
if( channel == memsetChannel ){
	if( ++dmacount == dmablocklength ){
		dmaSelectChannel( memsetChannel );
		dmaReset;
		//dmadone = 1;
	}
}
*/

/*
	if( channel == memsetChannel ){
		if( ++dmacount == dmablocklength ){
			dmaDisableChannel( memsetChannel );
			dmadone = 1;
		}
	}
	 else
*/
	//if( !channel )
	dmadone = 1;

	//dmastatus = DMAC->INTPEND.reg;
//	dmaState[ channel ] = 0;

//dmastatus = DMAC->INTSTATUS.reg;
/*
	for( uint32_t channel = 0; channel<12; channel++ ){
		if (DMAC->INTSTATUS.reg & (1<<channel)){
			//dmaSelectChannel( channel );
			dmaState[ channel ] = 0;
			//DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TCMPL |  DMAC_CHINTENCLR_TERR |  DMAC_CHINTENCLR_SUSP;
		}
	}
*/
/* */

	// clear interrupts

	//if( done ){
//		DMAC->CHID.reg = DMAC_CHID_ID( channel );
//		DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
		//if( done )
		//dmaState[ channel ] = 0 ; // mark channel as free
	//}
/*
	dmadone = 1;//DMAC->CHINTFLAG.reg;
	dmaState[ channel ] = 0;

	DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TCMPL |  DMAC_CHINTENCLR_TERR |  DMAC_CHINTENCLR_SUSP;
	*/
/*
	DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TCMPL; // clear
	DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TERR;
	DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_SUSP;
*/
//	dmaDisableChannel( channel );

	//dmadone = DMAC->CHINTFLAG.reg;

//	DMAC->CHINTFLAG.reg = DMAC_CHINTENCLR_TCMPL;

	__enable_irq();
}

void dmaInit() {
//	DMAC->CTRL.reg = DMAC_CTRL_SWRST;

	PM->AHBMASK.reg |= PM_AHBMASK_DMAC ;
	PM->APBBMASK.reg |= PM_APBBMASK_DMAC ;
	NVIC_EnableIRQ( DMAC_IRQn ) ;

	DMAC->BASEADDR.reg = (uint32_t)descriptor_section;
	DMAC->WRBADDR.reg = (uint32_t)wrb;
	DMAC->CTRL.reg = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);
}
/*
#define dmaSelectChannel( channel ) DMAC->CHID.reg = DMAC_CHID_ID( channel )
#define dmaEnable DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE
#define dmaDisable DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE
#define dmaEnableChannel( channel ) dmaSelectChannel( channel );dmaEnable
#define dmaDisableChannel( channel ) dmaSelectChannel( channel );dmaDisable
#define dmaReset DMAC->CHCTRLA.reg = DMAC_CHCTRLA_SWRST
*/
void memcpy32( uint32_t channel, void *dst, const void *src, size_t n) {
	dmaDisableChannel( channel );
	dmaReset;
	//DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << channel)); // enable done interrupt
	//DMAC->SWTRIGCTRL.reg |= (1 << channel);  // trigger channel
	DMAC->CHINTENSET.reg = DMAC_CHINTENSET_MASK;//DMAC_CHINTENSET_TCMPL ; // enable complete interrupt
	dmadone = 0;

	descriptor.descaddr = 0;
	descriptor.dstaddr = (uint32_t)dst + n;
	descriptor.srcaddr = (uint32_t)src + n;
	descriptor.btcnt =  n/4;
	descriptor.btctrl =  DMAC_BTCTRL_BEATSIZE_WORD | DMAC_BTCTRL_DSTINC | DMAC_BTCTRL_SRCINC | DMAC_BTCTRL_VALID;
	memcpy( &descriptor_section[channel], &descriptor, sizeof(dmacdescriptor) );
	//dmaEnable;
/*	dmaEnableChannel( channel );
	while(!dmadone);  // await DMA done isr
	dmaDisableChannel( channel );*/
//	dmaState[channel] = 1;

		DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
	  DMAC->SWTRIGCTRL.reg |= (1 << channel);  // trigger channel 0
}

void memset32( uint32_t channel, void *dst, uint32_t * value, size_t n) {
	dmaDisableChannel( channel );
	dmaReset;

//	DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << channel)); // enable done interrupt
	//DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL(0) | DMAC_CHCTRLB_TRIGSRC(0) | DMAC_CHCTRLB_TRIGACT_TRANSACTION;
	DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL ; // enable complete interrupt
	dmadone = 0;

//	dmaState[channel] = 1;

	descriptor.descaddr = 0;
	descriptor.dstaddr = (uint32_t)dst + n;
	descriptor.srcaddr = (uint32_t)value;
	descriptor.btcnt =  n / 4;
	descriptor.btctrl =  DMAC_BTCTRL_BEATSIZE_WORD | DMAC_BTCTRL_DSTINC | DMAC_BTCTRL_VALID;
	memcpy( &descriptor_section[channel], &descriptor, sizeof(dmacdescriptor) );

	dmaEnable;
	DMAC->SWTRIGCTRL.reg |= (1 << channel);  // trigger channel
}
/*
void spiMemset32( uint32_t channel, uint32_t * value, size_t n ){
	dmaDisableChannel( channel );
	dmaReset;
	//DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << chnltx)); // enable done interrupt
	DMAC->SWTRIGCTRL.reg |= (1 << channel);  // trigger channel
	DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL(0) | DMAC_CHCTRLB_TRIGSRC(SERCOM4_DMAC_ID_TX) | DMAC_CHCTRLB_TRIGACT_BEAT;
	DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;

	dmadone = 0;

	descriptor.descaddr = 0;
	descriptor.dstaddr = (uint32_t)&SERCOM4->SPI.DATA.reg;
	descriptor.btcnt =  n / 4;
	descriptor.srcaddr = (uint32_t)value;
	descriptor.btctrl =  DMAC_BTCTRL_BEATSIZE_WORD | DMAC_BTCTRL_VALID;
	memcpy( &descriptor_section[channel], &descriptor, sizeof(dmacdescriptor) );
	dmaEnable;
}

void spiMemset16( uint32_t channel, uint32_t * value, size_t n ){
	dmaDisableChannel( channel );
	dmaReset;
	//DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << chnltx)); // enable done interrupt
	DMAC->SWTRIGCTRL.reg |= (1 << channel);  // trigger channel
	DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL(0) | DMAC_CHCTRLB_TRIGSRC(SERCOM4_DMAC_ID_TX) | DMAC_CHCTRLB_TRIGACT_BEAT;
	DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;

	dmadone = 0;

	descriptor.descaddr = 0;
	descriptor.dstaddr = (uint32_t)&SERCOM4->SPI.DATA.reg;
	descriptor.btcnt =  n / 2;
	descriptor.srcaddr = (uint32_t)value;
	descriptor.btctrl =  DMAC_BTCTRL_BEATSIZE_HWORD | DMAC_BTCTRL_VALID;
	memcpy( &descriptor_section[channel], &descriptor, sizeof(dmacdescriptor) );
	dmaEnable;
}

void spiMemset8( uint32_t * value, size_t lines, size_t line ){
	dmaDisableChannel( memsetChannel );
	dmaReset;
	//DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << chnltx)); // enable done interrupt
	DMAC->SWTRIGCTRL.reg |= (1 << memsetChannel);  // trigger channel
	DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL(0) | DMAC_CHCTRLB_TRIGSRC(SERCOM4_DMAC_ID_TX) | DMAC_CHCTRLB_TRIGACT_BEAT;
	DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;

	dmadone = 0;
	dmablocklength = lines;
	dmacount = 0;

	descriptor.descaddr = (uint32_t)&descriptor_section[ memsetChannel ];
	descriptor.dstaddr = (uint32_t)&SERCOM4->SPI.DATA.reg;
	descriptor.btcnt = line;
	descriptor.srcaddr = (uint32_t)value + line;
	descriptor.btctrl =  DMAC_BTCTRL_BEATSIZE_BYTE | DMAC_BTCTRL_SRCINC | DMAC_BTCTRL_VALID;

	memcpy( &descriptor_section[ memsetChannel ], &descriptor, sizeof(dmacdescriptor) );
	dmaEnable;
}

void memsetLoop( void *dst, uint32_t * value, size_t lines, size_t line ) {
	dmaDisableChannel( memsetChannel );
	dmaReset;
	//DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << channel)); // enable done interrupt
	DMAC->SWTRIGCTRL.reg |= (1 << memsetChannel);  // trigger channel
	DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL ; // enable complete interrupt
	dmadone = 0;
	descriptor.descaddr = (uint32_t)&descriptor_section[ memsetChannel ];
	descriptor.dstaddr = (uint32_t)dst + line;
	descriptor.srcaddr = (uint32_t)value;
	descriptor.btcnt =  line;
	descriptor.btctrl =  DMAC_BTCTRL_BEATSIZE_BYTE | DMAC_BTCTRL_SRCINC | DMAC_BTCTRL_VALID;
	memcpy( &descriptor_section[memsetChannel], &descriptor, sizeof(dmacdescriptor) );
	dmaEnable;
}
*/
void spiDma( uint32_t chnltx, void *txdata, size_t n ){
	dmaDisableChannel( chnltx );
	dmaReset;

	DMAC->CHCTRLB.reg = DMAC_CHCTRLB_LVL(0) | DMAC_CHCTRLB_TRIGSRC(SERCOM4_DMAC_ID_TX) | DMAC_CHCTRLB_TRIGACT_BEAT;
	DMAC->SWTRIGCTRL.reg &= (uint32_t)(~(1 << chnltx)); // enable done interrupt
	//DMAC->SWTRIGCTRL.reg |= (1 << chnltx);  // trigger channel
	DMAC->CHINTENSET.reg = DMAC_CHINTENSET_TCMPL;//DMAC_CHINTENSET_MASK;

//	dmaState[ chnltx ] = 1;
	dmadone = 0;

	descriptor.descaddr = 0;
	descriptor.dstaddr = (uint32_t)&SERCOM4->SPI.DATA.reg;
	descriptor.btcnt =  n;
	descriptor.srcaddr = (uint32_t)txdata + n;
	descriptor.btctrl =  DMAC_BTCTRL_VALID | DMAC_BTCTRL_SRCINC;

	memcpy( &descriptor_section[chnltx], &descriptor, sizeof(dmacdescriptor) );
	dmaEnable;
//	DMAC->SWTRIGCTRL.reg |= (1 << chnltx);
}

void dmaStart( uint32_t channel ){
		/*uint32_t chn = DMAC_CHID_ID( channel );
		DMAC->CHID.reg = chn;//DMAC_CHID_ID( channel );
		DMAC->CHCTRLA.reg |= DMAC_CHCTRLA_ENABLE;
		dmaState[ chn ] = 1;
		*/
		dmaEnableChannel( channel );
}

void dmaWait2( uint32_t channel ){
	//while (wrb[channel].btctrl & DMAC_BTCTRL_VALID);
	while(
//DMAC->BUSYCH.bit.BUSYCH3 & ( 1 << channel )
			(wrb[channel].btctrl & DMAC_BTCTRL_VALID)
//			&& dmaState[ channel ]
//			&& !dmadone
//!dmadone
	);
	//while( !dmadone );
}

void dmaWait( uint32_t channel ){
	while( !dmadone );
	//while( dmaState[ channel ] );
//	dmaDisableChannel( channel );
}

void dmaEnd( uint32_t channel ){
	dmaDisableChannel( channel );
}

extern uint32_t *bf;

void memcpyTest( void ){
	memcpy32( 0, bf, &bf[(10*1024)/4], 10*1024);
	dmaStart(0);
	dmaWait(0);
	//memcpy( bf, &bf[(10*1024)/4], 10*1024 );
}

/*uint32_t getDmaState( uint32_t channel ){
	return dmaState[ channel ];//wrb[ channel ].descaddr == 0;
}*/

/*uint32_t dmaProgress( uint32_t chn ){
	return wrb[chn].btcnt;
}*/
/*
void dmaEnd( uint32_t chnltx ){
	DMAC->CHID.reg = DMAC_CHID_ID(chnltx);   //disable DMA to allow lib SPI
	DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
}
*/
/*uint32_t spiDmaDone( uint32_t chn ){
	register uint32_t done = dmadone[chn];
	if( done ){
		//dmaEnd(chn);
		DMAC->CHID.reg = DMAC_CHID_ID( chn );
		DMAC->CHCTRLA.reg &= ~DMAC_CHCTRLA_ENABLE;
	}
	return done;
}*/
