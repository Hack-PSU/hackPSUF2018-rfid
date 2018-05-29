#pragma once

#include <MFRC522.h>

String getUID();
void setKey(byte* key);
String readData();
void writeData(String data);
