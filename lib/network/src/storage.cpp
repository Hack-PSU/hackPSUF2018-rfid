#include "storage.hpp"

#ifdef ARDUINO_ARCH_SAM
#include <DueFlashStorage.h>

namespace hackPSU
{
    void Storage::begin(size_t size)
    {
        return;
    }

    void Storage::get(uint32_t address, char *buff, uint32_t size)
    {
        for (i = 0; i < size; i ++)
        {
            memset(buff[i], dueFlashStorage.read(i + address), 1);
        }
        return;
    }

    void Storage::put(uint32_t address, char *data, uint32_t dataLength)
    {
        return dueFlashStorage.write(address, data, dataLength);
    }
}

#else
#include <EEPROM.h>

namespace hackPSU {

    void Storage::begin(size_t size) {
        EEPROM.begin(size);
        return;
    }

    void Storage::get(uint32_t address, char *buff, uint32_t size) {
        EEPROM.get(0, buff);
        return;
    }

    void Storage::put(uint32_t address, char *data, uint32_t dataLength) {
        EEPROM.put(address, data);
        EEPROM.commit();
        return;
    }
}

#endif