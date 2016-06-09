#define IEBUS_VERSION "0.82"

// type of AVCLan hardwire driver
#define AVCLAN_PCA82C250

// maximum number of attempts to send a message
#define MAXSENDATTEMP   3

//********** arduino & driver based on PCA82C250 **********
// http://pighixxx.com/nanopdf.pdf
#ifdef AVCLAN_PCA82C250
#define AVCLANDRIVER_TYPE    "Arduino - PCA82C250"
// define out pin (pin 8 arduino) for AVCLAN_PCA82C250
#define DATAOUT_DDR	DDRB
#define DATAOUT_PORT	PORTB
#define	DATAOUT_PIN	PINB
#define DATAOUT		0

// define in pin (pin 9 arduino) for AVCLAN_PCA82C250
#define DATAIN_DDR	DDRB
#define DATAIN_PORT	PORTB
#define	DATAIN_PIN	PINB
#define DATAIN		1

// connected to digital pin A0
#define  COMMUT_DDR     DDRC
#define  COMMUT_PORT    PORTC
#define  COMMUT_PIN     PINC
#define  COMMUT_OUT     0

// LED connected to digital pin 13
#define LED_DDR		DDRB
#define LED_PORT	PORTB
#define LED_PIN		PINB
#define LED_OUT		5

#define INIT_TIME 13000
#define INIT2_TIME 20000

#endif





