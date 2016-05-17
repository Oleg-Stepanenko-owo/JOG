// only for Arduino like Leonardo

#include "Keyboard.h"
#include "Mouse.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(10, 11); // RX, TX
//---------------------------------------------------------------------------

static const int pkgLong = 2;
static const int pkgSize = 13;
static int valInc = 0;
//---------------------------------------------------------------------------

//const int pkgData[pkgSize][pkgLong] = {
//  {128, 248, 248,  0}, // 0 Scroll LEFT
//  {191, 248, 24,   0}, // 1 Scroll RIGHT
//  {224, 193, 8,    0}, // 2 Push
//  {224, 184, 144,  0}, // 3 to Right
//  {224, 232, 48,   0}, // 4 to Left
//  {224, 120, 16,  12}, // 5 to Up
//  {224, 216, 80,  0},  // 6 to Donw
//  {224, 56,  200, 0},  // 7 to Right + Up
//  {224, 152, 104, 0},  // 8 to Right + Down
//  {224, 104, 152, 0},  // 9 to Left + Up
//  {224, 200, 56,  0},  // 10 to Left + Down
//  {224, 248, 8,   0},  // 11 after Right, Left, Right+Up,
//  {240, 248, 8,   0}   // 12 after Up, Down, Right+Down, Left+UP, Left+Down
//};
const int pkgData[pkgSize][pkgLong] = {
  {224, 62}, // 0 Scroll LEFT
  {227, 62}, // 1 Scroll RIGHT
  {28, 34}, // 2 Push
  {124, 67}, // 3 to Right
  {124, 71}, // 4 to Left
  {124, 81}, // 5 to Up
  {124, 99},  // 6 to Donw
  {224, 56},  // 7 to Right + Up
  {224, 152},  // 8 to Right + Down
  {224, 104},  // 9 to Left + Up
  {224, 200},  // 10 to Left + Down
  {124, 34},  // 11 after Right, Left, Right+Up,
  {124, 34}   // 12 after Up, Down, Right+Down, Left+UP, Left+Down
};

int pkgVal[pkgLong];
const int waitTime = 200; //300 mSec
unsigned long wTime;

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
  AFTER_DOWN_2
};

//---------------------------------------------------------------------------
void setup()
//---------------------------------------------------------------------------
{
  mySerial.begin(2400);
  Serial.begin(9600);
  // initialize mouse control:
  Mouse.begin();
  Keyboard.begin();
}

//---------------------------------------------------------------------------
int getAction()
//---------------------------------------------------------------------------
{
  for ( int a = 0; a < pkgSize; a++ ) {
    if ( pkgData[a][0] == pkgVal[0] && pkgData[a][1] == pkgVal[1] ) {
      Serial.print("ACTION = "); Serial.println(a);
      switch ( a ) {
        case SCROLL_LEFT :
          Keyboard.press(KEY_LEFT_SHIFT);
          Keyboard.press(KEY_TAB);
          return SCROLL_LEFT;
        case SCROLL_RIGHT :
          Keyboard.press(KEY_TAB);
          return SCROLL_RIGHT;
        case PUSH :
          Keyboard.press(KEY_RETURN);
          return PUSH;
        case TO_RIGHT :
          Keyboard.press(KEY_RIGHT_ARROW);
          return TO_RIGHT;
        case TO_LEFT :
          Keyboard.press(KEY_LEFT_ARROW);
          return TO_LEFT;
        case TO_UP :
          Keyboard.press(KEY_UP_ARROW);
          return TO_UP;
        case TO_DOWN :
          Keyboard.press(KEY_DOWN_ARROW);
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

    if ( wTime && (wTime > millis()) ) {
      valInc = 0;
      return;
    }  wTime = 0;

    Serial.println(pkgVal[valInc - 1]);
    if ( pkgVal[valInc - 1] == 0 ) {
      valInc = 0;
      return;
    }
    if ( valInc == pkgLong ) {
      if ( -1 != getAction() ) {
        delay(100);
        Keyboard.releaseAll();
        wTime = (millis() + waitTime);
      }
      valInc = 0;
    }
  }
}
