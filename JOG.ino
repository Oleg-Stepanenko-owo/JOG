// only for Arduino like Leonardo

#include "Keyboard.h"
#include "Mouse.h"
#include <SoftwareSerial.h>

#define VERSIO_JOG "0.4"

SoftwareSerial mySerial(10, 11); // RX, TX
//---------------------------------------------------------------------------
static const int pkgLong = 2;
static const int pkgSize = 25;
static int valInc = 0;
//---------------------------------------------------------------------------

const int pkgData[pkgSize][pkgLong] = {
  {224, 62}, // 0 Scroll LEFT
  {227, 62}, // 1 Scroll RIGHT
  {28, 34},  // 2 Push
  {124, 67}, // 3 to Right
  {124, 71}, // 4 to Left
  {124, 81}, // 5 to Up
  {124, 99}, // 6 to Donw
  {224, 56}, // 7 to Right + Up
  {224, 152},// 8 to Right + Down
  {224, 104},// 9 to Left + Up
  {224, 200},// 10 to Left + Down
  {124, 34}, // 11 after Right, Left, Right+Up,
  {124, 34}, // 12 after Up, Down, Right+Down, Left+UP, Left+Down
  {231, 62}, // 13 SCROLL_RIGHT_1
  {235, 62}, // 14 SCROLL_RIGHT_2
  {232, 62},  // 15 SCROLL_LEFT_1
  {56, 34},   // 16 Map/guide
  {124, 192}, // 17 Audio
  {124, 200}, // 18 Cancel
  {60, 50},   // 19 Menu
  {34, 52},   // 20 A/C
  {44, 34},   // 21 Info
  {60, 42},   // 22 Setup
  {60, 68},   // 23 Light
  { -1, -1}   // 24 Long Push
};

//---------------------------------------------------------------------------
int pkgVal[pkgLong];
const int waitTime = 150; //300 mSec
const int maxWaitNextAction = 500;
const int mouseRange = 25;
const int incRangeCount = 3;
const int longpushtime =  1000;

int lastAction;
int actionIteration;
int moveStep;

unsigned long wTime;
unsigned long wFinishActionTime;
unsigned long wIsPush;
unsigned long wIsLongPush;

bool bKybAction;
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
  TO_LEFT_DOWN,
  AFTER_DOWN_1,
  AFTER_DOWN_2,
  SCROLL_RIGHT_1,
  SCROLL_RIGHT_2,
  SCROLL_LEFT_1,
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
  Serial.begin(9600);
  mySerial.begin(2400);

  delay(100);
  Serial.print("Start jog version:"); Serial.println( VERSIO_JOG );
  lastAction = -1;
  actionIteration = 0;
  moveStep = mouseRange;
  wTime = 0;
  wFinishActionTime = 0;

  // initialize mouse control:
  Mouse.begin();
  Keyboard.begin();

  bKybAction = true;
}

