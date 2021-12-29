#ifndef __PORT_H
#define __PORT_H
    #include "types.h"

    class Port
    {
        protected:
            uint16_t portNumber;
            Port(uint16_t portNumber);
            ~Port();
    };

    class Port8Bit : public Port
    {
        public:
            Port8Bit(uint16_t portNumber);
            ~Port8Bit();
            virtual uint8_t read();
            virtual void write(uint8_t data);
    };

    class Port8BitSlow : public Port8Bit
    {
        public:
            Port8BitSlow(uint16_t portNumber);
            ~Port8BitSlow();
            virtual void write(uint8_t data);
    };

    class Port16Bit : public Port
    {
        public:
            Port16Bit(uint16_t portNumber);
            ~Port16Bit();
            virtual uint16_t read();
            virtual void write(uint16_t data);
    };

    class Port32Bit : public Port
    {
        public:
            Port32Bit(uint16_t portNumber);
            ~Port32Bit();
            virtual uint32_t read();
            virtual void write(uint32_t data);
    };
#endif