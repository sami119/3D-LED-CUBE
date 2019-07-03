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

//times
#define RAIN_TIME 260
#define GLOW_TIME 8

//shifts
#define NEG_Y 0

//variables
uint16_t timer;
uint16_t modeTimer;
int8_t currentEffect;
uint8_t cube[8][8];
bool loading;
uint8_t selX = 0;
uint8_t selY = 0;
uint8_t selZ = 0;

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
  if(digitalRead(BUTTON_1)){
    currentEffect ++;
    loading = true;
    clearCube();
    if(currentEffect >= TOTAL){
      currentEffect = 0;
    }
    loadingLeds();
  } 
  if(digitalRead(BUTTON_2)){
    currentEffect --;
    loading = true;
    clearCube();
    if(currentEffect < 0){
      currentEffect = TOTAL - 1;
    }
    loadingLeds();
  }
  
  //change effects
  switch(currentEffect){
    //case LIGHTCUBE: lightCube(); break;
    case RAIN: rain(); modeTimer = RAIN_TIME; break;
    case PLANEBOING: planeBoing(); break;
    case SYMBOL: symbol(); break;
    case GLOW: glow(); break; 
  }
  renderCube();
}

//Animations
/*
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

void rain(){
  if(loading){
    clearCube();
    loading = false;
  }
  timer++;
  if(timer > modeTimer){
    timer = 0;
    shift(NEG_Y);
    uint8_t numDrops = random(0, 5);
    for(uint8_t i = 0; i < numDrops; i++){
      setVoxel(random(0,8), 7, random(0,8));
    }
  }
}

void planeBoing(){
  
}

void symbol(){
  
}

bool glowing;
uint16_t glowCount = 0;

void glow(){
 if (loading) {
    clearCube();
    glowCount = 0;
    glowing = true;
    loading = false;
  }
 timer++;
  if(timer > GLOW_TIME){
    timer = 0;
      if(glowing){
        if(glowCount < 256){
          do{
            randomLED();    
          }while (getVoxel(selX, selY, selZ));
        setVoxel(selX, selY, selZ);
        glowCount++; 
        }else{
        glowing = false;
        glowCount = 0;
      }      
  }else{
    if(glowCount < 256){
        do{
           randomLED();
          }while (!getVoxel(selX, selY, selZ));
        clearVoxel(selX, selY, selZ);
        glowCount++; 
      }else{
        clearCube();
        glowing = true;
        glowCount = 0;
      }
    }
  }
}

//Main methods used in the loop
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
void clearCube(){
  for(uint8_t i = 0; i < 8; i++){
   for(uint8_t j = 0; j < 8; j++){
    cube[i][j] = 0; 
   }
  }
}

//When loading: the leds are shining red.
void loadingLeds(){
  timer = 0;
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
  delay(500);
  digitalWrite(RED_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
}

//Voxel control
//shines the led by given y, z and x 
void setVoxel(uint8_t x, uint8_t y, uint8_t z){
  cube[7-y][7-z] != (0x01 << x);
}
void clearVoxel(uint8_t x, uint8_t y, uint8_t z) {
  cube[7 - y][7 - z] ^= (0x01 << x);
}
bool getVoxel(uint8_t x, uint8_t y, uint8_t z) {
  return (cube[7 - y][7 - z] & (0x01 << x)) == (0x01 << x);
}
//shifts the lights in the given direction
void shift(uint8_t dir){
 if(dir == NEG_Y){
  for(uint8_t y = 7; y > 0; y--){
    for(uint8_t z = 1; z < 8; z++){
      cube[y][z] = cube[y - 1][z];
    }
  }
 }
}
void randomLED(){
  selX = random(0,8);
  selY = random(0,8);
  selZ = random(0,8);   
}
