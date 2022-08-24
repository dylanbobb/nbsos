#include <gui/window.h>

using namespace NBSOS::Common;
using namespace NBSOS::GUI;

Window::Window(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h, int8_t r, int8_t g, int8_t b)
: CompositeWidget(parent, x, y, w, h, r, g, b)
{
    dragging = false;
}

Window::~Window()
{

}

void Window::onMouseDown(Common::int32_t x, Common::int32_t y, Common::uint8_t button)
{
    dragging = button == 1; // lmb
    CompositeWidget::onMouseDown(x, y, button);
}

void Window::onMouseUp(Common::int32_t x, Common::int32_t y, Common::uint8_t button)
{
    dragging = false;
    CompositeWidget::onMouseDown(x, y, button);
}

void Window::onMouseMove(Common::int32_t oldx, Common::int32_t oldy, Common::int32_t newx, Common::int32_t newy)
{
    if (dragging)
    {
        this->x += newx - oldx;
        this->y += newy - oldy;
    }

    CompositeWidget::onMouseMove(oldx, oldy, newx, newy);
}