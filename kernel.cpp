#include "types.h"
#include "gdt.h"
#include "interrupts.h"

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
    interrupts.activate();


    while(1);
}