#include <SPI.h>

//pins
#define RED_LED 14
#define GREEN_LED 15
#define BUTTON_1 18
#define BUTTON_2 19

//animations
#define LIGHTCUBE 0
#define RAIN 1
#define PLANEBOING 2
#define SYMBOL 3
#define GLOW 4
#define SEND_VOXELS 5
#define WOOP_WOOP 6
//#define PATERN 7
#define TOTAL 7

//times
#define RAIN_TIME 260
#define GLOW_TIME 8
#define SYMBOL_TIME 300
#define SEND_VOXELS_TIME 140
#define WOOP_WOOP_TIME 350
#define PLANE_BOING_TIME 300

//shifts
#define POS_X 0
#define NEG_X 1
#define POS_Y 2
#define NEG_Y 3
#define POS_Z 4
#define NEG_Z 5


//axis
#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

//timer
uint16_t timer;

//select
uint8_t selX = 0;
uint8_t selY = 0;
uint8_t selZ = 0;

//main variables
uint8_t cube[8][8];
int8_t currentEffect;
bool loading;

void setup() {
  //connection setup
  Serial.begin(9600);
  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  loading = true;
  currentEffect = LIGHTCUBE;

  //pins setup
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);

  randomSeed(analogRead(0));
  digitalWrite(GREEN_LED, HIGH);
}

void loop() {
  //buttons tick
  if (digitalRead(BUTTON_1) == LOW) {
    currentEffect ++;
    loading = true;
    clearCube();

    if (currentEffect >= TOTAL) {
      currentEffect = 0;
    }

    loadingLeds();
  }

  if (digitalRead(BUTTON_2) == LOW) {
    currentEffect --;
    loading = true;
    clearCube();

    if (currentEffect < 0) {
      currentEffect = TOTAL - 1;
    }

    loadingLeds();
  }

  //change effects
  switch (currentEffect) {
    case LIGHTCUBE: lightCube(); break;
    case RAIN: rain(); break;
    case PLANEBOING: planeBoing(); break;
    case SYMBOL: symbol("3210", 4); break;
    case GLOW: glow(); break;
    case SEND_VOXELS: sendVoxels(); break;
    case WOOP_WOOP: woopWoop(); break;
      //case PATERN: pattern(); break;
  }

  renderCube();
}

void renderCube() {
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(SS, LOW);
    SPI.transfer(0x01 << i);

    for (uint8_t j = 0; j < 8; j++) {
      SPI.transfer(cube[i][j]);
    }

    digitalWrite(SS, HIGH);
  }
}

//Animations
//RAIN animation
void rain() {
  if (loading) {
    clearCube();
    loading = false;
  }

  timer++;

  if (timer > RAIN_TIME) {
    uint8_t numDrops = random(0, 5);
    timer = 0;
    shift(NEG_Y);

    for (uint8_t i = 0; i < numDrops; i++) {
      setVoxel(random(0, 8), 7, random(0, 8));
    }
  }
}

//Symbol Animation 'Database'
uint8_t characters[4][8] = //small library with different symbols inside
{
  {0x66, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18}, //<3
  {0x10, 0x18, 0x14, 0x10, 0x10, 0x10, 0x10, 0x3C}, //1
  {0x3C, 0x42, 0x40, 0x40, 0x3C, 0x02, 0x02, 0x7E}, //2
  {0x0E, 0x11, 0x10, 0x0C, 0x10, 0x10, 0x11, 0x0E}, //3
};

uint8_t charCounter = 0;
uint8_t charPosition = 0;

//Symbol animation
void symbol(char str[], uint8_t num) {
  if (loading)
  {
    clearCube();
    charPosition = -1; //sets position before the 0 location in the cube
    charCounter = 0; //begins the chars in the characters 'database'
    loading = false;
  }

  timer++; //begins timer

  if (timer > SYMBOL_TIME)
  {
    timer = 0;

    shift(NEG_Z);
    charPosition++;

    if (charPosition == 7)
    {
      /*when the position of the char is at the end of the cube
        it begins the animation movement again
      */
      charCounter++;

      if (charCounter > num - 1)
      {
        charCounter = 0;
      }

      charPosition = 0;
    }

    //movement of the character
    if (charPosition == 0)
    {
      for (uint8_t i = 0; i < 8; i++)
      {
        cube[i][0] = characters[str[charCounter] - '0'][i];
      }
    }
  }
}

//SEND VOXELS animation
uint8_t sendDirection = 0;
bool sending = false;

