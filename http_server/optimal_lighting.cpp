#include <ErriezBH1750.h>
#include <ErriezBH1750_regs.h>
#include <Wire.h>

// Author: Meyrem and Houda Attache
// Co-author: Ohlhoff Claude

class OptimalLighting {

  private:
    String ACTION = "NONE";
    int led_pin = 10;
    uint16_t lux;
    // ADDR line LOW/open:  I2C address 0x23 (0x46 including R/W bit) [default]
    // ADDR line HIGH:      I2C address 0x5C (0xB8 including R/W bit)
    BH1750* sensor;

  public:

    uint16_t getLightIntensity() {
        return this->lux;
    }

    void setAction(String action) {
        this->ACTION = action;
    }

    void setUp() {
        this->sensor = new BH1750(LOW);
        // Initialize I2C bus
        Wire.begin();
        pinMode(this->led_pin, OUTPUT);
    }

    void loop() {

        // Initialize sensor in continues mode, high 0.5 lx resolution
        this->sensor->begin(ModeOneTime, ResolutionHigh);

        // Start conversion
        this->sensor->startConversion();

        // Wait for completion (blocking busy-wait delay)
        if (this->sensor->waitForCompletion()) {
          
            // Read light
            this->lux = this->sensor->read();

            if (this->ACTION == "READING") {
                // map the sensor reading to a range for the LED
                analogWrite(this->led_pin, map(this->lux, 0, 4800, 255, 0));
            } else if (this->ACTION == "COOKING") {
                // map the sensor reading to a range for the LED
                analogWrite(this->led_pin, map(this->lux, 0, 1200, 255, 0));
            } else if (this->ACTION == "WORKOUT") {
                // map the sensor reading to a range for the LED
                analogWrite(this->led_pin, map(this->lux, 0, 1800, 255, 0));
            } else if (this->ACTION == "SCREENTIME") {
                // map the sensor reading to a range for the LED
                analogWrite(this->led_pin, map(this->lux, 0, 900, 255, 0));
            } else if (this->ACTION == "OFFICE_WORK") {
                // map the sensor reading to a range for the LED
                analogWrite(this->led_pin, map(this->lux, 0, 1800, 255, 0));
            } else if (this->ACTION == "CODING") {
                // map the sensor reading to a range for the LED
                analogWrite(this->led_pin, map(this->lux, 0, 3000, 255, 0));
            } else if (this->ACTION == "EATING") {
                // map the sensor reading to a range for the LED
                analogWrite(this->led_pin, map(this->lux, 0, 1800, 255, 0));
            } else if (this->ACTION == "NONE") {
                // map the sensor reading to a range for the LED
                analogWrite(this->led_pin, map(this->lux, 0, 1800, 255, 0));
            }
        }

    }

};
