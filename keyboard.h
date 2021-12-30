#ifndef __KEYBOARD_H
#define __KEYBOARD_H

    #include "types.h"
    #include "interrupts.h"
    #include "driver.h"
    #include "port.h"

    class KeyboardEventHandler
    {
        KeyboardEventHandler();

        virtual void onKeyDown();
        virtual void onKeyUp();
    };

    class KeyboardDriver : public InterruptHandler, public Driver
    {
        Port8Bit dataPort;
        Port8Bit commandPort;

        KeyboardEventHandler* handler;
        public:
            KeyboardDriver(InterruptManager* manager, KeyboardEventHandler* handler);
            ~KeyboardDriver();
            virtual uint32_t handleInterrupt(uint32_t esp);
            virtual void activate();
    };
#endif