void sendVoxels() {
  if (loading) {
    clearCube();

    for (uint8_t x = 0; x < 8; x++) {
      for (uint8_t z = 0; z < 8; z++) {
        setVoxel(x, random(0, 2) * 7, z);
      }
    }

    loading = false;
  }

  timer++;

  if (timer > SEND_VOXELS_TIME) {
    timer = 0;

    if (!sending) {
      selX = random(0, 8);
      selZ = random(0, 8);

      if (getVoxel(selX, 0, selZ)) {
        selY = 0;
        sendDirection = POS_Y;

      }
      else if (getVoxel(selX, 7, selZ)) {
        selY = 7;
        sendDirection = NEG_Y;
      }

      sending = true;
    }
    else {
      if (sendDirection == POS_Y) {
        selY++;
        setVoxel(selX, selY, selZ);
        clearVoxel(selX, selY - 1, selZ);

        if (selY == 7) {
          sending = false;
        }
      }
      else {
        selY--;
        setVoxel(selX, selY, selZ);
        clearVoxel(selX, selY + 1, selZ);

        if (selY == 0) {
          sending = false;
        }
      }
    }
  }
}

//variables for WoopWoop animation
uint8_t cubeSize = 0;
bool cubeExpanding = true;

//woop woop animation
void woopWoop() {
  if (loading) {
    clearCube();
    cubeSize = 2;
    cubeExpanding = true;
    loading = false;
  }

  timer++;
  /*if the timer is more than WOOP_WOOP_TIME reset the timer
     and if CubeExpanding is true the cube will continue to expand
     until it reaches the size of 8and then the cube stops expanding
  */
  if (timer > WOOP_WOOP_TIME) {
    timer = 0;

    if (cubeExpanding) {
      cubeSize += 2;

      if (cubeSize == 8) {
        cubeExpanding = false;
      }
    }
    /*Starts decreasing the size of the cube until the size of 2 and then
       sets the CubeExpansion to true with starts the expansion again
    */
    else {
      cubeSize -= 2;

      if (cubeSize == 2) {
        cubeExpanding = true;
      }
    }

    clearCube();
    drawCube(4 - cubeSize / 2, 4 - cubeSize / 2, 4 - cubeSize / 2, cubeSize);
  }
}

//Plane Boing Animation
uint8_t planePosition = 0;
uint8_t planeDirection = 0;
bool looped = false;

void planeBoing() {
  if (loading) {
    uint8_t axis = random(0, 3);
    planePosition = random(0, 2) * 7;
    timer = 0;
    looped = false;
    loading = false;

    clearCube();
    setPlane(axis, planePosition);
    setPlaneDirection(axis);
  }
  timer++;

  if (timer > PLANE_BOING_TIME) {
    timer = 0;
    shift(planeDirection);

    if (planeDirection % 2 == 0) {
      planePosition++;

      if (planePosition == 7) {
        if (looped) {
          loading = true;
        }
        else {
          planeDirection++;
          looped = true;
        }
      }
    }
    else {
      planePosition--;

      if (planePosition == 0) {
        if (looped) {
          loading = true;
        }
        else {
          planeDirection--;
          looped = true;
        }
      }
    }
  }
}

void setPlaneDirection(uint8_t axis) {
  switch (axis) {
    case AXIS_X:
      if (planePosition == 0) {
        planeDirection = POS_X;
      }
      else {
        planeDirection = NEG_X;
      }
      break;
    case AXIS_Y:
      if (planePosition == 0) {
        planeDirection = POS_Y;
      }
      else {
        planeDirection = NEG_Y;
      }
      break;
    case AXIS_Z:
      if (planePosition == 0) {
        planeDirection = POS_Z;
      }
      else {
        planeDirection = NEG_Z;
      }
      break;
  }
}

//Glowing animation
bool glowing;
uint16_t glowCount = 0;

void glow() {
  if (loading) {
    clearCube();
    glowCount = 0;
    glowing = true;
    loading = false;
  }
  timer++;

  if (timer > GLOW_TIME) {
    timer = 0;

    if (glowing) {
      if (glowCount < 448) {
        do {
          randomLed();
        }
        while (getVoxel(selX, selY, selZ));

        setVoxel(selX, selY, selZ);
        glowCount++;
      }
      else if (glowCount < 512) {
        for (uint8_t i = 0; i < 8; i++) {
          for (uint8_t j = 0; j < 8; j++) {
            cube[i][j] = 0xFF;
          }
        }
        glowCount++;
      }
      else {
        glowing = false;
        glowCount = 0;
      }
    }
    else {
      if (glowCount < 448) {
        do {
          randomLed();
        }
        while (!getVoxel(selX, selY, selZ));

        clearVoxel(selX, selY, selZ);
        glowCount++;
      }
      else {
        clearCube();
        glowing = true;
        glowCount = 0;
      }
    }
  }
}


void randomLed() {
  selX = random(0, 8);
  selY = random(0, 8);
  selZ = random(0, 8);
}


