# Bong

Bong is a simple 2D table tennis game demonstrating basic capabilities of Blinkblind and SDL3.

## Project Scope

Project is currently under development. Our plan for Bong is;

1. Separating general functions into Blinkblind
2. Creating a full game with intro, menu and options.
3. We might include customization, game saves and replay abilities depending on the situation.

## Features

- Utilizing SDL3 callback system

## History of the Project

The development of the first version of this project, `Pong the Game', was started on October 8, 2014, at 5:11, a day with Total Lunar Eclipse (Blood Moon). It was my first proper attempt at developing games. This version was finished on December 20, 2014, at 5:06.

The first version had a 1500x900 window, written in C++ and using lots of pieces of code from Lazy Foo's tutorials. For this reason, although it was public, project was not under proper open-source license. It also didn't use VSYNC or FPS cap, resulting in different game speeds on different machines (some games were faster than human reaction) and over-used GPU.

Second attempt, which took place between June 6, 19:38 to June 7, 18:41, 2023 (less than a day), was a total rewrite without using any outside code and limiting project to C99 and using black-and-white colors only. Source code was dropped from 497 LOC to 224 LOC. It was very basic, but that was intentional.

The current version, specially named `Bong', is another full rewrite. The purpose of this rewrite is moving the game to new SDL3 library, along with that creating a custom library called Blinkblind that will be used across our other simple games and adding the game more functionality.

## License

Bong is under [MIT License](./LICENSE).
