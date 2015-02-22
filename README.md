# Quizgame

## Overview

Quizgame is Raspberry-Pi based application that presents several two-answer questions to the player. The time in which they complete the questions is their resulting score. A time penalty is applied for any questions that they answer incorrectly. The main game interface is shown over HDMI, combined with an led matrix panel highlighting important information such as the timer. Input is provided through two arcade buttons.

I designed this game to raise money at my brother-in-law's stag. The game presented questions in which the answer was either him or my soon to be sister-in-law.

![Quiz Game] (quizgame.jpg)

* [Code](#code)
  * [quizgameqt](#quizgameqt)
  * [librgbmatrix](#librgbmatrix)
  * [rpi-led-text](#rpi-led-text)
  * [Dev environment](#dev-environment)
  * [Configuring the PI](#configuring-the-pi)
* [Construction](#construction)
  * [Parts](#parts)
  * [Hardware assembly](#hardware-assembly)
  * [Building the cabinet](#building-the-cabinet)
* [Configuring the game](#configuring-the-game)

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

All the code was developed on OSX. For the Qt code, QtCreator ?.? was used. The code targets Qt version 5.?

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
#### Hardware assembly
#### Building the cabinet
## Configuring the game







