#ifndef __NBSOS__HARDWARE__PORT_H
#define __NBSOS__HARDWARE__PORT_H

#include <common/types.h>

namespace NBSOS
{
    namespace Hardware
    {
        class Port
        {
            protected:
                Common::uint16_t portNumber;
                Port(Common::uint16_t portNumber);
                ~Port();
        };

        class Port8Bit : public Port
        {
            public:
                Port8Bit(Common::uint16_t portNumber);
                ~Port8Bit();
                virtual Common::uint8_t read();
                virtual void write(Common::uint8_t data);
        };

        class Port8BitSlow : public Port8Bit
        {
            public:
                Port8BitSlow(Common::uint16_t portNumber);
                ~Port8BitSlow();
                virtual void write(Common::uint8_t data);
        };

        class Port16Bit : public Port
        {
            public:
                Port16Bit(Common::uint16_t portNumber);
                ~Port16Bit();
                virtual Common::uint16_t read();
                virtual void write(Common::uint16_t data);
        };

        class Port32Bit : public Port
        {
            public:
                Port32Bit(Common::uint16_t portNumber);
                ~Port32Bit();
                virtual Common::uint32_t read();
                virtual void write(Common::uint32_t data);
        };
    }
}
#endif