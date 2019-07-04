﻿# 3D-LED-CUBE
## Описание:
- 8x8x8 Led Cube
- 4 Анимации
## Схема:
![scheme](https://github.com/sami119/3D-LED-CUBE/blob/master/Schematic/scheme-1.jpg)
## Части:
- 9 броя интегрални схеми 74CH595
- 66 резистора 220 Ω
- 8 транзистора BD241C
- 514 светодиода
- 1 кондензатор 1000 µF
- 2 бутона
- Arduino Nano
## За кода:
Основни методи:
- renderCube - void - основната логика на програмата. Метода започва връзка чрез spi към shift регистрите, като след всеки клок са се пратили данни за конкретен ред
от лампички. двумерния масив cube е 8битов. Като такъв той пази по един ред от лампички с дадено y(височина) и z(дълбочина). За по-лесна работа с някой от
анимациите y e настроен да се брои от горе надолу.
- clearCube - void - Изгасва всички лампи.
- loadingLeds - void - Когато кубчето сменя анимациите светва червения LED, когато вече е смененa светва зеления LED.
- setVoxel - void - светва лампичка по дадено x y z, чрез побитови операции(1 се измества x пъти в шеснадесетичното число 0x01 след което чрез побитово "или" се
добавя към cube[y][z](8битово число)
- clearVoxel - void - изгасва лампичка със дадено x, y и z, чрез побитови операции.
- getVoxel - int - Връща истина, когато даденa лампичка свети и съответно лъжа, когато е изгаснала.
- setPlane - void - светва 8*8 леда във дадена ос и местоположение.
- shift - void - сменя местата на реда от битове x със съседен в зависимост от дадената посоката
- fill - void - запълва куба по даден 8битов патерн.
- randomLed - void - избира на рандом числа от 0 до 8 за x, y, z.
- drawCube - void - цикли s(размера на кубчето) пъти като след всеки цикъл са запалени някой от лампичките формиращи го.

Анимации:
- lightCube - void - всички LED светват.
- rain - анимация пресъздаваща дъжд. Тя използва метода shift като му предава да сменя по y надолу. Инициализира random брой капки и светва лампички на
най-горния ред по рандом y и z;
- planeBoing - 8x8 LED лампи се движат светвайки по дадена ос.
- symbol - предефинирани символи се местят по дадена ос.
- glow - в зависимост от това дали кубчето се запъква или изпразва се изключват или запалват дадени лампички.
- woopwoop - чертае кубче като го увеличава и намалява.
- sendVoxels - светва 64 лампички със рандом z 0 или 7 и random x,y. след което ги shift-ва в противоположната посока, по NEG и POS Z.
## Автори:
[@sami119](https://github.com/sami119) <br />
[@SlavKazakov](https://github.com/SlavKazakov) <br />
[@Stefanstefo](https://github.com/Stefanstefo) <br />
[@gogo213](https://github.com/gogo213) <br />
[@siokatalo](https://github.com/siokatalo)
## Кредити:
Проект използван за референция: <br />
- автор: [AlexGyver](https://github.com/AlexGyver)
- репо: https://github.com/AlexGyver/LEDcube
### Проекта е направен за: НП"Обучение за ИТ кариера"
