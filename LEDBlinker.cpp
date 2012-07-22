#include "LEDBlinker.h"

/* LEDBlinker constructor */
LEDBlinker::LEDBlinker() {
    /* Set up the LED flasher (see http://wiki.maemo.org/LED_patterns) */
    FILE *f = fopen("/sys/devices/platform/i2c_omap.2/i2c-2/2-0032/engine3_mode", "w");
    if (!f) return;
    fwrite("load\n", 5, 1, f);
    fclose(f);
    f = fopen("/sys/devices/platform/i2c_omap.2/i2c-2/2-0032/engine3_leds", "w");
    if (!f) return;
    fwrite("000011100\n", 10, 1, f);
    fclose(f);
    f = fopen("/sys/devices/platform/i2c_omap.2/i2c-2/2-0032/engine3_load", "w");
    if (!f) return;
    fwrite("9d80" // refresh engine
           "40ff" // set max brightness
           "1100" // pause for a little bit
           "4000" // set min brightness
           "c000" // terminate pattern
           "c000"
           "c000"
           "c000\n",
           33, 1, f);
    fclose(f);
}

LEDBlinker::~LEDBlinker() {
    FILE *f = fopen("/sys/devices/platform/i2c_omap.2/i2c-2/2-0032/engine3_mode", "w");
    if (!f) return;
    fwrite("disabled\n", 9, 1, f);
    fclose(f);        
}

void LEDBlinker::blink() {
    // Flash the device LED
    FILE *f = fopen("/sys/devices/platform/i2c_omap.2/i2c-2/2-0032/engine3_mode", "w");
    if (!f) return;
    fwrite("run\n", 4, 1, f);
    fclose(f);
}

LEDBlinker::BlinkAction::BlinkAction(LEDBlinker * a) {
    blinker = a;
    time = 0;
    latency = a ? a->getLatency() : 0;
}

LEDBlinker::BlinkAction::BlinkAction(LEDBlinker * a, int t) {
    blinker = a;
    time = t;
    latency = a ? a->getLatency() : 0;
}



