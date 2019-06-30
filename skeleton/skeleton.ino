#include <SPI.h>

//pins
#define RED_LED 5
#define GREEN_LED 7
#define BUTTON_1 18
#define BUTTON_2 19

//animations
#define LIGHTCUBE 0
#define ANIMATION_2 1
#define ANIMATION_3 2

//other
#define TOTAL 3

//variables
uint64_t randomTimer;
int8_t currentEffect;
uint8_t cube[8][8];
bool loading;

void setup() {
  //sets connection
  Serial.begin(9600);
  SPI.begin();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  
  //Sets the initial effect
  loading = true;
  currentEffect = LIGHTCUBE;
  
  //The switch mode lights
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  //The buttons for changing mode
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);

  randomSeed(analogRead(0));
  digitalWrite(GREEN_LED, HIGH);
}

void loop() {
  randomTimer++;

  //buttons check
  if(digitalRead(BUTTON_1)){
    clearCube();
    loading = true;
    currentEffect ++;
    if(currentEffect >= TOTAL){
      currentEffect = 0;
    }
    loadingLeds();
    randomSeed(randomTimer);
    randomTimer = 0;
  }
  
  if(analogRead(BUTTON_2)){
    clearCube();
    loading = true;
    currentEffect --;
    if(currentEffect < 0){
      currentEffect = TOTAL - 1;
    }
    loadingLeds();
    randomSeed(randomTimer);
    randomTimer = 0;
  }
  
  //changeEffect
  switch(currentEffect){
    case LIGHTCUBE: lightCube(); break;
    case ANIMATION_2: animation2(); break;
    case ANIMATION_3: animation3(); break;
  }
}

//Animations
//Test animation - Checks if all leds are responive.
void lightCube() {
  if (loading) {
    clearCube();
    for(uint8_t i=0; i<8; i++) {
      for(uint8_t j=0; j<8; j++) {
        cube[i][j] = 0xFF;
      }
    }
    loading = false;
  }

void animation2(){
  
}

void animation3(){
  
}

//Main methods used in the loop

//transfers all the data from the array to hexademical and connects to the leds
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

//Clears the cube of any running animations.
void clearCube(){
  for(uint8_t i = 0; i < 8; i++){
   for(uint8_t j = 0; j < 8; j++){
    cube[i][j] = 0; 
   }
  }
}

//When loading the leds are shining red.
void loadingLeds(){
    digitalWrite(RED_LED, HIGH);
    digitalWrite(GREEN_LED, LOW);
    delay(500);
    digitalWrite(RED_LED, LOW);
    digitalWrite(GREEN_LED, HIGH);
}
