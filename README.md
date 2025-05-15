# README

## XONIX - C++ SFML Game Project

A 2D Xonix-style arcade game built using C++ and the SFML 3.0.0 library.

### Gameplay Link

(https://www.linkedin.com/posts/taha-ebaad-819b8328b_cpp-sfml-gamedevelopment-activity-7328885649569169408-QjBi?utm_source=share&utm_medium=member_desktop&rcm=ACoAAEZ5lNUBO_-anndHWlDvE3UmJfwqGNORPsk)

---

## Project Description

Xonix is a modern, interactive recreation of the classic Xonix game developed as a final project for the Programming Fundamentals course. Developed in C++ using SFML, the game features both single-player and two-player modes, multiple difficulty levels, power-ups, geometric enemy movement, scoreboards, and rich audio-visual feedback.

The objective of the game is to claim territory by capturing tiles while avoiding enemies. The project includes extensive game mechanics, menus, file handling, timers, and custom logic to deliver an engaging player experience.

---

## Features

* Modular code with separate logic blocks for game loop, input, collision, scoring, and enemy movement
* Single and two-player modes with distinct control schemes
* Menu systems: Main Menu, Level Selection, Mode Selection, Score Display
* Scoring system with dynamic multipliers and file-based score storage
* Power-ups that temporarily freeze enemies and opponents
* Difficulty levels: Easy, Medium, Hard, Continuous
* Geometric enemy movements and increasing challenge over time
* Scoreboard system displaying top scores and playtime
* Audio effects and background music integration

---

## Project Structure and Task Distribution

**Team Members:**

* Taha Ebaad https://www.linkedin.com/in/taha-ebaad-819b8328b?utm_source=share&utm_campaign=share_via&utm_content=profile&utm_medium=android_app 
* Syed M. Mesum Ali Naqvi https://www.linkedin.com/in/mesum-ali-8667a2310?utm_source=share&utm_campaign=share_via&utm_content=profile&utm_medium=android_app 

**Task Allocation:**

| Member | Tasks                                                             |
| ------ | ----------------------------------------------------------------- |
| Mesum  | All Menus, Enemy Level Setup, File-based Scoring, Scoreboard      |
| Taha   | Power-Up and Scoring Logic, 2-Player Mode, Enemy Movement, Report |

---

## Core Systems and Logic

### Menu System

* Implemented via `Menu()`, `levelMenu()`, `playerModeMenu()`
* Options for difficulty, game mode, scorecard, and exit
* End-game screen shows score, tile and move counts

### Difficulty & Enemies

* Easy: 2 Enemies
* Medium: 4 Enemies
* Hard: 6 Enemies
* Continuous: Starts from 2 then Enemies increase every 20 seconds (timer-based logic)

### Movement and Scoring

* `tileCount1` `tileCount2` and `moveCount1` `moveCount2` track player progress
* Move counted only on Successful tile capture

### Enemy Mechanism

* Speed increases every 20 seconds
* Geometric motion (Zig-Zag and Circular) after 30 seconds using timer logic
* `move()` function handles pathing and pattern changes

### Power-Ups

* Triggered at scores: First at 50, then from 70 to every 30th after i.e. 100, 130, 160...
* Freeze enemies/opponents for 3 seconds
* Visual cue: background turns magenta
* Logic managed by `PowerUpCount`, `PowerUpActive`

### Scoreboard

* Stores top 5 scores with time and player names
* Displays results post-game
* Player names with `!` e.g !Player1 indicate death during gameplay

---

## Two Player Mode

* Player 1: Arrow Keys
* Player 2: WASD
* Shared game timer

**Collision Rules:**

* Hitting trails causes instant death
* Simultaneous head-on: moving player loses
* Enemy collision with trail: game over
* Highest scorer wins if both die

---

## Audio-Visual Enhancements (Bonus)

* Magenta background during power-ups, Opposite Player and Enemies Paused for 3 seconds.
* `.wav` sound effects for tile capture, game over, and power-up activation
* Managed with `sf::SoundBuffer` and `sf::Sound`

---

## Prerequisites

* C++ Compiler (g++, MSVC, etc.)
* [SFML 3.0.0](https://www.sfml-dev.org/)

---

## How to Build & Run

### Windows (g++)

```bash
g++ main.cpp -o XONIX -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
XONIX.exe
```

### Linux (Debian-based)

```bash
sudo apt install libsfml-dev g++
g++ main.cpp -o XONIX -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio
./XONIX
```

Ensure all asset files (images, audio, fonts, etc.) are in the same directory as the executable.

---

## Project Files

* `main.cpp` - Main game logic
* `score.txt` - Scoreboard file
* `.wav`, `.jpeg`, `.png` - Game assets
* `.ttf` - Custom font file

---

## Libraries and Limitations

* **Used:** C++ standard headers, SFML (graphics, audio, window, system)
* **Avoided:** `vector`, STL containers, other restricted headers

---

## Conclusion

The XONIX project fulfills its intended goal as a feature-rich, interactive arcade game. It combines game logic, real-time graphics, audio feedback, and file handling, reinforcing our understanding of C++ fundamentals and event-driven programming.

---

## Author

* **Taha Ebaad**

> *This project was developed as part of the Programming Fundamentals final project.*
