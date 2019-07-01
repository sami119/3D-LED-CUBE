# 3D-LED-CUBE
## Description:
- 8x8x8 Led Cube
- 3 Animations
## Scheme:
![scheme](https://github.com/sami119/3D-LED-CUBE/blob/master/Schematic/scheme-1.jpg)
## Particles:
//TODO
## About the code:
- clearCube - Clears the cube of any running animations.
- loadingLeds - When loading: the leds are shining red.
- renderCube - основната логика на програмата. Метода започва връзка чрез spi към shift регистрите, като след всеки клок са се пратили данни за конкретен ред от лампички.
двумерния масив cube е 8битов. Като такъв той пази по един ред от лампички с дадено y(височина) и z(дълбочина). За по-лесна работа с някой от анимациите y e настроен да се брои от горе надолу.
- lightCube - lights all the leds.
- rain - анимация пресъздаваща дъжд. Тя използва метода shift като му предава да сменя по y надолу. Инициализира random брой капки и светва лампички на най горния ред по рандом y и z;
- shift - сменя местата на реда от битове x със съседен в зависимост от дадената посоката
- setVoxel - светва лампичка по дадено x y z
## Autors:
[@sami119](https://github.com/sami119) <br />
[@SlavKazakov](https://github.com/SlavKazakov) <br />
[@Stefanstefo](https://github.com/Stefanstefo) <br />
[@gogo213](https://github.com/gogo213) <br />
[@siokatalo](https://github.com/siokatalo)
## Credits:
Project used for reference: <br />
- autor: [AlexGyver](https://github.com/AlexGyver)
- repo: https://github.com/AlexGyver/LEDcube
### Project made for НП"Обучение за ИТ кариера"
