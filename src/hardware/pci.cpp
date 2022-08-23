#include <hardware/pci.h>

using namespace NBSOS::Common;
using namespace NBSOS::Hardware;
using namespace NBSOS::Drivers;

void kprintf(char* str);
void kprintHex(uint8_t);

PCIDeviceDescriptor::PCIDeviceDescriptor()
{
}

PCIDeviceDescriptor::~PCIDeviceDescriptor()
{
}

PCIController::PCIController()
: dataPort(0xCFC),
  commandPort(0xCF8)
{
}

PCIController::~PCIController()
{
}

uint32_t PCIController::read(uint16_t bus, uint16_t device, uint16_t function, uint32_t registerOffset)
{
    uint32_t id = 
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registerOffset & 0xFC);
    
    commandPort.write(id);
    uint32_t result = dataPort.read();
    return result >> (8 * (registerOffset % 4));
}

void PCIController::write(uint16_t bus, uint16_t device, uint16_t function, uint32_t registerOffset, uint32_t value)
{
    uint32_t id = 
        0x1 << 31
        | ((bus & 0xFF) << 16)
        | ((device & 0x1F) << 11)
        | ((function & 0x07) << 8)
        | (registerOffset & 0xFC);
    
    commandPort.write(id);
    dataPort.write(value);
}

bool PCIController::deviceHasFunctions(uint16_t bus, uint16_t device)
{
    return read(bus, device, 0, 0x0E) & (1 << 7);   
}

void PCIController::selectDrivers(DriverManager* driverManager, InterruptManager* interrupt)
{
    // TODO: Hooray for triple nesting! Surely there's a more clever way to check this...
    for (int bus = 0; bus < 8; bus++)
    {
        for (int device = 0; device < 32; device++)
        {
            int numFunctions = deviceHasFunctions(bus, device) ? 8 : 1;
            for (int function = 0; function < numFunctions; function++)
            {
                PCIDeviceDescriptor descriptor = getDeviceDescriptor(bus, device, function);

                if (descriptor.vendorId == 0x0000 || descriptor.vendorId == 0xFFFF) continue;

                for (int barNum = 0; barNum < 6; barNum++)
                {
                    BaseAddressRegister bar = getBaseAddressRegister(bus, device, function, barNum);
                    if (bar.address && (bar.type == InputOutput))
                        descriptor.portBase = (uint32_t) bar.address;

                    Driver* driver = getDriver(descriptor, interrupt);
                    if (driver != 0)
                        driverManager->addDriver(driver);
                }

                kprintf("PCI BUS ");
                kprintHex(bus & 0xFF);

                kprintf(", DEVICE ");
                kprintHex(device & 0xFF);

                kprintf(", FUNCTION ");
                kprintHex(function & 0xFF);

                kprintf(" = VENDOR ");
                kprintHex((descriptor.vendorId & 0xFF00) >> 8);
                kprintHex(descriptor.vendorId & 0xFF);
                kprintf(", DEVICE ");
                kprintHex((descriptor.deviceId & 0xFF00) >> 8);
                kprintHex(descriptor.deviceId & 0xFF);
                kprintf("\n");
            }
        }
    }
}

BaseAddressRegister PCIController::getBaseAddressRegister(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar)
{
    BaseAddressRegister result;

    uint32_t headerType = read(bus, device, function, 0x0E) & 0x7F;
    int maxBars = 6 - (4 * headerType);
    if (bar >= maxBars)
        return result;

    uint32_t barValue = read(bus, device, function, 0x10 + 4 * bar);
    result.type = (barValue & 0x1) ? InputOutput : MemoryMapping;
    uint32_t temp; // TODO: determine which bits of a MemoryMapping BaseAddressRegister are writeable

    if (result.type == MemoryMapping)
    {
        switch((barValue >> 1) & 0x3)
        {
            case 0: // 32 bit
            case 1: // 20 bit
            case 2: // 64 bit
                break;
        }

        result.prefetchable = ((barValue >> 3) & 0x1) == 0x1;
    }
    else
    {
        // IO
        result.address = (uint8_t*) (barValue & ~0x3);
        result.prefetchable = false;
    }

    return result;
}

Driver* PCIController::getDriver(PCIDeviceDescriptor descriptor, InterruptManager* interrupt)
{
    Driver* driver = 0;
    switch (descriptor.vendorId)
    {
        case 0x1022:
            // AMD
            switch (descriptor.deviceId)
            {
                case 0x2000:
                    kprintf("AMD am79c973 ");
                    break;
            }
            break;
        case 0x8086:
            // Intel
            break;
    }

    switch (descriptor.classId)
    {
        case 0x03:
            // Graphics
            switch (descriptor.subclassId)
            {
                case 0x00:
                    kprintf("VGA ");
                    break;
            }
            break;
    }

    return driver;
}

PCIDeviceDescriptor PCIController::getDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function)
{
    PCIDeviceDescriptor result;

    result.bus = bus;
    result.device = device;
    result.function = function;

    result.vendorId = read(bus, device, function, 0x00);
    result.deviceId = read(bus, device, function, 0x02);

    result.classId = read(bus, device, function, 0x0b);
    result.subclassId = read(bus, device, function, 0x0a);

    result.interfaceId = read(bus, device, function, 0x09);

    result.revision = read(bus, device, function, 0x08);
    result.interrupt = read(bus, device, function, 0x3c);

    return result;
}

