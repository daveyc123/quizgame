This code is based on Henner Zeller's (h.zeller@acm.org) project located here:
https://github.com/Boomerific/rpi-rgb-led-matrix

It provides:
* An interface to render fonts to the display (see led-display.h)
  - A simple timer display
  - Text scrolling vertically or horizontally
  - Can switch between variable width, monospaced and pictographic fonts (without needing to re-render the fonts)
  - Can select your font for each of the category (put fonts in $PWD/fonts or /usr/local/share/fonts)
* A lower level interface that allows you to render text and draw to canvases (see text.h)
  - This would allow you for example to interleave text of different colours and fonts or sizes
* You can use both interfaces by swapping in your own canvases as desired (see LEDDisplay::SwapCanvas)

Some notes on fonts:
* Put your fonts in /usr/local/share/fonts (or $PWD/fonts or /usr/share/fonts)
* If you don't want to hunt down fonts yourself, just copy the fonts included to one of the above directories
* If you want to use your fonts with the led-display.h API, make sure to call the Set*Font() APIs

Some notes on wiring:
* Basically followed: https://learn.adafruit.com/connecting-a-16x32-rgb-led-matrix-panel-to-a-raspberry-pi/wiring-the-display
* I needed to level shift the SDA and SCL lines (OE and CLK on the panel)
  - Used a 74AHCT125
  - Powered it with 5V from the rpi
  - Connect rpi to input (A), panel to output (Y) and OE to ground
  - It still worked without level shifting, we'd just get some noticable garbage pixels