//---------------------------------------------------------------------------
int getAction()
//---------------------------------------------------------------------------
{
  for ( int a = 0; a < pkgSize; a++ ) {
    if ( pkgData[a][0] == pkgVal[0] && pkgData[a][1] == pkgVal[1] ) {
      //      Serial.print("pkgVal = "); Serial.print(pkgVal[0]);
      //      Serial.print(" "); Serial.println(pkgVal[1]);
      switch ( a ) {
        case SCROLL_LEFT :
        case SCROLL_LEFT_1 :
          if ( bKybAction) Keyboard.press(KEY_LEFT_ARROW);
          else Mouse.move(0, 0, 10);
          return SCROLL_LEFT;
        case SCROLL_RIGHT :
        case SCROLL_RIGHT_1 :
        case SCROLL_RIGHT_2 :
          {
            if ( bKybAction) Keyboard.press(KEY_RIGHT_ARROW);
            else Mouse.move(0, 0, -10);
          }
          return SCROLL_RIGHT;
        case PUSH :
          //          {
          //            if ( bKybAction) Keyboard.press( KEY_RETURN );
          //            else Mouse.click();
          //          delay(100);
          //          }
          return PUSH;
        case TO_RIGHT :
          {
            if ( bKybAction) Keyboard.press(KEY_RIGHT_ARROW);
            else Mouse.move(moveStep, 0, 0);
          }
          return TO_RIGHT;
        case TO_LEFT :
          {
            if ( bKybAction) Keyboard.press(KEY_LEFT_ARROW);
            else Mouse.move(-moveStep, 0, 0);
          }
          return TO_LEFT;
        case TO_UP :
          {
            if ( bKybAction) Keyboard.press(KEY_UP_ARROW);
            else Mouse.move(0, -moveStep, 0);
          }
          return TO_UP;
        case TO_DOWN :
          {
            if ( bKybAction) Keyboard.press(KEY_DOWN_ARROW);
            else Mouse.move(0, moveStep, 0);
          }
          return TO_DOWN;
        case TO_RIGHT_UP :
          return TO_RIGHT_UP;
        case TO_RIGHT_DOWN :
          return TO_RIGHT_DOWN;
        case TO_LEFT_UP :
          return TO_LEFT_UP;
        case TO_LEFT_DOWN :
          return TO_LEFT_DOWN;
        case AFTER_DOWN_1 :
        case AFTER_DOWN_2 :
          Keyboard.releaseAll();
          return AFTER_DOWN_1;
        case  B_MAP:
          return B_MAP;
        case B_AUDIO :
          return B_AUDIO;
        case B_CANCEL :
          Keyboard.press(KEY_ESC);
          return B_CANCEL;
        case B_MENU :
          Keyboard.press(KEY_LEFT_GUI);
          return B_MENU;
        case  B_AC :
          return B_AC;
        case B_INFO :
          return B_INFO;
        case B_SETUP :
          return B_SETUP;
        case B_LIGHT :
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
  if ( mySerial.available() ) {
    pkgVal[valInc++] = mySerial.read();
    Serial.print(pkgVal[valInc - 1]); Serial.println();
    if ( wTime && (wTime > millis()) ) {
      valInc = 0;
      //Serial.print("1 ");
      return;
    }
    wTime = 0;
    // Serial.println(pkgVal[valInc - 1]);
    if ( (!pkgVal[valInc - 1])  || (valInc > pkgLong) ) {
      valInc = 0;
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
    Serial.println("PUSH!!!");
    lastAction = 0;
    Keyboard.releaseAll();
  } else if ( ( 0 != wIsLongPush) && ( wIsLongPush < millis()) ) {
    // Long PUSH
    wIsPush = 0;
    wIsLongPush = 0;
    Serial.println("LONG PUSH!!!");
    lastAction = 0;
    delay(300);
    Keyboard.releaseAll();
    wTime = (millis() + 300);
  }

  if ( valInc == pkgLong ) {
    int iAction = getAction();

    if ( PUSH == iAction ) {
      if ( lastAction == PUSH ) {
        wIsPush = millis() + 200;
        valInc = 0;
      }
      else {
        Serial.println("!2!");
        wIsPush = millis() + 200;
        wIsLongPush = millis() + 500;
        lastAction = PUSH;
        valInc = 0;
      }
      return;
    }
    else if ( ( -1 != iAction ) && ( B_LIGHT >= iAction ) ) {
      Serial.println(" "); Serial.print("Action:"); Serial.println(iAction);

      valInc = 0;
      if ( (iAction == SCROLL_LEFT) || (iAction == SCROLL_LEFT_1) )
      {
        Keyboard.releaseAll();
        wTime = ( millis() + 300 );
      }
      else if ( (iAction == SCROLL_RIGHT) || (iAction == SCROLL_RIGHT_1) || (iAction == SCROLL_RIGHT_2)  )
      {
        Keyboard.releaseAll();
        wTime = ( millis() + 300 );
      }
      else if ( iAction == PUSH )
      {
        wTime = ( millis() + 600 );
        Keyboard.releaseAll();
      }
      else if (TO_RIGHT >= iAction || iAction <= TO_DOWN)
      {
        if ( !bKybAction )
        { // only for mouse

          if ( iAction == lastAction )
          {
            if ( (wFinishActionTime + maxWaitNextAction) < millis() )
            {
              actionIteration = 0;
              moveStep = mouseRange;
            }

            Serial.print("actionIteration="); Serial.println(actionIteration);
            ++actionIteration;
            if ( actionIteration % incRangeCount == 0 )
            {
              moveStep += mouseRange;
            }
            wFinishActionTime = millis();
          }
          else
          {
            lastAction = iAction;
            actionIteration = 0;
            moveStep = mouseRange;
          }
          wTime = (millis() + waitTime);
        }
        else
        {
          delay(100);
          Keyboard.releaseAll();
          wTime = (millis() + waitTime);
        }
      } // else if (TO_RIGHT >= iAction || iAction <= TO_DOWN)
      else if (B_MAP >= iAction || iAction <= B_LIGHT)
      {
        if (B_SETUP == iAction ) {
          bKybAction = !bKybAction;
          Serial.print("bKybAction = "); Serial.println(bKybAction);
        }

        valInc = 0;
        delay(100);
        Keyboard.releaseAll();
        wTime = (millis() + waitTime);
      }
    } // -1 != iAction
  }
  //valInc = 0;
}

