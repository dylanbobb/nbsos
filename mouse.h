#ifndef __MOUSE_H
#define __MOUSE_H

    #include "types.h"
    #include "interrupts.h"
    #include "driver.h"
    #include "port.h"

    class MouseEventHandler
    {
        public:
            MouseEventHandler();

            virtual void onActivate();
            virtual void onMouseMove(int xOffset, int yOffset);
            virtual void onMouseDown(uint8_t button);
            virtual void onMouseUp(uint8_t button);
    };

    class MouseDriver : public InterruptHandler, public Driver
    {
        Port8Bit dataPort;
        Port8Bit commandPort;
        uint8_t buffer[3];
        uint8_t offset;
        uint8_t buttons;

        MouseEventHandler* handler;
        
        public:
            MouseDriver(InterruptManager* manager, MouseEventHandler* handler);
            ~MouseDriver();
            virtual uint32_t handleInterrupt(uint32_t esp);
            virtual void activate();
    };
#endif