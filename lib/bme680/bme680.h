#include <Adafruit_Sensor.h>
#include "Zanshin_BME680.h"   ///< The BME680 sensor library

class BME680Wrapper{
    private:
        BME680_Class BME680;
        int32_t temp;
        int32_t humidity;
        int32_t gas;
        int32_t pressure;

    public:
        BME680Wrapper();
        void initBME680();
        bool readBME680data();
        int32_t getTempData(){return temp;};
        int32_t getHumidityData(){return humidity;};
        int32_t getGasData(){return gas;};
        int32_t getPressureData(){return pressure;};
};