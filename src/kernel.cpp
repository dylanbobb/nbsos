#include <common/types.h>
#include <gdt.h>
#include <hardware/interrupts.h>
#include <hardware/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>

using namespace NBSOS;
using namespace NBSOS::Common;
using namespace NBSOS::Drivers;
using namespace NBSOS::Hardware;

void kprintf(char* str)
{
    static uint16_t* VMem = (uint16_t*)0xb8000;

    static uint8_t x = 0;
    static uint8_t y = 0;

    for (int32_t i = 0; str[i] != '\0'; i++)
    {
        switch(str[i])
        {
            case '\n':
                x = 0;
                y++;
                break;
            default:
                VMem[80*y+x] = (VMem[80*y+x] & 0xFF00) | str[i];
                x++;
                break;
        }
        if (x >= 80)
        {
            x = 0;
            y++;
        }
        if (y >= 25)
        {
            for (y = 0; y < 25; y++)
                for (x = 0; x < 80; x++)
                    VMem[80*y+x] = (VMem[80*y+x] & 0xFF00) | ' ';
            x = 0;
            y = 0;
        }
    }
}

void kprintHex(uint8_t hex)
{
    char* msg = "00";
    char* hexStr = "0123456789ABCDEF";
    msg[0] = hexStr[(hex >> 4) & 0xF];
    msg[1] = hexStr[hex & 0xF];
    kprintf(msg);
}

class KernelKeyboardEventHandler : public KeyboardEventHandler
{
    public:
        void onKeyDown(char key)
        {
            char* str = " ";
            str[0] = key;
            kprintf(str);
        }
};

class KernelMouseEventHandler: public MouseEventHandler
{
    int8_t x,y;
    public:
        void onActivate()
        {
            x = 40;
            y = 12;
            static uint16_t* VMem = (uint16_t*)0xb8000;

            VMem[80*12+40] = ((VMem[80*12+40] & 0xF000) >> 4) |
                            ((VMem[80*12+40] & 0x0F00) << 4) |
                            ((VMem[80*12+40] & 0x00FF));
        }

        void onMouseMove(int xOffset, int yOffset)
        {
            static uint16_t* VMem = (uint16_t*)0xb8000;
            VMem[80*y+x] = ((VMem[80*y+x] & 0xF000) >> 4) |
            ((VMem[80*y+x] & 0x0F00) << 4) |
            ((VMem[80*y+x] & 0x00FF));

            x += xOffset;
            if (x < 0) x = 0;
            if (x >= 80) x = 79;

            y += yOffset;
            if (y < 0) y = 0;
            if (y >= 25) y = 24;

            VMem[80*y+x] = ((VMem[80*y+x] & 0xF000) >> 4) |
                        ((VMem[80*y+x] & 0x0F00) << 4) |
                        ((VMem[80*y+x] & 0x00FF));
        }

        void onMouseDown(uint8_t button)
        {
            kprintf("Mouse down\n");
        }

        void onMouseUp(uint8_t button)
        {
            kprintf("Mouse up\n");
        }
};

typedef void (*constructor)();
extern "C" constructor start_ctors;
extern "C" constructor end_ctors;
extern "C" void callConstructors()
{
    for (constructor* i = &start_ctors; i != &end_ctors; i++)
    {
        (*i)();
    }
}

extern "C" void kmain(void* multiboot_structure, uint32_t magic)
{
    kprintf("Welcome to NBSOS!\n");
    kprintf("Getting things setup...\n");

    kprintf("Setting up GDT...\n");
    GlobalDescriptorTable gdt;

    kprintf("Setting up IDT...\n");
    InterruptManager interrupts(&gdt);

    kprintf("Setting up Driver Management...\n");
    DriverManager driverManager;

    kprintf("Setting up Keyboard...\n");
    KernelKeyboardEventHandler keyboardHandler;
    KeyboardDriver keyboard(&interrupts, &keyboardHandler);
    driverManager.addDriver(&keyboard);

    kprintf("Setting up Mouse...\n");
    KernelMouseEventHandler mouseHandler;
    MouseDriver mouse(&interrupts, &mouseHandler);
    driverManager.addDriver(&mouse);

    PCIController pciController;
    pciController.selectDrivers(&driverManager, &interrupts);

    kprintf("Activating Drivers...\n");
    driverManager.activateAll();

    kprintf("Activating interrupts...\n");
    interrupts.activate();

    while(1);
}