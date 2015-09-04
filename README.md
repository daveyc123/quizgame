# Quizgame

## Overview

Quizgame is Raspberry-Pi based application that presents several two-answer questions to the player. The time in which they complete the questions is their resulting score. A time penalty is applied for any questions that they answer incorrectly. The main game interface is shown over HDMI, combined with an led matrix panel highlighting important information such as the timer. Input is provided through two arcade buttons.

I designed this game to raise money at my brother-in-law's stag. The game presented questions in which the answer was either him or my soon to be sister-in-law.

# [Watch the game in action on YouTube!](https://www.youtube.com/watch?v=bzoEmVioPfQ)

![Quiz Game] (quizgame.jpg)

## Code

The main game (quizgameqt) is implemented using a mix of Qt C++ and Qml. The interface to the led panel is implemented in librgbmatrix. rpi-led-text is a simple program that can be used to test the behaviour of libgrbmatrix.

#### quizgameqt

The quizgameqt folder contains everyhing to build the main game, including the QtCreator project files. The UI for the game is implemented in QML and can be found in the project resources. The game logic is implemented in Qt C++. One of the advantages of having the game implemented in Qt is that the majority of the code can be built and run on the host platform you are developing on. This considerably speeds up the development cycle. In order to disable the portions of code that require PI specific code (led interface, button interface), simply remove the #define BUILD_FOR_PI in main.cpp.

#### librgbmatrix

librgbmatrix is a C++ library for interfacing with an [led matrix panel](http://www.adafruit.com/product/420) connected to a Raspberry Pi. More details available under [librgbmatrix](librgbmatrix/README.md). Example usage of the library can be found in quizgameqt/qquizleddisplay.cpp and in the rpi-led-text sample.

#### rpi-led-text

rpi-led-text is a sample program that exercises the librgbmatrix library. Like librgbmatrix, it's adapted from the code available at https://github.com/Boomerific/rpi-rgb-led-matrix

The sample program has several demos.
* ./led-matrix 0 -> Runs through the main APIs of librgbmatrix
* ./led-matrix 1 -> Example showing drawing on a canvas (in this case, flashes the text "Winning!"
* ./led-matrix 2 -> Loops through the characters of a pictogram font
* ./led-matrix 3 -> Clears the led

#### Dev environment

All the code was developed on OSX. For the Qt code, QtCreator 3.1.2 was used. The code targets Qt version 5.2

#### Configuring the PI

All these instructions were executed in August of 2014. YMMV. We were using a Model B (rev 2) board.

##### Setting up the OS
I ended up using the noobs setup guide to get an SD card flashed with raspbian os
* See http://www.raspberrypi.org/help/noobs-setup/
* Used sdformatter 4.0 (osx) to format the SD card
* During the setup selected the raspbian os
* After setup, used the configuration tool that comes up to enable ssh
* Installed vim (sudo apt-get install vim)
* updated my keyboard layout so that the pipe character worked!
  * sudo vi /etc/default/keyboard
  * switch XKBLAYOUT=”gb” to XKBLAYOUT=”us”

#### Installing Qt

You have a few options for getting Qt running on your PI. Building from source is one option. At the time, I didn't find any great cross-compiling instructions (although it seems like this new [guide](http://qt-project.org/wiki/RaspberryPi_Beginners_guide) would have been handy). Fortunately there are also some prebuilt packages published at twolife.be

* On your PI, add the twolife.be repo
  * deb http://twolife.be/raspbian/ wheezy main backports
  * deb-src http://twolife.be/raspbian/ wheezy main backports
	 * sudo apt-key adv --keyserver keyserver.ubuntu.com --recv-key 2578B775
	 * sudo apt-get update
	 * sudo apt-get upgrade
* Install Qt
  * sudo apt-get install qtdeclarative5-dev libqt5webkit5 libgles2-mesa-dev qtdeclarative5-qtquick2-plugin qtdeclarative5-quicklayouts-plugin qtdeclarative5-controls-plugin qtdeclarative5-window-plugin
* Verify the installation with a sample Qt app
  * git clone https://git.gitorious.org/qt5-raspberrypi-example/qt5-raspberrypi-example.git
  * make
  * qmake
  * ./helloworld -platform eglfs (the eglfs gets around the windowing problems)

## Construction
#### Parts

In order to build the game, you'll need the following parts:
* (2) [Medium 16x32 RGB LED matrix panel](http://www.adafruit.com/products/420)
* (1) LCD monitor with HDMI input (or DVI/VGA with HDMI to DVI/VGA adapter)
* (1) [Half size Breadboard](http://www.adafruit.com/products/64)
* (1) [5V 4A (4000mA) switching power supply](http://www.adafruit.com/products/1466)
* (1) [Female DC Power adapter - 2.1mm jack to screw terminal block](http://www.adafruit.com/products/368)
* (2) [Large Arcade Button with LED - 60mm Red/Green](http://www.adafruit.com/products/1190)
* (1) [Raspberry Pi Model B](http://www.adafruit.com/products/998)
* (1) [74AHCT125 - Quad Level-Shifter (3V to 5V)](http://www.adafruit.com/products/1787)
* Female/Female and Male/Male Jumper wires

#### Building the cabinet

The cabinet was built using two 4'x8' sheets of 1/2" MDF. The pictures below show the different pieces that were cut out to make the cabinet. It was assembled using wood glue and a brad nailer. Once assembled, I did one quick sand to smooth out the cut edges and to round out the corners. I then applied Bondo (which works great on MDF) to all of the joints and imperfections, after which I did another sanding. A coat of primer was applied and then several coats of black paint. Finally all of the hardware was installed. The arcade buttons have a screw mechanism which makes them easy to install. For the LED panels, I used machine screws through a panel mounted inside the back of the cabinet. For the LCD monitor, I also used a panel on the inside of the cabinet and some machine screws which were tied into the VESA mounting screws of the LCD. For the Raspberry PI, I purchased some hex spacers and machine screws which allowed me to mount the PI on the inside of the cabinet.

## Configuring the game

#### Building the game
* make
* qmake

#### Setting the questions
The questions are stored in json format under questions.json

#### Resetting the high scores
The highscores are stored in json format under highscores.json

#### Starting the game
./qtquizgame -platform eglfs -r <path to high scores json> -q <path to questions json>







