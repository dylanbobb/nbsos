#include <drivers/vga.h>

using namespace NBSOS::Common;
using namespace NBSOS::Drivers;
using namespace NBSOS::Hardware;

void VGA::writeRegisters(uint8_t* registers)
{
    // misc
    miscPort.write(*(registers++));

    // sequencer
    for (uint8_t i = 0; i < 5; i++)
    {
        sequencerIndexPort.write(i);
        sequencerDataPort.write(*(registers++));
    }

    // cathode
    //      acquire lock
    cathodeIndexPort.write(0x03); 
    cathodeDataPort.write(cathodeDataPort.read() | 0x80);
    cathodeIndexPort.write(0x11);
    cathodeDataPort.write(cathodeDataPort.read() & ~0x80);
    registers[0x03] = registers[0x03] | 0x80;
    registers[0x11] = registers[0x11] & ~0x80; 
    for (uint8_t i = 0; i < 25; i++)
    {
        cathodeIndexPort.write(i);
        cathodeDataPort.write(*(registers++));
    }

    // graphics controller
    for (uint8_t i = 0; i < 9; i++)
    {
        graphicsControllerIndexPort.write(i);
        graphicsControllerDataPort.write(*(registers++));
    }

    // attributes
    for (uint8_t i = 0; i < 21; i++)
    {
        attributeControllerResetPort.read();
        attributeControllerIndexPort.write(i);
        attributeControllerWritePort.write(*(registers++));
    }

    attributeControllerResetPort.read();
    attributeControllerIndexPort.write(0x20);
}

uint8_t* VGA::getFrameBufferSegment()
{
    graphicsControllerIndexPort.write(0x06);
    uint8_t segmentNumber = ((graphicsControllerDataPort.read() >> 2) & 0x03);
    switch (segmentNumber)
    {
        default:
        case 0: return (uint8_t*) 0x00000;
        case 1: return (uint8_t*) 0xA0000;
        case 2: return (uint8_t*) 0xB0000;
        case 3: return (uint8_t*) 0xB8000;
    }
}

void VGA::putPixel(uint32_t x, uint32_t y, uint8_t colorIndex)
{
    uint8_t* pixelAddress = getFrameBufferSegment() + (320 * y) + x;
    *pixelAddress = colorIndex;
}

uint8_t VGA::getColorIndex(uint8_t r, uint8_t g, uint8_t b)
{
    if (r == 0x00 && g == 0x00 && b == 0xA8)
        return 0x01;
}

VGA::VGA() :
    miscPort(0x3c2),
    cathodeIndexPort(0x3d4),
    cathodeDataPort(0x3d5),
    sequencerIndexPort(0x3c4),
    sequencerDataPort(0x3c5),
    graphicsControllerIndexPort(0x3ce),
    graphicsControllerDataPort(0x3cf),
    attributeControllerIndexPort(0x3c0),
    attributeControllerReadPort(0x3c1),
    attributeControllerWritePort(0x3c0),
    attributeControllerResetPort(0x3da)
{

}

VGA::~VGA()
{

}

bool VGA::supportsMode(uint32_t width, uint32_t height, uint32_t colorDepth)
{
    return width == 320 && height == 200 && colorDepth == 8;
}

bool VGA::setMode(uint32_t width, uint32_t height, uint32_t colorDepth)
{
    if (!supportsMode(width, height, colorDepth))
        return false;

    unsigned char g_320x200x256[] =
    {
        /* MISC */
            0x63,
        /* SEQ */
            0x03, 0x01, 0x0F, 0x00, 0x0E,
        /* CRTC */
            0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
            0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x9C, 0x0E, 0x8F, 0x28,	0x40, 0x96, 0xB9, 0xA3,
            0xFF,
        /* GC */
            0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
            0xFF,
        /* AC */
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x41, 0x00, 0x0F, 0x00,	0x00
    };

    writeRegisters(g_320x200x256);
    return true;
}

void VGA::putPixel(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    putPixel(x, y, getColorIndex(r, g, b));
}

void VGA::fillRectangle(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint8_t r, uint8_t g, uint8_t b)
{
    for (uint32_t Y = y; Y < y + h; Y++)
        for (uint32_t X = x; X < x + w; X++)
            putPixel(X, Y, r, g, b);
}