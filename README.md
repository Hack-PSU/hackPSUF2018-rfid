# RFID Round 2 The Electric Boogaloo
This repository hosts the Arduino code used by the NodeMCU RFID Wristband scanner.  Code found in this repository is dependent on code from other GitHub repositories.  Please see the docstring at the top of each Arduino file for a complete list of requirements for each module.  This repository also assumes that you have access to a server compatible with the HackPSU Redis Cache Server, which will be discussed later.

* [Provided Software](#provided-software)
  * [driver.py](#driverpy)
  * [writer.py](#writerpy)
  * [test_programs/configwriter.py](#test_programsconfigwriterpy)
  * [test_programs/configreader.py](#test_programsconfigreaderpy)
* [Intended Use Case](#intended-use-case)
* [Hardware Setup](#hardware-setup)
* [Python Modules](#python-modules)
  * [HackPSUconfig](#hackpsuconfig)
  * [HackPSUfauxlcd](#hackpsufauxlcd)
  * [HackPSUlcd](#hackpsulcd)
  * [HackPSUredis](#hackpsuredis)
  * [HackPSUrfid](#hackpsurfid)

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


## Provided Software
This repository provides four executable scripts as well as several Python modules for use with the RaspberryPi.  

### driver.py
This is the main program for the wristband scanner.  This program has four states described in the script's docstring.  The primary state for this application is the scanning state, which posts scans to the server as given by the HackPSUredis module.  Scans are identified by a wristband UID, timestamp, and location.  The location is known by a global variable, which can be configured in two ways.  The location can be set either by giving the program a configuration file with the name `pi.cfg` and adding the line `location=?` where `?` is the desired location.  Locations will be passed to the scan receiver as a string over HTTP, so they may be anything so long as your server knows how to handle the data.  The second, and prefered method, to change location of the scanner is by using the scanning state.  The scanning state will scan a wristband and read sector 8 of the wristband's user data.  This sector is assumed to be 16 (8-bit) bytes in length and will be converted to an ASCII string before being stored as the scanner's location.  The third state for the location is the registration state.  This state reads a 3-digit pin from the keypad and displays the name assigned to that user through a query to the HackPSUredis module.  If the user's name is correct, a wristband is then scanned and the pin and wristband are associated by the server.

### writer.py
This is a helper program that is to be run pre-event and loads data on to wristbands for use as location wristbands.  Unlike, the drive.py program, this requires terminal access.  This program will prompt the user for a string of length 16 or less and write that to sector 8 of the next scanned wristband as an ASCII string with no null terminator.

### test_programs/configWriter.py
This is a helper program that is intended to make creating configuration files for the driver program easier.  It requires the HackPSUconfig module from the root directory of this repository.  The program will prompt the user for key, value pairs via the terminal and will create a config file with those values.  An example configuration file is provided below.
```
location=Penn State
redisLocation=127.0.0.0
#The next line doesn't matter; driver.py doesn't check this property
apiKey=None
```

### test_programs/readConfig.py
This is a helper program that is intended to aid in validating configuration files.  It reads in a configuration file and displays the contents in a JSON like syntax without any comments.

## Intended Use Case
This repository provides code that is intended for use as an entrance system for a hackathon.  The HackPSU team will be using the following procedure.  
1. Before the hackathon use writer.py to configure one wristband for each location that will be available.  Each wristband will be marked with its location and set aside so that they are not given to eventgoers.
2. During registraiton user driver.py in registration mode to register hackers with our server and track who came to the event.  Each hacker will be given a wristband, which will serve as their key into events.
3. Organizers will be placed with a scanner at each tracked event and will scan a location wristband to set the scanner location
4. Organizers will make sure that hackers scan their wristbands at each event and the scanner will display a `Y/N` for whether or not they are to be allowed in.
5. Orgainizers will checck the HackPSU redis server to make sure that all scans are posted to the primary server correctly

## Hardware Setup
Coming soon.

## Python Modules
There are 5 modules provided for abstractoions; while each module should be documented in detail in its own docstring, an overview of each module is provided below.

### HackPSUconfig
Abstractions for converting the driver configration file to and from a Python dictionary.  This module also provides methods for using the configuration file as a dictionary directly, avoiding storing the dictionary in memory.

### HackPSUfauxlcd
A faux lcd module, which behaves identically to the HackPSUlcd module, except that it printts to the terminal rather than to the LCD.

### HackPSUlcd
Abstractions over print statements for the LCD.  Each method prints some amount of data to part of the screen.

### HackPSUredis
Abstractions over API calls to our Redis Cache Server.  For other hackathons using our code.  This module may need to be redone to use different routes.  Our routes can be seen in the module docstring

### HackPSUrfid
Abstractions over the MFRC522 library for MiFare wristbands.  
