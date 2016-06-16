#define IEBUS_VERSION "0.82"

// type of AVCLan hardwire driver
#define AVCLAN_PCA82C250

// maximum number of attempts to send a message
#define MAXSENDATTEMP   3

//********** arduino & driver based on PCA82C250 **********
// http://pighixxx.com/nanopdf.pdf
// http://pighixxx.com/leonardopdf.pdf

#define AVCLANDRIVER_TYPE    "Arduino - PCA82C250"
// define out pin (digital #9 arduino) for AVCLAN_PCA82C250
#define DATAOUT_DDR	DDRB
#define DATAOUT_PORT	PORTB
#define	DATAOUT_PIN	PINB
#define DATAOUT		5

// define in pin (digital #8 arduino) for AVCLAN_PCA82C250
#define DATAIN_DDR	DDRB
#define DATAIN_PORT	PORTB
#define	DATAIN_PIN	PINB
#define DATAIN		4

// connected to digital pin A0
#define  COMMUT_DDR     DDRF
#define  COMMUT_PORT    PORTF
#define  COMMUT_PIN     PINF
#define  COMMUT_OUT     7

#define INIT_TIME 13000
#define INIT2_TIME 20000

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) //0
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))  //1
#endif

