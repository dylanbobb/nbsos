#ifndef __NBSOS__DRIVERS__MOUSE_H
#define __NBSOS__DRIVERS__MOUSE_H

#include <common/types.h>
#include <hardware/interrupts.h>
#include <drivers/driver.h>
#include <hardware/port.h>

namespace NBSOS
{
    namespace Drivers
    {
        class MouseEventHandler
        {
            public:
                MouseEventHandler();

                virtual void onActivate();
                virtual void onMouseMove(int xOffset, int yOffset);
                virtual void onMouseDown(Common::uint8_t button);
                virtual void onMouseUp(Common::uint8_t button);
        };

        class MouseDriver : public Hardware::InterruptHandler, public Driver
        {
            Hardware::Port8Bit dataPort;
            Hardware::Port8Bit commandPort;
            Common::uint8_t buffer[3];
            Common::uint8_t offset;
            Common::uint8_t buttons;

            MouseEventHandler* handler;
            
            public:
                MouseDriver(Hardware::InterruptManager* manager, MouseEventHandler* handler);
                ~MouseDriver();
                virtual Common::uint32_t handleInterrupt(Common::uint32_t esp);
                virtual void activate();
        };
    }
}
#endif