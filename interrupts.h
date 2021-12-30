#ifndef __INTERRUPTS_H
#define __INTERRUPTS_H

    #include "gdt.h"
    #include "types.h"
    #include "port.h"

    class InterruptManager
    {
        protected:
            static InterruptManager* activeInterruptManager; 

            struct GateDescriptor
            {
                uint16_t handlerAddressLowBits;
                uint16_t gdt_codeSegmentSelector;
                uint8_t reserved;
                uint8_t access;
                uint16_t handlerAddressHighBits;
            } __attribute__((packed));

            static GateDescriptor interruptDescriptorTable[256];

            struct InterruptDescriptorTablePointer
            {
                uint16_t size;
                uint32_t base;
            } __attribute__((packed));

            static void setInterruptDescriptorTableEntry(
                uint8_t interruptNumber,
                uint16_t codeSegmentSelectorOffset,
                void (*handler)(),
                uint8_t DescriptorPrivilegeLevel,
                uint8_t DescriptorType
            );

            Port8BitSlow picMasterCommand;
            Port8BitSlow picMasterData;
            Port8BitSlow picSlaveCommand;
            Port8BitSlow picSlaveData;

        public:
            InterruptManager(GlobalDescriptorTable* gdt);
            ~InterruptManager();
            static uint32_t handleInterrupt(uint8_t interrupt, uint32_t esp);
            uint32_t doHandleInterrupt(uint8_t interrupt, uint32_t esp);
            void activate();
            void deactivate();
            static void ignoreInterruptRequest();
            static void handleInterruptRequest0x00();
            static void handleInterruptRequest0x01();
    };
#endif