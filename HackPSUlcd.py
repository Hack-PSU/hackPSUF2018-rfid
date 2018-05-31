"""
This module provides abstractions on top of the LCD library
Requires: https://gist.github.com/DenisFromHR/cc863375a6e19dce359d

"""
import I2C_LCD_Driver

MY_LCD = I2C_LCD_Driver.lcd()
DEBUG_ROW = 1
DATA_ROW = 2

def printDebug(location, wifi):
	MY_LCD.lcd_clear()
	MY_LCD.lcd_display_string("Loc: " + location + ", " + wifi + "%", DEBUG_ROW)

def printName(name):
	MY_LCD.lcd_clear()
	MY_LCD.lcd_display_string(name, DEBUG_ROW)

def printSize(size):
	MY_LCD.lcd_clear()
	MY_LCD.lcd_display_string("Size: " + size, DATA_ROW)

def printScan(goodScan):
	MY_LCD.lcd_clear()
	MY_LCD.lcd_display_string("Allow in: " + goodScan, DATA_ROW)

def printLocation(location):
	MY_LCD.lcd_clear()
	MY_LCD.lcd_display_string("Location: " + location, DATA_ROW)

def printRegistered(registered):
	MY_LCD.lcd_clear()
	MY_LCD.lcd_display_string("Registered: " + registered, DATA_ROW)
	
def printMsg(msg):
	MY_LCD.lcd_clear()
	MY_LCD.lcd_display_string(msg, DATA_ROW)


