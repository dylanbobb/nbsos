#include "keyboard.h"

void kprintf(char*);

KeyboardDriver::KeyboardDriver(InterruptManager* manager)
: InterruptHandler(0x21, manager),
  dataPort(0x60),
  commandPort(0x64)
{
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
    static bool shift = false;
    switch (key)
    {
        case 0x45: case 0xC5: break;
        case 0xFA: break;
        case 0x02: if(shift) kprintf("!"); else kprintf("1"); break;
        case 0x03: if(shift) kprintf("@"); else kprintf("2"); break;
        case 0x04: if(shift) kprintf("#"); else kprintf("3"); break;
        case 0x05: if(shift) kprintf("$"); else kprintf("4"); break;
        case 0x06: if(shift) kprintf("%"); else kprintf("5"); break;
        case 0x07: if(shift) kprintf("^"); else kprintf("6"); break;
        case 0x08: if(shift) kprintf("&"); else kprintf("7"); break;
        case 0x09: if(shift) kprintf("*"); else kprintf("8"); break;
        case 0x0A: if(shift) kprintf("("); else kprintf("9"); break;
        case 0x0B: if(shift) kprintf(")"); else kprintf("0"); break;

        case 0x10: if(shift) kprintf("Q"); else kprintf("q"); break;
        case 0x11: if(shift) kprintf("W"); else kprintf("w"); break;
        case 0x12: if(shift) kprintf("E"); else kprintf("e"); break;
        case 0x13: if(shift) kprintf("R"); else kprintf("r"); break;
        case 0x14: if(shift) kprintf("T"); else kprintf("t"); break;
        case 0x15: if(shift) kprintf("Y"); else kprintf("y"); break;
        case 0x16: if(shift) kprintf("U"); else kprintf("u"); break;
        case 0x17: if(shift) kprintf("I"); else kprintf("i"); break;
        case 0x18: if(shift) kprintf("O"); else kprintf("o"); break;
        case 0x19: if(shift) kprintf("P"); else kprintf("p"); break;

        case 0x1E: if(shift) kprintf("A"); else kprintf("a"); break;
        case 0x1F: if(shift) kprintf("S"); else kprintf("s"); break;
        case 0x20: if(shift) kprintf("D"); else kprintf("d"); break;
        case 0x21: if(shift) kprintf("F"); else kprintf("f"); break;
        case 0x22: if(shift) kprintf("G"); else kprintf("g"); break;
        case 0x23: if(shift) kprintf("H"); else kprintf("h"); break;
        case 0x24: if(shift) kprintf("J"); else kprintf("j"); break;
        case 0x25: if(shift) kprintf("K"); else kprintf("k"); break;
        case 0x26: if(shift) kprintf("L"); else kprintf("l"); break;

        case 0x2C: if(shift) kprintf("Z"); else kprintf("z"); break;
        case 0x2D: if(shift) kprintf("X"); else kprintf("x"); break;
        case 0x2E: if(shift) kprintf("C"); else kprintf("c"); break;
        case 0x2F: if(shift) kprintf("V"); else kprintf("v"); break;
        case 0x30: if(shift) kprintf("B"); else kprintf("b"); break;
        case 0x31: if(shift) kprintf("N"); else kprintf("n"); break;
        case 0x32: if(shift) kprintf("M"); else kprintf("m"); break;
        case 0x33: if(shift) kprintf("<"); else kprintf(","); break;
        case 0x34: if(shift) kprintf(">"); else kprintf("."); break;
        case 0x35: if(shift) kprintf("?"); else kprintf("/"); break;

        case 0x1C: kprintf("\n"); break;
        case 0x39: kprintf(" "); break;
        case 0x2A: case 0x36: shift = true; break;
        case 0xAA: case 0xB6: shift = false; break;
        default:
            if (key >= 0x80)
                break;
            char* msg = "Keyboard 0x00\n";
            char* hex = "0123456789ABCDEF";
            msg[11] = hex[(key >> 4) & 0xF];
            msg[12] = hex[key & 0xF];
            kprintf(msg);
    }

    return esp;
}