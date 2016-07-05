#include "Keyboard.h"
#include "Mouse.h"

#include "AVCLanDrv.h"
#include "AVCLanHonda.h"
#include "config.h"

#define VERSIO_JOG "0.77"

//---------------------------------------------------------------------------
static const int pkgLong = 4;
static const int pkgSize = 21;
static int valInc = 0;


//#define HONDA_DIS_ON   sbi(COMMUT_PORT, COMMUT_OUT);
//#define HONDA_DIS_OFF  cbi(COMMUT_PORT, COMMUT_OUT);
#define HONDA_DIS_ON      analogWrite( A0, 255 ); analogWrite( A1, 0 );
#define HONDA_DIS_OFF     analogWrite( A0, 0 ); analogWrite( A1, 255 );
//---------------------------------------------------------------------------

const int pkgData[pkgSize][pkgLong] = {
  { 255,   0,   0,   0 }, // 0 Scroll LEFT
  { 129,   0,   0, 126 }, // 1 Scroll RIGHT
  { 128,  64,   0,  63 }, // 2 Push
  { 128,   0,   4, 123 }, // 3 to Right
  { 128,   0,   1, 126 }, // 4 to Left
  { 128,   0,   8, 119 }, // 5 to Up
  { 128,   0,   2, 125 }, // 6 to Donw
  { 128,   0,  12, 115 }, // 7 to Right + Up
  { 128,   0,   6, 121 }, // 8 to Right + Down
  { 128,   0,   9, 118 }, // 9 to Left + Up
  { 128,   0,   3, 124 }, // 10 to Left + Down
  { 128,   0,   0, 127 }, // 11 after JOG mouse,
  { 128,   1,   0, 126 }, // 12 Map/guide
  { 128,   0,  64,  63 }, // 13 Audio
  { 128,   0,  32,  95 }, // 14 Cancel
  { 128,  32,   0,  95 }, // 15 Menu
  { 128,   4,   0, 123 }, // 16 A/C
  { 128,  16,   0, 111 }, // 17 Info
  { 128,   8,   0, 119 }, // 18 Setup
  { 128,   2,   0, 125 }, // 19 Light
  {  -1,  -1,   0,   0 }  // 20 Long Push
};

//---------------------------------------------------------------------------
int pkgVal[pkgLong];
const int waitTime = 150; //300 mSec
const int maxWaitNextAction = 600;
const int scrollNextAction = 300;
const int mouseRange = 5;
const int incRangeCount = 3;
const int longpushtime =  1200;

int lastAction;
int actionIteration;
int moveStep;

unsigned long wTime;
unsigned long wIsPush;
unsigned long wIsLongPush;

bool bKybAction;
bool isMainDisplay;
//---------------------------------------------------------------------------

enum eActions {
  SCROLL_LEFT = 0,
  SCROLL_RIGHT,
  PUSH,
  TO_RIGHT,
  TO_LEFT,
  TO_UP,
  TO_DOWN,
  TO_RIGHT_UP,
  TO_RIGHT_DOWN,
  TO_LEFT_UP,
  TO_LEFT_DOWN, //10
  AFTER_MOUSE_DOWN,
  B_MAP,
  B_AUDIO,
  B_CANCEL,
  B_MENU,
  B_AC,
  B_INFO,
  B_SETUP,
  B_LIGHT,
  LONG_PUSH
};

//---------------------------------------------------------------------------
void setup()
//---------------------------------------------------------------------------
{
  pinMode( A0, OUTPUT ); // Video signal
  pinMode( A1, OUTPUT ); // JOG signal

  HONDA_DIS_ON;

  Serial.begin(250000); //LOG
  Serial1.begin(4800);  //JOG

  avclan.begin();
  avclanHonda.begin();
  avclan.deviceAddress = 0x0131;


  delay(100);
  Serial.print("Start jog version:");
  Serial.println( VERSIO_JOG );

  lastAction = -1;
  actionIteration = 0;
  moveStep = mouseRange;
  wTime = 0;
  // initialize mouse control:
  Mouse.begin();
  Keyboard.begin();

  bKybAction = true;

  delay(13000); //sleep for showing original HONDA display
  HONDA_DIS_OFF;
  isMainDisplay = false;
}

