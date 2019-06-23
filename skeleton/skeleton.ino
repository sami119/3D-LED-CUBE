//pins
#define RED_LED 5
#define GREEN_LED 7
#define BUTTON_1 18
#define BUTTON_2 19

//animations
#define ANIMATION_1 0
#define ANIMATION_2 1
#define ANIMATION_3 2

//other
#define TOTAL 3

//variables
int8_t currentEffect;
uint8_t cube[8][8];

void setup() {
  //sets connection
  Serial.begin(9600);
  
  //Sets the initial effect
  currentEffect = ANIMATION_1;
  
  //The switch mode lights
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  //The buttons for changing mode
  pinMode(BUTTON_1, INPUT_PULLUP);
  pinMode(BUTTON_2, INPUT_PULLUP);
  
  analogWrite(GREEN_LED, HIGH);
}

void loop() {
  if(analogRead(BUTTON_1)){
    currentEffect ++;
    if(currentEffect >= TOTAL){
      currentEffect = 0;
    }
  }
  
  if(analogRead(BUTTON_2)){
    currentEffect --;
    if(currentEffect < 0){
      currentEffect = TOTAL - 1;
    }
  }
  
  //changeEffect
  switch(currentEffect){
    case ANIMATION_1: animation1(); break;
    case ANIMATION_2: animation2(); break;
    case ANIMATION_3: animation3(); break;
  }
}

//Animations
void animation1(){
  
}

void animation2(){
  
}

void animation3(){
  
}

void clearCube(){
  for(uint8_t i = 0; i < 8; i++){
   for(uint8_t j = 0; j < 8; j++){
    cube[i][j] = 0; 
   }
  }
}
