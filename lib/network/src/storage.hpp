/* Due to the Arduino Due using Flash instead of EEPROM, it had become necessary
   to abstract various storage read/ write processes.
*/

#ifdef ARDUINO_ARCH_SAM
#include <DueFlashStorage.h>
#else
#include <EEPROM.h>
#endif

namespace hackPSU{

    class Storage{
        public:

            static void begin(size_t size);

            static void get(uint32_t address, char *buff, uint32_t size);

            static void put(uint32_t address, char *data, uint32_t dataLength);

        protected:

        private:
        #ifdef ARDUINO_ARCH_SAM
        DueFlashStorage dueFlashStorage;
        #endif
    };
}

