#ifndef __NBSOS__GUI__WINDOW_H
#define __NBSOS__GUI__WINDOW_H

#include <gui/widget.h>

namespace NBSOS
{
    namespace GUI
    {
        class Window : public CompositeWidget
        {
            protected:
                bool dragging;
            public:
                Window(Widget* parent,
                       Common::int32_t x,
                       Common::int32_t y,
                       Common::int32_t w, 
                       Common::int32_t h, 
                       Common::int8_t r, 
                       Common::int8_t g, 
                       Common::int8_t b);
                ~Window();
                void onMouseDown(Common::int32_t x, Common::int32_t y, Common::uint8_t button);
                void onMouseUp(Common::int32_t x, Common::int32_t y, Common::uint8_t button);
                void onMouseMove(Common::int32_t oldx, Common::int32_t oldy, Common::int32_t newx, Common::int32_t newy);
        };
    }
}

#endif