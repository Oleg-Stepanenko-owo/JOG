static const int pkgLong = 4;
static const int pkgSize = 13;
static int valInc = 0;

const int pkgData[pkgSize][pkgLong] = {
  {128, 248, 248,  0}, // 0 Scroll LEFT
  {191, 248, 24,   0}, // 1 Scroll RIGHT
  {224, 193, 8,    0}, // 2 Push
  {224, 184, 144,  0}, // 3 to Right
  {224, 232, 48,   0}, // 4 to Left
  {224, 120, 16,  12}, // 5 to Up
  {224, 216, 80,  0},  // 6 to Donw
  {224, 56,  200, 0},  // 7 to Right + Up
  {224, 152, 104, 0},  // 8 to Right + Down
  {224, 104, 152, 0},  // 9 to Left + Up
  {224, 200, 56, 0},   // 10 to Left + Down
  {224, 248, 8, 0},   // 11 after Right, Left, Right+Up,
  {240, 248, 8, 0}    // 12 after Up, Down, Right+Down, Left+UP, Left+Down
};

int pkgVal[pkgLong];

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
  TO_LEFT_DOWN
};

void setup() {
  Serial.begin(4800);
}
void getAction()
{
//  Serial.println("getAction");
//  Serial.println(pkgVal[0]);
//  Serial.println(pkgVal[1]);
//  Serial.println(pkgVal[2]);
//  Serial.println(pkgVal[3]);
//  Serial.println("_____________");
//  Serial.println(pkgData[0][0]);
//  Serial.println(pkgData[0][1]);
//  Serial.println(pkgData[0][2]);
//  Serial.println(pkgData[0][3]);
  for ( int a = 0; a < pkgSize; a++ ) {
    if ( pkgData[a][0] == pkgVal[0] && pkgData[a][1] == pkgVal[1]
         && pkgData[a][2] == pkgVal[2] && pkgData[a][3] == pkgVal[3]) {
      Serial.print("ACTION = "); Serial.println(a);
    }
  }
}

void loop() {
  // int receivedBytes = 0;
  if ( Serial.available() ) {
    //Serial.print("Received size = "); Serial.println( receivedBytes );
    //Serial.print( Serial.read() );
    //Serial.println(valInc);
    pkgVal[valInc++] = Serial.read();
//    Serial.print("pkgVal"); Serial.println(pkgVal[valInc-1]);   
    //Serial.print("valInc"); Serial.println(valInc);
    if ( ((pkgVal[valInc-1] == 0) || (pkgVal[valInc-1] == 12)) && (valInc == pkgLong) ) {
      getAction();
      valInc = 0;
      return;
    }
    
    if ( valInc == pkgLong ) {
      valInc = 0;
    }
  }
}
