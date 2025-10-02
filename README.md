# Framebuffer Display Lab

Experimentation project with OLED SSD1306/SSD1307 displays on embedded Linux.

## About

Started as a side project from [Ronda Linux](https://github.com/DaviVolcan/RondaLinux) after installing and activating a
128x64 OLED display with framebuffer. The goal is to implement and compare three different approaches for displaying
system information (temperature, CPU usage, etc.).

## Implementations

- [ ] **1. Static Bitmap Fonts** [🔄 In Progress]
- Fonts as C arrays
- Direct framebuffer rendering

- [ ] **2. FreeType2**
- TTF/OTF font loading
- Font flexibility
- Rendering via FreeType2

- [ ] **3. LVGL**
- Complete framework
- Ready-to-use widgets
- More elaborate interface

## Hardware

- OLED SSD1306/SSD1307 128x64 display
- Embedded Linux with an active framebuffer
- Access via `/dev/fb0`

## Status

Project in development, all implementations will be done in C.