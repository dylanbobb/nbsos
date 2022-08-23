#ifndef __NBSOS__GUI__WIDGET_H
#define __NBSOS__GUI__WIDGET_H

#include <common/types.h>
#include <common/graphicscontext.h>

namespace NBSOS
{
    namespace GUI
    {
        class Widget
        {
            protected:
                Widget* parent;
                Common::int32_t x;
                Common::int32_t y;
                Common::int32_t w;
                Common::int32_t h;
                Common::uint8_t r;
                Common::uint8_t g;
                Common::uint8_t b;
                bool focusable;
            public:
                Widget(Widget* parent,
                       Common::int32_t x,
                       Common::int32_t y,
                       Common::int32_t w, 
                       Common::int32_t h, 
                       Common::int8_t r, 
                       Common::int8_t g, 
                       Common::int8_t b);
                ~Widget();
                virtual void getFocus(Widget* Widget);
                virtual void modelToScreen(Common::int32_t &x, Common::int32_t &y);
                virtual void draw(Common::GraphicsContext *gc);
                virtual void onMouseDown(Common::int32_t x, Common::int32_t y);
                virtual void onMouseUp(Common::int32_t x, Common::int32_t y);
                virtual void onMouseMove(Common::int32_t oldx, Common::int32_t oldy, Common::int32_t newx, Common::int32_t newy);
                virtual void onKeyDown(char);
                virtual void onKeyUp(char);
        };

        class CompositeWidget : public Widget
        {
            private:
                Widget* children[100];
                int numChildren;
                Widget* focusedChild;
            public:
                CompositeWidget(Widget* parent,
                                Common::int32_t x,
                                Common::int32_t y,
                                Common::int32_t w, 
                                Common::int32_t h, 
                                Common::int8_t r, 
                                Common::int8_t g, 
                                Common::int8_t b);
                ~CompositeWidget();
                virtual void getFocus(Widget* Widget);
                virtual void draw(Common::GraphicsContext *gc);
                virtual void onMouseDown(Common::int32_t x, Common::int32_t y);
                virtual void onMouseUp(Common::int32_t x, Common::int32_t y);
                virtual void onMouseMove(Common::int32_t oldx, Common::int32_t oldy, Common::int32_t newx, Common::int32_t newy);
                virtual void onKeyDown(char);
                virtual void onKeyUp(char);
        }; 
    }
}

#endif
