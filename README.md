# (dis)order

![Generative waves](disorder.gif)
[Portfolio Entry](https://kysariin.github.io/project/systems/disorder)

## About
**(dis)order** is a generative art project and embedded program that visualizes Joy Division's iconic album *Unknown Pleasures*. Running on an ESP32 microcontroller, it utilizes a combination of generative, math-driven wave patterns and time-synchronized text rendering to pan the lyrics of the song "Disorder" across the screen. 

Rather than playing a pre-rendered video, the graphics are calculated upon bootup.

## Hardware
* **Microcontroller:** LilyGO ESP32 Display (with built-in TFT LCD Screen)
* **Power:** USB-C cable or compatible battery attachment

## Software & Dependencies
* **Development Environment:** PlatformIO (VS Code)
* **Core Library:** `TFT_eSPI`

## Technical Features
* **Generative Seed:** To ensure the waves are uniquely generated upon every boot rather than looping a deterministic sequence, the setup function seeds the random number generator by reading environmental static from an unconnected pin (`analogRead(0)`).
* **Double Buffering:** Drawing complex sine waves and bell curves directly to the screen frame-by-frame causes severe flickering. This project utilizes `TFT_eSprite` to reduce discomfort of watching.
* **Dynamic Scrolling:** The lyric scroll speed is not fixed. It uses linear interpolation based on the milliseconds between timestamps, automatically speeding up or slowing down in an attempt to match the vocal delivery.

## Installation & Reproducibility
To replicate this project, assuming you have the same hardware, you can just clone this repo's code
