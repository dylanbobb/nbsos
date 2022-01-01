#ifndef __KEYBOARD_H
#define __KEYBOARD_H

    #include "types.h"
    #include "interrupts.h"
    #include "driver.h"
    #include "port.h"

    class KeyboardEventHandler
    {
        public:
            KeyboardEventHandler();

            // TODO: This should probably be a keycode enum, but a char is fine to get up and running for now.
            virtual void onKeyDown(char);
            virtual void onKeyUp(char);
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