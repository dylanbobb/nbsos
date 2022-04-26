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

void PCIController::selectDrivers(DriverManager* driverManager)
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