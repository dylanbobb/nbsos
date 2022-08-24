#ifndef __NBSOS__DRIVERS__VGA_H
#define __NBSOS__DRIVERS__VGA_H

#include <common/types.h>
#include <drivers/driver.h>
#include <hardware/port.h>

namespace NBSOS
{
    namespace Drivers
    {
        class VGA
        {
            protected:
                Hardware::Port8Bit miscPort;
                Hardware::Port8Bit cathodeIndexPort;
                Hardware::Port8Bit cathodeDataPort;
                Hardware::Port8Bit sequencerIndexPort;
                Hardware::Port8Bit sequencerDataPort;
                Hardware::Port8Bit graphicsControllerIndexPort;
                Hardware::Port8Bit graphicsControllerDataPort;
                Hardware::Port8Bit attributeControllerIndexPort;
                Hardware::Port8Bit attributeControllerReadPort;
                Hardware::Port8Bit attributeControllerWritePort;
                Hardware::Port8Bit attributeControllerResetPort;

                void writeRegisters(Common::uint8_t* registers);
                Common::uint8_t* getFrameBufferSegment();
                virtual Common::uint8_t getColorIndex(Common::uint8_t r, Common::uint8_t g, Common::uint8_t b);
            public:
                VGA();
                ~VGA();

                virtual bool supportsMode(Common::uint32_t width, Common::uint32_t height, Common::uint32_t colorDepth);
                virtual bool setMode(Common::uint32_t width, Common::uint32_t height, Common::uint32_t colorDepth);
                virtual void putPixel(Common::int32_t x, Common::int32_t y, Common::uint8_t r, Common::uint8_t g, Common::uint8_t b);
                virtual void putPixel(Common::int32_t x, Common::int32_t y, Common::uint8_t colorIndex);
                virtual void fillRectangle(Common::uint32_t x, Common::uint32_t y, Common::uint32_t w, Common::uint32_t h, Common::uint8_t r, Common::uint8_t g, Common::uint8_t b);
        };
    }
}

#endif