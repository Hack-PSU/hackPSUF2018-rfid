# Libraries used by HackPSU's rfid device
These libraries are to be downloaded from their original creator and placed into the lib directory as necessary.  The lib directory is in our .gitignore to avoid licnesing problems

## Arduino Library
We are using the Arduino default library for NodeMCU
### NodeMCU Note
The NodeMCU board must be added to Arduino SDK for this code to compile

## RFID Library
We are using [miguelbalboa](https://github.com/miguelbalboa)'s [rfid library](https://github.com/miguelbalboa/rfid) for NXP's MFRC522.<br/>
This library can be installed by downloading the repository as a .zip file and installing it in the Arduino IDE.

## I2C LCD Library
We are using [marcoshwartz](https://github.com/marcoschwartz/)'s [LiquidCrystal_I2C](https://github.com/marcoschwartz/LiquidCrystal_I2C) for our Hitachi compatible LCD with I2C backpack.  This library can be installed by downloading the repository as a .zip file and installing it in the Arduino IDE.
