# AI E-Ink Backpack Tag

A small wearable device with a 4-color e-paper display that shows a **new AI-generated scenery image every day**, along with the date and battery level — synced automatically over WiFi.

## How it works

```
AI image  →  Python conversion  →  Hosted online  →  ESP32-S3 fetches over WiFi  →  E-ink display
(scenery)     (resize, dither,      (GitHub /            (WiFi + HTTP)               (4-color
              4-color pack)          hosting)                                         refresh)
```

1. A scenery image (currently: manually chosen; planned: generated automatically via an image-generation API) is produced.
2. `converter/convert_image.py` resizes it to the panel's resolution and quantizes it down to the display's 4 available colors (black/white/red/yellow) using Floyd-Steinberg dithering, then packs it into the exact raw byte format the display controller expects (2 bits per pixel).
3. The packed file is hosted online (currently GitHub raw hosting).
4. The ESP32-S3 connects to WiFi, downloads the raw bytes over HTTP, and pushes them directly to the e-paper display.

## Hardware

- Seeed Studio XIAO ESP32-S3
- Waveshare 3.7" 4-color e-paper display (240×416, black/white/red/yellow)
- 800mAh Li-Po battery + TP4056 charging module
- 3D-printed / custom enclosure, keyring attachment

## Repo structure

```
firmware/     Arduino sketch + display driver libraries (.ino, .h, .cpp)
converter/    Python image-conversion pipeline (convert_image.py)
images/       Currently hosted image the device fetches
docs/         Photos/diagrams for this README
```

## Setup

1. Flash `firmware/eink_keychain.ino` to the ESP32-S3 (Arduino IDE, ESP32 board package installed).
2. Fill in your WiFi credentials and image URL at the top of the sketch.
3. To change the displayed image: run
   ```bash
   python3 converter/convert_image.py your_photo.jpg output_name
   ```
   then upload the generated `.bin` to `images/` and update the URL if it changed.

## Roadmap

- [x] Display driver bring-up (4-color e-paper working over SPI)
- [x] Python image conversion pipeline (resize, dither, 2bpp packing)
- [x] WiFi fetch of a hosted image over HTTP
- [ ] Automated daily image generation via an AI image-generation API
- [ ] On-device overlay of date + battery percentage
- [ ] Deep sleep + button controls for power efficiency

## Why I built this

The key idea of integrating embedded WiFi/HTTP with microcontoller to display image on eink display really fascinated me to build this project.This project made me learn about low-level display protocols, bit packing embedded, Wi-Fi/HTTP image processing