//---------------------------------------------------------------------------
int getAction()
//---------------------------------------------------------------------------
{
  for ( int a = 0; a < pkgSize; a++ ) {
    if ( pkgData[a][0] == pkgVal[0] && pkgData[a][1] == pkgVal[1]
         && pkgData[a][2] == pkgVal[2] && pkgData[a][3] == pkgVal[3] )
    {
      //      Serial.print("pkgVal = "); Serial.print(pkgVal[0]);
      //      Serial.print(" "); Serial.println(pkgVal[1]);
      switch ( a ) {
        case SCROLL_LEFT :
          Serial.println("SL!");
          return SCROLL_LEFT;
        case SCROLL_RIGHT :
          Serial.println("SR!");
          return SCROLL_RIGHT;
        case PUSH :
          return PUSH;
        case TO_RIGHT :
          Serial.println("JR!");
          return TO_RIGHT;
        case TO_LEFT :
          Serial.println("JL!");
          return TO_LEFT;
        case TO_UP :
          Serial.println("JU!");
          return TO_UP;
        case TO_DOWN :
          Serial.println("JD!");
          return TO_DOWN;
        case TO_RIGHT_UP :
          return TO_RIGHT_UP;
        case TO_RIGHT_DOWN :
          return TO_RIGHT_DOWN;
        case TO_LEFT_UP :
          return TO_LEFT_UP;
        case TO_LEFT_DOWN :
          return TO_LEFT_DOWN;
        case AFTER_MOUSE_DOWN :
          return AFTER_MOUSE_DOWN;
        case  B_MAP:
          Serial.println("MAP!");
          return B_MAP;
        case B_AUDIO :
          Serial.println("AUDIO!");
          return B_AUDIO;
        case B_CANCEL :
          Serial.println("CANCEL!");
          return B_CANCEL;
        case B_MENU :
          Serial.println("MENU!");
          return B_MENU;
        case  B_AC :
          Serial.println("AC!");
          return B_AC;
        case B_INFO :
          Serial.println("INFO!");
          return B_INFO;
        case B_SETUP :
          Serial.println("SETUP!");
          return B_SETUP;
        case B_LIGHT :
          Serial.println("LIGHT!");
          return B_LIGHT;
        default:
          return -1;
      }
    }
  }
}

