#include "mouse.h"

void kprintf(char*);

MouseDriver::MouseDriver(InterruptManager* manager)
: InterruptHandler(0x2C, manager),
  dataPort(0x60),
  commandPort(0x64)
{
    offset = 0;
    buttons = 0;

    static uint16_t* VMem = (uint16_t*)0xb8000;

    VMem[80*12+40] = ((VMem[80*12+40] & 0xF000) >> 4) |
                    ((VMem[80*12+40] & 0x0F00) << 4) |
                    ((VMem[80*12+40] & 0x00FF));

    commandPort.write(0xAB); // interrupts
    commandPort.write(0x20); // get current state
    uint8_t status = (dataPort.read() | 2);
    commandPort.write(0x60); // set state
    dataPort.write(status);
    commandPort.write(0xD4);
    dataPort.write(0xF4); // enable mouse
    dataPort.read();
}

MouseDriver::~MouseDriver()
{
}

uint32_t MouseDriver::handleInterrupt(uint32_t esp)
{
    uint8_t status = commandPort.read();
    if (!(status & 0x20))
        return esp;


    static int8_t x=40,y=12;

    buffer[offset] = dataPort.read();
    offset = (offset + 1) % 3;

    if (offset == 0)
    {
        static uint16_t* VMem = (uint16_t*)0xb8000;

        VMem[80*y+x] = ((VMem[80*y+x] & 0xF000) >> 4) |
                       ((VMem[80*y+x] & 0x0F00) << 4) |
                       ((VMem[80*y+x] & 0x00FF));

        x += buffer[1];
        if (x < 0) x = 0;
        if (x >= 80) x = 79;

        y -= buffer[2];
        if (y < 0) y = 0;
        if (y >= 25) y = 24;

        VMem[80*y+x] = ((VMem[80*y+x] & 0xF000) >> 4) |
                       ((VMem[80*y+x] & 0x0F00) << 4) |
                       ((VMem[80*y+x] & 0x00FF));
    }

    return esp;
}