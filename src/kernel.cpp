#include <common/types.h>
#include <gdt.h>
#include <hardware/interrupts.h>
#include <hardware/pci.h>
#include <drivers/driver.h>
#include <drivers/keyboard.h>
#include <drivers/mouse.h>
#include <drivers/vga.h>
#include <gui/desktop.h>
#include <gui/window.h>

using namespace NBSOS;
using namespace NBSOS::Common;
using namespace NBSOS::Drivers;
using namespace NBSOS::Hardware;
using namespace NBSOS::GUI;

static uint8_t x = 0;
static uint8_t y = 0;

void kprintf(char* str)
{
    static uint16_t* VMem = (uint16_t*)0xb8000;

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

void clearScreen()
{
    static uint16_t* VMem = (uint16_t*)0xb8000;
    for (y = 0; y < 25; y++)
        for (x = 0; x < 80; x++)
            VMem[80*y+x] = (VMem[80*y+x] & 0xFF00) | ' ';
    x = 0;
    y = 0;
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
    kprintf("__    __  _______    ______    ______    ______  \n/  \\  /  |/       \\  /      \\  /      \\  /      \\ \n$$  \\ $$ |$$$$$$$  |/$$$$$$  |/$$$$$$  |/$$$$$$  |\n$$$  \\$$ |$$ |__$$ |$$ \\__$$/ $$ |  $$ |$$ \\__$$/ \n$$$$  $$ |$$    $$< $$      \\ $$ |  $$ |$$      \\ \n$$ $$ $$ |$$$$$$$  | $$$$$$  |$$ |  $$ | $$$$$$  |\n$$ |$$$$ |$$ |__$$ |/  \\__$$ |$$ \\__$$ |/  \\__$$ |\n$$ | $$$ |$$    $$/ $$    $$/ $$    $$/ $$    $$/ \n$$/   $$/ $$$$$$$/   $$$$$$/   $$$$$$/   $$$$$$/\n");
    kprintf("\n\nBy: Dylan Bobb\n\n");
    int i = 0;
    while (i < 2000000000)
    {
        i++;
        if (i % 100000000 == 0) kprintf(".");
    }
    clearScreen();
    kprintf("Getting things setup...\n");

    kprintf("Setting up GDT...\n");
    GlobalDescriptorTable gdt;

    kprintf("Setting up IDT...\n");
    InterruptManager interrupts(&gdt);

    kprintf("Setting up Desktop...\n");
    Desktop desktop(320, 200, 0x00, 0x00, 0xA8);

    kprintf("Setting up Driver Management...\n");
    DriverManager driverManager;

    kprintf("Setting up Keyboard...\n");
    // KernelKeyboardEventHandler keyboardHandler;
    // KeyboardDriver keyboard(&interrupts, &keyboardHandler); // uncomment these for text mode keyboard
    KeyboardDriver keyboard(&interrupts, &desktop);
    driverManager.addDriver(&keyboard);

    kprintf("Setting up Mouse...\n");
    // KernelMouseEventHandler mouseHandler;
    // MouseDriver mouse(&interrupts, &mouseHandler); // uncomment these for text mode mouse
    MouseDriver mouse(&interrupts, &desktop);
    driverManager.addDriver(&mouse);

    kprintf("Setting up PCI Controller...\n");
    PCIController pciController;
    pciController.selectDrivers(&driverManager, &interrupts);

    kprintf("Setting up VGA...\n");
    VGA vga;

    kprintf("Activating Drivers...\n");
    driverManager.activateAll();

    kprintf("Activating interrupts...\n");
    interrupts.activate();

    kprintf("Creating windows...\n");
    Window window1(&desktop, 10, 10, 20, 20, 0xA8, 0x00, 0x00);
    desktop.addChild(&window1);
    Window window2(&desktop, 40, 15, 30, 20, 0x00, 0xA8, 0x00);
    desktop.addChild(&window2);

    kprintf("Switching to graphics mode...\n");
    i = 0;
    while (i < 2000000000)
    {
        i++;
        if (i % 100000000 == 0) kprintf(".");
    }
    clearScreen();
    vga.setMode(320, 200, 8);

    while(1)
    {
        // TODO: This is a terrible idea once we allow for multi-threading
        desktop.draw(&vga);
    }
}