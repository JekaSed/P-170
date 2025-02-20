#include "flat-button.h"
#include "src/theme/theme.h"
#include <qtmaterialflatbutton.h>

QAbstractButton* makeFlatButton(const QString& text, const QIcon& icon, QWidget* parent)
{
    auto* bt = new QtMaterialFlatButton(text, parent);
    bt->setBackgroundMode(Qt::OpaqueMode);
    bt->setBackgroundColor(theme::iconColor());
    bt->setForegroundColor(theme::textColor());
    bt->setCornerRadius(4);
    bt->setIconSize({20, 20});
    bt->setIcon(icon);
    bt->setCursor(Qt::PointingHandCursor);
    return bt;
}
