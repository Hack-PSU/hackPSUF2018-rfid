"""
This module provides an abstraction on top of the MFRC522 library
That library can be found at https://github.com/mxgxw/MFRC522-python

Methods:
	detectBand()
		Get whether or not a band is on the scanner
	getUID()
		Get a wristband's UID as a string
	readLocation()
		Get the location written to the first sectors of the wristband
	writeLocation(location : str)
		Write the given location to the current wristband
"""
import sys
import io
import json
import MFRC522

reader = MFRC522.MFRC522()
locationSector = 8
key = [0xFF,0xFF,0xFF,0xFF,0xFF,0xFF]

def detectBand():
	"""
	bool detectBand ( void )
	
	Detect the first available rfid tag and get the reader status
	This is a nonblocking call
	
	Returns:
		bandPresent: True for a band present, or False for no band present
	
	"""
	(status, TagType) = reader.MFRC522_Request(reader.PICC_REQIDL)
	if status == reader.MI_OK:
		return True
	return False

def getUID():
	"""
	str getUID ( void )
	
	Detect the first available rfid tag and get the UID
	This is nonblocking call
	
	Returns:
		value: A UID string formatted like "?,?,?,?"
	
	Raises:
		ValueError: This will be raised iff no wristband is available
	"""
	status = None
	while not (status == reader.MI_OK):
		(status,TagType) = reader.MFRC522_Request(reader.PICC_REQIDL)
	(status, uid) = reader.MFRC522_Anticoll()
	while not status == reader.MI_OK:
		(status, uid) = reader.MFRC522_Anticoll()
	value = "{uid1},{uid2},{uid3},{uid4}".format(uid1=uid[0], uid2=uid[1], uid3=uid[2], uid4=uid[3])
	return value

def readLocation():
	"""
	str readLocation ( void )
	
	Detect the first available rfid tag and get the data from the locationSector
	
	Returns:
		strOut: A string from the data back from the tag
		
	Raises:
		ValueError: This will be raised if no wristband is available or the wrong amount of data is returned
	"""
	status = None
	while not (status == reader.MI_OK):
		(status,TagType) = reader.MFRC522_Request(reader.PICC_REQIDL)
	(status, uid) = reader.MFRC522_Anticoll()
	if not (status == reader.MI_OK):
		raise ValueError('No rfid tag detected')
	reader.MFRC522_SelectTag(uid)
	reader.MFRC522_Auth(reader.PICC_AUTHENT1A, 8, key, uid)
	tmpOut = sys.stdout
	sys.stdout = io.BytesIO()
	reader.MFRC522_Read(locationSector)
	output = sys.stdout.getvalue()
	sys.stdout = tmpOut
	reader.MFRC522_StopCrypto1()
	arrLoc = output.index("[")
	output = output[arrLoc:]
	obj = json.loads(output)
	strOut = "".join(chr(i) for i in obj)
	return strOut

def writeLocation(location):
	"""
	void writeLocation ( location:str )
	
	Detect the first available rfid tag and write the given location to the locationSector
	
	Args:
		location: The location to write to the wristband
		
	Raises:
		ValueError if there is no wristband available or the location is longer than 16 chars in length
	"""
	status = None
	while not (status == reader.MI_OK):
		(status,TagType) = reader.MFRC522_Request(reader.PICC_REQIDL)
	(status, uid) = reader.MFRC522_Anticoll()
	if not (status == reader.MI_OK):
		raise ValueError('No rfid tag detected')
	reader.MFRC522_SelectTag(uid)
	reader.MFRC522_Auth(reader.PICC_AUTHENT1A, 8, key, uid)
	if len(location) > 16:
		raise ValueError('Location string is too long')
	buffer = [ord(c) for c in location]
	while len(buffer) < 16:
		buffer.append(0)
	tmp_out = sys.stdout
	sys.stdout = io.BytesIO()
	reader.MFRC522_Write(locationSector, buffer)
	sys.stdout = tmp_out
	reader.MFRC522_StopCrypto1()

