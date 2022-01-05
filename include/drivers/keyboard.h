#ifndef __NBSOS__DRIVERS__KEYBOARD_H
#define __NBSOS__DRIVERS__KEYBOARD_H

#include <common/types.h>
#include <hardware/interrupts.h>
#include <drivers/driver.h>
#include <hardware/port.h>

namespace NBSOS
{
    namespace Drivers
    {
        class KeyboardEventHandler
        {
            public:
                KeyboardEventHandler();

                // TODO: This should probably be a keycode enum, but a char is fine to get up and running for now.
                virtual void onKeyDown(char);
                virtual void onKeyUp(char);
        };

        class KeyboardDriver : public Hardware::InterruptHandler, public Driver
        {
            Hardware::Port8Bit dataPort;
            Hardware::Port8Bit commandPort;

            KeyboardEventHandler* handler;
            public:
                KeyboardDriver(Hardware::InterruptManager* manager, KeyboardEventHandler* handler);
                ~KeyboardDriver();
                virtual Common::uint32_t handleInterrupt(Common::uint32_t esp);
                virtual void activate();
        };
    }
}
#endif