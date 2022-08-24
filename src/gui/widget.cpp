#include <gui/widget.h>

using namespace NBSOS::Common;
using namespace NBSOS::GUI;

Widget::Widget(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h, int8_t r, int8_t g, int8_t b)
: KeyboardEventHandler()
{
    this->parent = parent;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->r = r;
    this->g = g;
    this->b = b;
    this->focusable = true;
}

Widget::~Widget()
{
}

void Widget::getFocus(Widget* widget)
{
    if (parent != 0)
        parent->getFocus(widget);
}

void Widget::modelToScreen(int32_t &x, int32_t &y)
{
    if (parent != 0)
        parent->modelToScreen(x, y);
    x += this->x;
    y += this->y;
}

void Widget::draw(GraphicsContext* gc)
{
    int X = 0;
    int Y = 0;
    modelToScreen(X, Y);
    gc->fillRectangle(X, Y, w, h, r, g, b);
}

void Widget::onMouseDown(int32_t x, int32_t y, uint8_t button)
{
    if (focusable)
        getFocus(this);
}

void Widget::onMouseUp(int32_t x, int32_t y, uint8_t button)
{

}

void Widget::onMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy)
{

}

bool Widget::containsCoordinate(int32_t x, int32_t y)
{
    return this->x <= x && x < this->x + this-> w
        && this->y <= y && y < this->y + this->h; 
}

CompositeWidget::CompositeWidget(Widget* parent, int32_t x, int32_t y, int32_t w, int32_t h, int8_t r, int8_t g, int8_t b)
: Widget(parent, x, y, w, h, r, g, b)
{
    focusedChild = 0;
    numChildren = 0;
}

CompositeWidget::~CompositeWidget()
{

}

void CompositeWidget::getFocus(Widget* widget)
{
    focusedChild = widget;
    if (parent != 0)
        parent->getFocus(this);
}

bool CompositeWidget::addChild(Widget* child)
{
    if (numChildren >= 100) return false;
    children[numChildren++] = child;
    return true;
}

void CompositeWidget::draw(GraphicsContext* gc)
{
    Widget::draw(gc);
    for (int i = numChildren - 1; i >= 0; i--)
        children[i]->draw(gc);
}

void CompositeWidget::onKeyDown(char str)
{
    if (focusedChild != 0)
        focusedChild->onKeyDown(str);
}

void CompositeWidget::onKeyUp(char str)
{
    if (focusedChild != 0)
        focusedChild->onKeyUp(str);
}

void CompositeWidget::onMouseDown(int32_t x, int32_t y, uint8_t button)
{
    for (int i = 0; i < numChildren; i++)
    {
        if (children[i]->containsCoordinate(x - this->x, y - this->y))
        {
            children[i]->onMouseDown(x - this->x, y - this->y, button);
            break;
        }
    }    
}

void CompositeWidget::onMouseUp(int32_t x, int32_t y, uint8_t button)
{
    for (int i = 0; i < numChildren; i++)
    {
        if (children[i]->containsCoordinate(x - this->x, y - this->y))
        {
            children[i]->onMouseUp(x - this->x, y - this->y, button);
            break;
        }
    }    
}

void CompositeWidget::onMouseMove(int32_t oldx, int32_t oldy, int32_t newx, int32_t newy)
{
    int firstChild = -1;
    for (int i = 0; i < numChildren; i++)
    {
        if (children[i]->containsCoordinate(oldx - this->x, oldy - this->y))
        {
            children[i]->onMouseMove(oldx - this->x, oldy - this->y, newx - this->x, newy - this->y);
            firstChild = i;
            break;
        }
    }

    for (int i = 0; i < numChildren; i++)
    {
        if (children[i]->containsCoordinate(newx - this->x, newy - this->y))
        {
            if (firstChild != i) // avoid sending 2 events to the same widget
                children[i]->onMouseMove(oldx - this->x, oldy - this->y, newx - this->x, newy - this->y);
            break;
        }
    }        
}