//lightCube animation
void lightCube() {
  if (loading) {
    clearCube();
    for (uint8_t i = 0; i < 8; i++) {
      for (uint8_t j = 0; j < 8; j++) {
        cube[i][j] = 0xFF;
      }
    }
    loading = false;
  }
}

//pattern animation
bool full;

void pattern() {
  if (loading) {
    clearCube();
    loading = false;
    full = false;
  }
  if (full) {
    fill(0x24);
    delay(500);
    fill(0x42);
    delay(500);
    full = false;
  }
  else {
    fill(0x81);
    delay(500);
    fill(0x42);
    delay(500);
    fill(0x24);
    delay(500);
    fill(0x18);
    delay(500);
    full = true;
  }
}

//Main methods used in the loop
//fills the cube with pattern of 8bits
void fill(uint8_t pattern) {
  for (int y = 0; y < 8; y++) {
    for (int z = 0; z < 8; z++) {
      cube[y][z] = pattern;
    }
  }
}

//Clears the cube
void clearCube() {
  for (uint8_t i = 0; i < 8; i++) {
    for (uint8_t j = 0; j < 8; j++) {
      cube[i][j] = 0;
    }
  }
}

//When loading: the leds are shining red.
void loadingLeds() {
  timer = 0;
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  delay(500);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
}

//Voxel control
//shines the led by given y, z and x
void setVoxel(uint8_t x, uint8_t y, uint8_t z) {
  cube[7 - y][7 - z] |= (0x01 << x);
}

//clears the led by given x, y and z
void clearVoxel(uint8_t x, uint8_t y, uint8_t z) {
  cube[7 - y][7 - z] ^= (0x01 << x);
}

//returns true if the given led is shining
bool getVoxel(uint8_t x, uint8_t y, uint8_t z) {
  return (cube[7 - y][7 - z] & (0x01 << x)) == (0x01 << x);
}

//shifts the lights in the given direction
void shift(uint8_t dir) {
  if (dir == POS_X) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t z = 0; z < 8; z++) {
        cube[y][z] = cube[y][z] << 1;
      }
    }
  }
  else if (dir == NEG_X) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t z = 0; z < 8; z++) {
        cube[y][z] = cube[y][z] >> 1;
      }
    }
  }

  else if (dir == POS_Y) {
    for (uint8_t y = 1; y < 8; y++) {
      for (uint8_t z = 0; z < 8; z++) {
        cube[y - 1][z] = cube[y][z];
      }
    }

    for (uint8_t i = 0; i < 8; i++) {
      cube[7][i] = 0;
    }
  }
  else if (dir == NEG_Y) {
    for (uint8_t y = 7; y > 0; y--) {
      for (uint8_t z = 0; z < 8; z++) {
        cube[y][z] = cube[y - 1][z];
      }
    }

    for (uint8_t i = 0; i < 8; i++) {
      cube[0][i] = 0;
    }
  }

  else if (dir == POS_Z) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t z = 1; z < 8; z++) {
        cube[y][z - 1] = cube[y][z];
      }
    }

    for (uint8_t i = 0; i < 8; i++) {
      cube[i][7] = 0;
    }
  }
  else if (dir == NEG_Z) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t z = 7; z > 0; z--) {
        cube[y][z] = cube[y][z - 1];
      }
    }

    for (uint8_t i = 0; i < 8; i++) {
      cube[i][0] = 0;
    }
  }
}

//sets a plane of 8x8 bits to light
void setPlane(int axis, uint8_t i) {
  for (uint8_t j = 0; j < 8; j++) {
    for (uint8_t k = 0; k < 8; k++) {
      switch (axis) {
        case AXIS_X: setVoxel(i, j, k); break;
        case AXIS_Y: setVoxel(j, i, k); break;
        case AXIS_Z: setVoxel(j, k, i); break;
      }
    }
  }
}

//draws a cube using the setvoxel method
void drawCube(uint8_t x, uint8_t y, uint8_t z, uint8_t s) {
  for (uint8_t i = 0; i < s; i++) {
    setVoxel(x, y + i, z);
    setVoxel(x + i, y, z);
    setVoxel(x, y, z + i);
    setVoxel(x + s - 1, y + i, z + s - 1);
    setVoxel(x + i, y + s - 1, z + s - 1);
    setVoxel(x + s - 1, y + s - 1, z + i);
    setVoxel(x + s - 1, y + i, z);
    setVoxel(x, y + i, z + s - 1);
    setVoxel(x + i, y + s - 1, z);
    setVoxel(x + i, y, z + s - 1);
    setVoxel(x + s - 1, y, z + i);
    setVoxel(x, y + s - 1, z + i);
  }
}
