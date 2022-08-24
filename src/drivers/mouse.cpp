#include <drivers/mouse.h>

using namespace NBSOS::Common;
using namespace NBSOS::Drivers;
using namespace NBSOS::Hardware;

void kprintf(char*);

MouseEventHandler::MouseEventHandler()
{
}

void MouseEventHandler::onMouseMove(int xOffset, int yOffset)
{
}

void MouseEventHandler::onMouseDown(uint8_t button)
{
}

void MouseEventHandler::onMouseUp(uint8_t button)
{
}

void MouseEventHandler::onActivate()
{
}

MouseDriver::MouseDriver(InterruptManager* manager, MouseEventHandler* handler)
: InterruptHandler(0x2C, manager),
  dataPort(0x60),
  commandPort(0x64)
{
    this->handler = handler;
}

void MouseDriver::activate()
{
    offset = 0;
    buttons = 0;

    handler->onActivate();

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

    buffer[offset] = dataPort.read();

    if (handler == 0)
    {
        return esp;
    }
    
    offset = (offset + 1) % 3;

    if (offset == 0)
    {
        if (buffer[1] != 0 || buffer[2] != 0)
        {
            handler->onMouseMove((int8_t)buffer[1],-((int8_t)buffer[2]));
        }
        for (uint8_t i = 0; i < 3; i++)
        {
            if ((buffer[0] & (0x1 << i)) != (buttons & (0x1 << i)))
            {
                if (buttons & (0x1 << i))
                    handler->onMouseUp(i + 1);
                else
                    handler->onMouseDown(i + 1);
            }
        }
        buttons = buffer[0];
    }

    return esp;
}