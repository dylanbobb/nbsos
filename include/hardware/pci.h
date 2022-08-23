#ifndef __NBSOS__HARDWARE__PCI_H
#define __NBSOS__HARDWARE__PCI_H

#include <common/types.h>
#include <hardware/port.h>
#include <hardware/interrupts.h>
#include <drivers/driver.h>

namespace NBSOS
{
    namespace Hardware
    {
        enum BaseAddressRegisterType 
        {
            MemoryMapping = 0,
            InputOutput = 1
        };

        class BaseAddressRegister
        {
            public:
                bool prefetchable;
                Common::uint8_t* address;
                Common::uint32_t size;
                BaseAddressRegisterType type;
        };

        class PCIDeviceDescriptor
        {
            public:
                Common::uint32_t portBase;
                Common::uint32_t interrupt;

                Common::uint16_t bus;
                Common::uint16_t device;
                Common::uint16_t function;

                Common::uint16_t vendorId;
                Common::uint16_t deviceId;

                Common::uint8_t classId;
                Common::uint8_t subclassId;
                Common::uint8_t interfaceId;

                Common::uint8_t revision;

                PCIDeviceDescriptor();
                ~PCIDeviceDescriptor();
        };

        class PCIController
        {
            Port32Bit dataPort;
            Port32Bit commandPort;

            public:
                PCIController();
                ~PCIController();
                
                Common::uint32_t read(Common::uint16_t bus, Common::uint16_t device, Common::uint16_t function, Common::uint32_t registerOffset);
                void write(Common::uint16_t bus, Common::uint16_t device, Common::uint16_t function, Common::uint32_t registerOffset, Common::uint32_t value);
                bool deviceHasFunctions(Common::uint16_t bus, Common::uint16_t device);

                void selectDrivers(Drivers::DriverManager* driverManager, Hardware::InterruptManager* interrupt);
                Drivers::Driver* getDriver(PCIDeviceDescriptor descriptor, Hardware::InterruptManager* interrupt);
                PCIDeviceDescriptor getDeviceDescriptor(Common::uint16_t bus, Common::uint16_t device, Common::uint16_t function);
                BaseAddressRegister getBaseAddressRegister(Common::uint16_t bus, Common::uint16_t device, Common::uint16_t function, Common::uint16_t bar);
        };
    }
}
#endif