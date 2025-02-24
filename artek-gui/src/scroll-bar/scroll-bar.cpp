#include "scroll-bar.h"
#include "qtmaterialscrollbar.h"
#include "src/theme/theme.h"

gsl::owner<QScrollBar*> makeScroll(Qt::Orientation o, QWidget* p)
{
    gsl::owner<QtMaterialScrollBar*> scroll = new QtMaterialScrollBar(p);
    scroll->setOrientation(o);
    scroll->setSliderColor(theme::iconColor());
    scroll->setBackgroundColor(theme::mixColors(theme::backgroundColor(), theme::foregroundColor()));
    return scroll;
}
