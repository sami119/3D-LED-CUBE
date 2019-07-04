#include <SPI.h>

//pins
#define RED_LED 5
#define GREEN_LED 7
#define BUTTON_1 18
#define BUTTON_2 19

//animations
//#define LIGHTCUBE 0
#define RAIN 0
#define PLANEBOING 1
#define SYMBOL 2
#define GLOW 3
#define TOTAL 4
#define SEND_VOXELS 5
#define WOOP_WOOP 6

//times
#define RAIN_TIME 260
#define GLOW_TIME 8
#define SYMBOL_TIME 300
#define SEND_VOXELS_TIME 140
#define WOOP_WOOP_TIME 350

//shifts
#define NEG_Y 0
#define NEG_Z 1
#define NEG_X 2
#define POS_X 3
#define POS_Z 4
#define POS_Y 5

//axis
#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

//timer
uint16_t timer;

//main variables
int8_t currentEffect;
uint8_t cube[8][8];
bool loading;

void setup() {
  //connection setup
  Serial.begin(9600);
  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));

  //pins setup
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);

  randomSeed(analogRead(0));

  currentEffect = RAIN;
  digitalWrite(GREEN_LED, HIGH);
  loading = false;
}

void loop() {
  //buttons tick
  if (digitalRead(BUTTON_1)) {
    currentEffect ++;
    loading = true;
    clearCube();

    if (currentEffect >= TOTAL) {
      currentEffect = 0;
    }

    loadingLeds();
  }

  if (digitalRead(BUTTON_2)) {
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
    //case LIGHTCUBE: lightCube(); break;
    case RAIN: rain(); break;
    case PLANEBOING: planeBoing(); break;
    case SYMBOL: symbol(); break;
    case GLOW: glow(); break;
    case SEND_VOXELS: sendVoxels(); break;
    case WOOP_WOOP: woopWoop(); break;
  }

  renderCube();
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

      (random(0, 8), 7, random(0, 8));
    }
  }
}

//Symbol Animation 'Database'
uint8_t characters[][8] = //small library with different symbols inside
{
  {0x3C, 0x40, 0x40, 0x3C, 0x40, 0x40, 0x42, 0x3C},//3
  {0x3C, 0x42, 0x40, 0x40, 0x3C, 0x02, 0x02, 0x7E},//2
  {0x10, 0x18, 0x14, 0x10, 0x10, 0x10, 0x10, 0x3C},//1
  {0x66, 0xFF, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18} //<3
};
uint8_t charCounter = 0;
uint8_t charPosition = 0;

//Symbol animation
void symbol() {
  //variables
  char string[] {};
  uint8_t len;

  if (loading)
  {
    clearCube();
    charPosition = -1; //sets position before the the 0 location in the cube
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

      if (charCounter > len - 1)
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
        cube[i][0] = characters[string[charCounter] - '0'][i];
      }
    }
  }
}


void sendVoxels() {

}

//variables for WoopWoop animation
uint8_t cubeSize = 0;
bool CubeExpanding = true;

//woop woop animation
void woopWoop() {
  if (loading) {
    clearCube();
    cubeSize = 2;
    CubeExpanding = true;
    loading = false;
  }
  timer++;

/*if the timer is more than WOOP_WOOP_TIME reset the timer
 * and if CubeExpanding is true the cube will continue to expand
 * until it reaches the size of 8and then the cube stops expanding
*/
  if (timer > WOOP_WOOP_TIME) {
    timer = 0;
    if (CubeExpanding) {
      cubeSize += 2;
      if (cubeSize == 8) {
        CubeExpanding = false;
      }
    }
    /*Starts decreasing the size of the cube until the size of 2 and then
     * sets the CubeExpansion to true with starts the expansion again
     */
    else {
      cubeSize -= 2;
      if (cubeSize == 2) {
        CubeExpanding = true;
      }
    }
    clearCube();
    drawCube(4 - cubeSize / 2, 4 - cubeSize / 2, 4 - cubeSize / 2, cubeSize);
  }
}


//Plane Boing Animation
int planeCounter;

void planeBoing() {
  if (loading) {
    clearCube();
    planeCounter = 0;
    loading = false;
  }

  if (planeCounter == AXIS_X) {
    planeBoing(AXIS_X);
  }

  if (planeCounter == AXIS_Y) {
    planeBoing(AXIS_Y);
  }

  if (planeCounter == AXIS_Z) {
    planeBoing(AXIS_Z);
  }
}

void planeBoing(int plane) {
  for (int i = 0; i < 8; i++) {
    fill(0x00);
    setPlane(plane, i);
    delay(500);
  }

  for (int i = 7; i >= 0; i--) {
    fill(0x00);
    setPlane(plane, i);
    delay(500);
  }
}


//Glowing animation
bool glowing;
uint16_t glowCount = 0;

//select
uint8_t selX = 0;
uint8_t selY = 0;
uint8_t selZ = 0;

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
      if (glowCount < 256) {
        do {
          randomLed();
        }
        while (getVoxel(selX, selY, selZ));

        setVoxel(selX, selY, selZ);
        glowCount++;
      }
      else {
        glowing = false;
        glowCount = 0;
      }
    }
    else {
      if (glowCount < 256) {
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


/*
  //all leds shine
  void lightCube() {
  if (loading) {
    clearCube();
    loading = false;
  }
  for(uint8_t i=0; i<8; i++) {
    for(uint8_t j=0; j<8; j++) {
      cube[i][j] = 0xFF;
    }
  }
  }
*/

//Main methods used in the loop

//fills the cube with pattern of 8bits
void fill(uint8_t pattern) {
  for (int y = 0; y < 8; y++) {
    for (int z = 0; z < 8; z++) {
      cube[y][z] = pattern;
    }
  }
}

//renders the array of bits to lights
void renderCube() {
  for (uint8_t i = 0; i < 8; i++) {
    digitalWrite(SS, LOW);//begins transfering data
    SPI.transfer(0x01 << (7 - i));//y counts from top to bottom
    for (uint8_t j = 0; j < 8; j++) {
      SPI.transfer(cube[i][j]);
    }
    digitalWrite(SS, HIGH);//closes conection
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
  cube[7 - y][7 - z] != (0x01 << x);
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
  if (dir == NEG_Y) {
    for (uint8_t y = 7; y > 0; y--) {
      for (uint8_t z = 1; z < 8; z++) {
        cube[y][z] = cube[y - 1][z];
      }
    }
  }
  else if (dir == NEG_Z) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t z = 7; z > 0; z--) {
        cube[y][z] = cube[y][z - 1];
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
  else if (dir == POS_X) {
    for (uint8_t y = 0; y < 8; y++) {
      for (uint8_t z = 0; z < 8; z++) {
        cube[y][z] = cube[y][z] << 1;
      }
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
