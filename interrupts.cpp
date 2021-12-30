#include "interrupts.h"

void kprintf(char* str);

uint32_t InterruptManager::handleInterrupt(uint8_t interrupt, uint32_t esp)
{
    kprintf("Interrupt received\n");
    return esp;
}

InterruptManager::GateDescriptor InterruptManager::interruptDescriptorTable[256];

InterruptManager InterruptManager::activeInterruptManager = 0;

void InterruptManager::setInterruptDescriptorTableEntry(
    uint8_t interruptNumber,
    uint16_t codeSegmentSelectorOffset,
    void (*handler)(),
    uint8_t DescriptorPrivilegeLevel,
    uint8_t DescriptorType
)
{
    const uint8_t IDT_DESC_PRESENT = 0x80;
    interruptDescriptorTable[interruptNumber].handlerAddressLowBits = ((uint32_t)handler) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].handlerAddressHighBits = (((uint32_t)handler) >> 16) & 0xFFFF;
    interruptDescriptorTable[interruptNumber].gdt_codeSegmentSelector = codeSegmentSelectorOffset;
    interruptDescriptorTable[interruptNumber].access = IDT_DESC_PRESENT | DescriptorType | ((DescriptorPrivilegeLevel&3) << 5);
    interruptDescriptorTable[interruptNumber].reserved = 0;
}

void InterruptManager::activate()
{
    if (activeInterruptManager != 0)
    {
        activeInterruptManager->deactivate();
    }
    activeInterruptManager = this;
    asm("sti");
}

void InterruptManager::deactivate()
{
    if (activeInterruptManager == this)
    {
        activeInterruptManager = 0;
        asm("cli")
    }
    
}

InterruptManager::InterruptManager(GlobalDescriptorTable* gdt)
: picMasterCommand(0x20),
  picMasterData(0x21),
  picSlaveCommand(0xA0),
  picSlaveData(0xA1)
{
    uint16_t codeSegmentSelector = gdt->CodeSegmentSelector();
    const uint8_t IDT_INTERRUPT_GATE = 0xE;

    for (uint16_t i =0; i < 256; i++)
    {
        setInterruptDescriptorTableEntry(
            i,
            codeSegmentSelector,
            &ignoreInterruptRequest,
            0,
            IDT_INTERRUPT_GATE
        );
    }

    // timer
    setInterruptDescriptorTableEntry(
            0x20,
            codeSegmentSelector,
            &handleInterruptRequest0x00,
            0,
            IDT_INTERRUPT_GATE
    );

    // keyboard
    setInterruptDescriptorTableEntry(
            0x21,
            codeSegmentSelector,
            &handleInterruptRequest0x01,
            0,
            IDT_INTERRUPT_GATE
    );

    // remap PIC
    picMasterCommand.write(0x11);
    picSlaveCommand.write(0x11);
    picMasterData.write(0x20);
    picSlaveData.write(0x28);

    // // assign roles (master/slave)
    picMasterData.write(0x04);
    picSlaveData.write(0x02);
    picMasterData.write(0x01);
    picSlaveData.write(0x01);
    picMasterData.write(0x00);
    picSlaveData.write(0x00);


    // load IDT
    InterruptDescriptorTablePointer idt;
    idt.size = 256 * sizeof(GateDescriptor) - 1;
    idt.base = (uint32_t)interruptDescriptorTable;
    asm volatile("lidt %0" : : "m" (idt));
}

InterruptManager::~InterruptManager()
{
}