#ifndef __NBSOS__HARDWARE__PCI_H
#define __NBSOS__HARDWARE__PCI_H

#include <common/types.h>
#include <hardware/port.h>

namespace NBSOS
{
    namespace Hardware
    {
        class PCIDeviceDescriptor
        {
            public:
                Common::uint32_t portBase;
                Common::uint32_t interrupt;

                Common::uint16_t vendorID;
                Common::uint16_t deviceID;

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
                PCIController(Common::uint32_t portBase);
                ~PCIController();
                
                Common::uint32_t write();
        };
    }
}
#endif