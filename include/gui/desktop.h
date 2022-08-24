#ifndef __NBSOS__GUI__DESKTOP_H
#define __NBSOS__GUI__DESKTOP_H

#include <gui/widget.h>
#include <drivers/mouse.h>

namespace NBSOS
{
    namespace GUI
    {
        class Desktop : public CompositeWidget, public Drivers::MouseEventHandler
        {
            protected:
                Common::uint32_t mouseX;
                Common::uint32_t mouseY;
            public:
                Desktop(Common::int32_t w, Common::int32_t h, Common::uint8_t r, Common::uint8_t g, Common::uint8_t b);
                ~Desktop();

                void draw(Common::GraphicsContext* gc);
                void onMouseMove(int xOffset, int yOffset);
                void onMouseDown(Common::uint8_t button);
                void onMouseUp(Common::uint8_t button);
        };
    }
}

#endif