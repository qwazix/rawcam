#ifndef LED_BLINKER_H
#define LED_BLINKER_H
#include <string>

#include <FCam/FCam.h>
#include <FCam/Action.h>
#include <FCam/Device.h>

/* An FCam Device to flash one of the LEDs on the front of the device at capture time. */
class LEDBlinker : public FCam::Device {
    
public:
        
    LEDBlinker();
    ~LEDBlinker();

    /*
     * An action representing the blinking of the front LED
     */
    class BlinkAction : public FCam::CopyableAction<BlinkAction> {
    public:

        /* Constructors and destructor */
        BlinkAction(LEDBlinker * b);
        BlinkAction(LEDBlinker * b, int time);

        /* Implementation of doAction() as required */
        void doAction() {blinker->blink();}
        
    protected:
        LEDBlinker *blinker;
        int size;
    };
    
    /* Normally, this is where a device would add metadata tags to a
     * just-created frame , based on the timestamps in the
     * Frame. However, we don't have anything useful to add here, so
     * tagFrame does nothing. */
    void tagFrame(FCam::Frame) {}
    
    /* Flashes the LED */
    void blink();
    
    /* Returns latency in microseconds. Zero is a reasonable approximate for this device. */
    int getLatency() {return 0;}
};

#endif
