#ifndef __NBSOS__HARDWARE__INTERRUPTS_H
#define __NBSOS__HARDWARE__INTERRUPTS_H

#include <gdt.h>
#include <common/types.h>
#include <hardware/port.h>

namespace NBSOS
{
    namespace Hardware
    {
        class InterruptManager;

        class InterruptHandler
        {
            protected:
                Common::uint8_t interrupt;
                InterruptManager* interruptManager;

                InterruptHandler(Common::uint8_t interrupt, InterruptManager* interruptManager);
                ~InterruptHandler();
            public:
                virtual Common::uint32_t handleInterrupt(Common::uint32_t esp);
        };

        class InterruptManager
        {
            friend class InterruptHandler;
            protected:
                static InterruptManager* activeInterruptManager; 
                InterruptHandler* handlers[256];

                struct GateDescriptor
                {
                    Common::uint16_t handlerAddressLowBits;
                    Common::uint16_t gdt_codeSegmentSelector;
                    Common::uint8_t reserved;
                    Common::uint8_t access;
                    Common::uint16_t handlerAddressHighBits;
                } __attribute__((packed));

                static GateDescriptor interruptDescriptorTable[256];

                struct InterruptDescriptorTablePointer
                {
                    Common::uint16_t size;
                    Common::uint32_t base;
                } __attribute__((packed));

                static void setInterruptDescriptorTableEntry(
                    Common::uint8_t interruptNumber,
                    Common::uint16_t codeSegmentSelectorOffset,
                    void (*handler)(),
                    Common::uint8_t DescriptorPrivilegeLevel,
                    Common::uint8_t DescriptorType
                );

                Port8BitSlow picMasterCommand;
                Port8BitSlow picMasterData;
                Port8BitSlow picSlaveCommand;
                Port8BitSlow picSlaveData;

            public:
                InterruptManager(GlobalDescriptorTable* gdt);
                ~InterruptManager();
                static Common::uint32_t handleInterrupt(Common::uint8_t interrupt, Common::uint32_t esp);
                Common::uint32_t doHandleInterrupt(Common::uint8_t interrupt, Common::uint32_t esp);
                void activate();
                void deactivate();
                static void ignoreInterruptRequest();
                static void handleInterruptRequest0x00();
                static void handleInterruptRequest0x01();
                static void handleInterruptRequest0x0C();
        };
    }
}
#endif