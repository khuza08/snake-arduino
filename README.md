# Snake Game - Wokwi Simulator

A classic Snake game on Arduino Mega with ILI9341 TFT display and analog joystick controls.

## Hardware

- Arduino Mega 2560
- ILI9341 320x240 TFT Display
- Analog Joystick Module
- Push Button (green)
- Buzzer (passive)
<img height="512" alt="sorry for bad wiring xD" src="https://github.com/user-attachments/assets/4aa025ab-8569-4b24-88e1-befb12c38f74" />

## Pin Connections

### ILI9341 Display
| Display Pin | Arduino Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| CS | 10 |
| RST | 8 |
| D/C | 9 |
| MOSI | 51 |
| SCK | 52 |
| LED | 5V |
| MISO | 50 |

### Joystick
| Joystick Pin | Arduino Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| VERT | A1 |
| HORZ | A2 |
| SEL | 42 |

### Other Components
| Component | Arduino Pin |
|---|---|
| Button | 44 |
| Buzzer | 40 |

## Controls

- **Joystick** - Move snake (UP/DOWN/LEFT/RIGHT)
- **Button** - Start game / Restart after game over

## Features

- 32x20 game grid
- Progressive difficulty (speed increases every 5 points)
- BUZZER
- Score tracking
- Joystick input with debouncing

## Running in Wokwi

1. Go to [wokwi.com](https://wokwi.com)
2. Create new Arduino Mega project
3. Copy diagram.json as circuit
4. Copy snake_game_ili9341.ino as code
5. Click Play and enjoy!

## Notes

- Display rotated 90° for landscape mode
- Joystick axes inverted due to rotation
- Button debouncing prevents accidental triggers

## Disclaimer

This project has only been tested in **Wokwi simulator**. I don't have physical Arduino Mega hardware to test on real hardware. 
While the code should work on actual hardware, im not sure though, feel free to report any bugs.