//---------------------------------------------------------------------------
void loop()
//---------------------------------------------------------------------------
{
  //----------------------- AVC LAN ------------------------------------------
  if ( INPUT_IS_SET ) {
    byte res = avclan.readMessage();
    if ( !res )
    {
      switch (avclanHonda.getActionID())
      {
        case ACT_CAM_ON:
          HONDA_DIS_ON;
          break;
        case ACT_CAM_OFF:
          if ( !isMainDisplay ) HONDA_DIS_OFF;
          break;
        default:
          break;
      }
    }
  }

  //----------------------- JOG ----------------------------------------------
  if ( Serial1.available() ) {
    pkgVal[valInc++] = Serial1.read();
    // Serial.println(pkgVal[valInc - 1]);

    if (  valInc == 1 && pkgVal[0] != 128 && pkgVal[0] != 129 && pkgVal[0] != 255  ) {
      valInc = 0;
      Serial.println("JE1");
      return;
    } else if ( valInc == 2 && pkgVal[1] > 3 && pkgVal[1] != 64
                && pkgVal[1] != 32 && pkgVal[1] != 4 && pkgVal[1] != 16 && pkgVal[1] != 8 ) {
      valInc = 0;
      Serial.println("JE2");
      return;
    }
  }

  if ( (0 != wIsPush) && (wIsPush < millis()) ) {
    // PUSH
    wIsPush = 0;
    wIsLongPush = 0;
    if ( bKybAction) Keyboard.press( KEY_RETURN );
    else Mouse.click();

    delay(100);

    lastAction = 0;
    Keyboard.releaseAll();
    Serial.println("P!");
  } else if ( ( 0 != wIsLongPush) && ( wIsLongPush < millis()) ) {
    // Long PUSH
    wIsPush = 0;
    wIsLongPush = 0;
    Serial.println("LP!");

    isMainDisplay = !isMainDisplay;

    if ( isMainDisplay ) {
      HONDA_DIS_ON;
    } else {
      HONDA_DIS_OFF;
    }

    lastAction = LONG_PUSH;
    delay(100);
    Keyboard.releaseAll();
    wTime = (millis() + maxWaitNextAction );
  }

  if ( valInc == pkgLong ) {
    int iAction = getAction();
    if ( -1 != iAction ) valInc = 0;

    // Serial.print("LA:"); Serial.println(lastAction);
    if ( (lastAction == iAction) && wTime && (wTime > millis()) ) {
      valInc = 0;
      //      Serial.println("wTime");
      return;
    }
    wTime = 0;

    if ( PUSH == iAction ) {
      if ( lastAction == PUSH ) {
        wIsPush = millis() + maxWaitNextAction;
      }
      else {
        // Serial.println("StartPushAct");
        wIsPush = millis() + maxWaitNextAction;
        wIsLongPush = millis() + longpushtime;
        lastAction = PUSH;
      }
      return;
    }
    else if ( ( -1 != iAction ) && ( B_LIGHT >= iAction ) ) {
      //      Serial.println(" "); Serial.print("Action:"); Serial.println(iAction);
      //----------------------- ACTION EVENTS -------------------------------------
      valInc = 0;
      if ( iAction == SCROLL_LEFT )
      {
        if ( bKybAction) Keyboard.press(KEY_LEFT_ARROW);
        else Mouse.move(0, 0, 10);
        delay(100);
        Keyboard.releaseAll();
        wTime = ( millis() + scrollNextAction );
      }
      else if ( iAction == SCROLL_RIGHT  )
      {
        if ( bKybAction) Keyboard.press(KEY_RIGHT_ARROW);
        else Mouse.move(0, 0, -10);
        delay(100);
        Keyboard.releaseAll();
        wTime = ( millis() + scrollNextAction );
      }
      else if ( iAction == PUSH )
      {
        wTime = ( millis() + maxWaitNextAction );
        Keyboard.releaseAll();
      }
      else if ( TO_RIGHT >= iAction || iAction <= TO_DOWN )
      {
        if ( !bKybAction )
        { // only for mouse
          if ( TO_RIGHT == iAction ) {
            Mouse.move( moveStep, 0, 0 );
          } else if ( TO_LEFT == iAction ) {
            Mouse.move(-moveStep, 0, 0);
          } else if ( TO_UP == iAction ) {
            Mouse.move(0, -moveStep, 0);
          } else if ( TO_DOWN  == iAction ) {
            Mouse.move(0, moveStep, 0);
          }
          lastAction = iAction;
          wTime = ( millis() + waitTime );
          //          Serial.print("actionIteration="); Serial.println(actionIteration);
          ++actionIteration;
          if ( actionIteration % incRangeCount == 0 )
          {
            moveStep += mouseRange;
          }
        }
        else //// only for KEYBOARD
        {
          if ( TO_RIGHT == iAction )
          {
            Keyboard.press(KEY_RIGHT_ARROW);
            lastAction = iAction;
            delay(100);
            wTime = ( millis() + scrollNextAction );
          } else if ( TO_LEFT == iAction ) {
            Keyboard.press(KEY_LEFT_ARROW);
            lastAction = iAction;
            delay(100);
            wTime = ( millis() + scrollNextAction );
          } else if ( TO_UP == iAction ) {
            Keyboard.press(KEY_UP_ARROW);
            lastAction = iAction;
            delay(100);
            wTime = ( millis() + scrollNextAction );
          } else if ( TO_DOWN  == iAction ) {
            Keyboard.press(KEY_DOWN_ARROW);
            lastAction = iAction;
            delay(100);
            wTime = ( millis() + scrollNextAction );
          }
          Keyboard.releaseAll();
        }
      } // else if (TO_RIGHT >= iAction || iAction <= TO_DOWN)
      else if ( AFTER_MOUSE_DOWN == iAction ) {
        Serial.println("new itertion");
        actionIteration = 0;
        moveStep = mouseRange;
        delay(100);
        Keyboard.releaseAll();

      } else if (B_MAP >= iAction || iAction <= B_LIGHT) {
        //Serial.println("KEYBOARD");
        lastAction = iAction;

        if ( B_SETUP == iAction ) {
          bKybAction = !bKybAction;
          // Serial.print("bKybAction = "); Serial.println(bKybAction);
        } else if ( B_MENU == iAction ) {
          Keyboard.press(KEY_LEFT_GUI);
        } else if ( B_CANCEL == iAction ) {
          Keyboard.press(KEY_ESC);
        }

        delay(100);
        Keyboard.releaseAll();
        wTime = (millis() + maxWaitNextAction);
      }
    } // -1 != iAction
  }
}

