#include "keyboard.h"

void kprintf(char*);
void kprintHex(uint8_t);


KeyboardEventHandler::KeyboardEventHandler()
{
}

void KeyboardEventHandler::onKeyDown(char)
{
}

void KeyboardEventHandler::onKeyUp(char)
{
}

KeyboardDriver::KeyboardDriver(InterruptManager* manager, KeyboardEventHandler* handler)
: InterruptHandler(0x21, manager),
  dataPort(0x60),
  commandPort(0x64)
{
    this->handler = handler;
}

void KeyboardDriver::activate()
{
    while(commandPort.read() & 0x1)
        dataPort.read(); // clear out the buffer
    commandPort.write(0xAE); // interrupts
    commandPort.write(0x20); // get current state
    uint8_t status = (dataPort.read() | 1) & ~0x10;
    commandPort.write(0x60); // set state
    dataPort.write(status);
    dataPort.write(0xF4); // enable keyboard
}

KeyboardDriver::~KeyboardDriver()
{
}

uint32_t KeyboardDriver::handleInterrupt(uint32_t esp)
{
    uint8_t key = dataPort.read();

    if (handler == 0)
        return esp;
    

    // TODO: Re-add shift support to keyhandler later.
    switch (key)
    {
        case 0x45: case 0xC5: break;
        case 0xFA: break;
        case 0x02: handler->onKeyDown('1'); break;
        case 0x03: handler->onKeyDown('2'); break;
        case 0x04: handler->onKeyDown('3'); break;
        case 0x05: handler->onKeyDown('4'); break;
        case 0x06: handler->onKeyDown('5'); break;
        case 0x07: handler->onKeyDown('6'); break;
        case 0x08: handler->onKeyDown('7'); break;
        case 0x09: handler->onKeyDown('8'); break;
        case 0x0A: handler->onKeyDown('9'); break;
        case 0x0B: handler->onKeyDown('0'); break;

        case 0x10: handler->onKeyDown('q'); break;
        case 0x11: handler->onKeyDown('w'); break;
        case 0x12: handler->onKeyDown('e'); break;
        case 0x13: handler->onKeyDown('r'); break;
        case 0x14: handler->onKeyDown('t'); break;
        case 0x15: handler->onKeyDown('y'); break;
        case 0x16: handler->onKeyDown('u'); break;
        case 0x17: handler->onKeyDown('i'); break;
        case 0x18: handler->onKeyDown('o'); break;
        case 0x19: handler->onKeyDown('p'); break;

        case 0x1E: handler->onKeyDown('a'); break;
        case 0x1F: handler->onKeyDown('s'); break;
        case 0x20: handler->onKeyDown('d'); break;
        case 0x21: handler->onKeyDown('f'); break;
        case 0x22: handler->onKeyDown('g'); break;
        case 0x23: handler->onKeyDown('h'); break;
        case 0x24: handler->onKeyDown('j'); break;
        case 0x25: handler->onKeyDown('k'); break;
        case 0x26: handler->onKeyDown('l'); break;

        case 0x2C: handler->onKeyDown('z'); break;
        case 0x2D: handler->onKeyDown('x'); break;
        case 0x2E: handler->onKeyDown('c'); break;
        case 0x2F: handler->onKeyDown('v'); break;
        case 0x30: handler->onKeyDown('b'); break;
        case 0x31: handler->onKeyDown('n'); break;
        case 0x32: handler->onKeyDown('m'); break;
        case 0x33: handler->onKeyDown(','); break;
        case 0x34: handler->onKeyDown('.'); break;
        case 0x35: handler->onKeyDown('/'); break;

        case 0x1C: handler->onKeyDown('\n'); break;
        case 0x39: handler->onKeyDown(' '); break;
        // case 0x2A: case 0x36: shift = true; break;
        // case 0xAA: case 0xB6: shift = false; break;
        default:
            if (key >= 0x80)
                break;
            kprintf("Keyboard 0x");
            kprintHex(key);
            kprintf("\n");
            break;
    }

    return esp;
}