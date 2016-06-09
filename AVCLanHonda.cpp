//--------------------------------------------------------------------------------
#include "limits.h"
#include "AVCLanDrv.h"
#include "AVCLanHonda.h"

#include <avr/pgmspace.h>
//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
//TODO: master -> 131
// < d 183 131 06170D31020158
// +++
// < d 183 131 08590D31020122712F
// Rear camm off

// 09780D3103024B000009 - next track ?
// 08590D3102012212D0
// 08590D3102012213D1
// 08590D3102012214D2
// 0618023102004D - mute button press
// < d 183 100 0B600D01000140165400000B - time 16:54
// < d 100 183 0D60073100010003011018FF5008 - FM1 101.8
// 0D60073100010000021014FF5002 - FM2 101.4

// { action_name,  packed_size, end_off_packege_word(check_sum)  }
const AvcInCmdTable  mtSearchHead[] PROGMEM = {
  { ACT_PREP_CAMOFF,       0x06,   0x58},      //06170D31020158
  { ACT_BUTTON_DOWN,       0x08,   0xBC},	   // 08590D3102012000BC
  { ACT_BUTTON_UP,         0x08,   0xBE},     // 08590D3102012101BE
  { ACT_B_DISPOFF,         0x08,   0x2F},     // 08590D31020122712F
  { ACT_B_DISPFULL_DOWN,   0x08,   0x43},     // 08590D310201228543
  { ACT_B_DISPFULL_UP,     0x08,   0xBD},     // 08590D3102012001BD
  { ACT_B_DISPHULF,        0x08,   0xCF},     // 08590D3102012211CF
  { ACT_CAM_ON,            0x09,   0x31},     // 09590D31020194000031
  { ACT_CAM_OFF,           0x09,   0x30},     // 09590D31020191020030
  { ACT_DISP_OFF,          0x09,   0x2E},     // 09590D3102019100002E
  { ACT_TEL,               0x0A,   0xE3},	  // start Tel Action
  { ACT_TEL_CANCEL,        0x0A,   0xDC}	  // Cancel or End Call_ACTION
  //        { ACT_VOL,            0x0A,    0x06, {0x68, 0x02, 0x31, 0x02, 0x02, 0x00}, 0x00, {0x00}}  //0x05 = 1
};

const byte mtSearchHeadSize = sizeof(mtSearchHead) / sizeof(AvcInCmdTable);

//--------------------------------------------------------------------------------
void AVCLanHonda::begin()
//--------------------------------------------------------------------------------
{
  avclan.deviceAddress = 0x0131;

  bPrepareCamOff = false;
  bShowHondaDisp = true;
  setHondaDisLast(true);
  bShowRearCam =  false;
  bFirstStart_20 = true;
  bFreeze = false;

  setWaitTime( 0L );
  freezeTime = 0L;

  // timer1 setup, prescaler factor - 1024
  TCCR1A = 0;       // normal mode
  TCCR1B = 5;       // Prescaler /1024
  TCNT1H = TI1_H;   // Load counter value hi
  TCNT1L = TI1_L;   // Load counter value lo
  DISABLE_TIMER1_INT;
}

//--------------------------------------------------------------------------------
void AVCLanHonda::setWaitTime( const unsigned long mTime )
//--------------------------------------------------------------------------------
{
  waitTime = mTime;
  bWait = (bool) waitTime;
}

// Use the last received message to determine the corresponding action ID,
// store it in avclan object
//--------------------------------------------------------------------------------
void AVCLanHonda::getActionID()
//--------------------------------------------------------------------------------
{
  avclan.actionID = avclan.getActionID( mtSearchHead, mtSearchHeadSize );
};

// process action
//--------------------------------------------------------------------------------
void AVCLanHonda::processAction( AvcActionID ActionID )
//--------------------------------------------------------------------------------
{
  if ( bFirstStart_20 ) {
    if ( (INIT2_TIME > millis()) && (ACT_CAM_ON == ActionID) ) {
      bShowRearCam = true;
      setHondaDisLast(false);
      bShowHondaDisp = true;
      bFirstStart_20 = false;
      setWaitTime(0L);
      return;
    }
    if ( INIT2_TIME < millis() ) bFirstStart_20 = false;
  }

  if ( bPrepareCamOff && (ACT_B_DISPOFF == ActionID) ) {
    ActionID = ACT_CAM_OFF;
  } else bPrepareCamOff = false;

  switch ( ActionID ) {
    case ACT_BUTTON_UP:
      {
        if ( !bShowRearCam || !isWait() )
        {
          if ( ULONG_MAX > (millis() + BUTT_WAIT) )
            setWaitTime( (unsigned long)((millis() + BUTT_WAIT)) );
          else setWaitTime( BUTT_WAIT );
        }
      }
      break;
    case ACT_CAM_ON:
      bShowRearCam = true;
      setHondaDisLast( isShowHondaDisp() );
      bShowHondaDisp = true;
      setWaitTime(0L);
      break;
    case ACT_B_DISPOFF:
    case ACT_B_DISPFULL_UP:
    case ACT_B_DISPHULF:
      if ( !bShowRearCam ) {
        // need freeze on 2000 sec after code receiving.
        bShowHondaDisp = false;
        setHondaDisLast( false );
        setWaitTime(0L);
        bFreeze = true;
        freezeTime = (millis() + FREEZE_TIME);
      }
      break;
    case ACT_CAM_OFF:
      bShowRearCam = false;
      bShowHondaDisp = bHondaDisLast;
      setWaitTime(0L);
      break;
    case ACT_PREP_CAMOFF:
      bPrepareCamOff = true;
      break;
  }
};

//--------------------------------------------------------------------------------
void AVCLanHonda::checkWait()
//--------------------------------------------------------------------------------
{
  if ( isWait() && (getWaitTime() < millis()) ) {
    setWaitTime( 0L );
  }
}

//--------------------------------------------------------------------------------
bool AVCLanHonda::getCommute()
//--------------------------------------------------------------------------------
{
  return (bShowHondaDisp || bShowRearCam);
}

//--------------------------------------------------------------------------------
void AVCLanHonda::tryToShowHondaDisp()
//--------------------------------------------------------------------------------
{
  setHondaDisLast( bShowHondaDisp );
  bShowHondaDisp = true;
}

//--------------------------------------------------------------------------------
void AVCLanHonda::setHondaDis( bool val )
//--------------------------------------------------------------------------------
{
  bShowHondaDisp = val;
  setHondaDisLast( val );
  setWaitTime(0L);
}


AVCLanHonda avclanHonda;


