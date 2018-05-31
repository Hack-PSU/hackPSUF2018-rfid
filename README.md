# RFID Round 2 The Electric Boogaloo
This repository holds the arduino compatible code for HackPSU's Fall 2018 hardware persnonel management system.

## Documentation
In an effort to maintain effective documentation, we will attempt to follow current C++ documentation standards for open-sourced projects.  There are however more than one standard, so [Doxygen](https://www.stack.nl/~dimitri/doxygen/manual/docblocks.html) like syntax is recommended.  Following Doxygen standards will enable us to publish a LaTeX or HTML like documentation guide for future developers.

## Libraries
In order to avoid licensing issues, the `~/lib/` directory has been ignored in this repository.  The folder is provided as a hint for developers to store their dependencies in there to avoid confusion as to which are missing from any given user's environment.  See [`~/libraries.md`](https://github.com/Hack-PSU/hackPSUF2018-rfid/blob/master/libraries.md) for required dependencies.

## Hardware
This repository is built on the assumption that our given hardware is being used.  Our hardware is listed below.
  * NodeMCU microcontroller
  * MFRC522 rfid module
  * Hitachi HD44780 compatible LCD with I2C backpack
  * 4x4 keypad
