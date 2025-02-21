#include "theme.h"
#include <qcolor.h>
#include <QDebug>
#include <qfont.h>
#include <qpainter.h>
#include <QFontDatabase>

namespace theme {

namespace {

const QColor backgroundCl{0x151f30};
const QColor foregroundCl{0x132b3c};
const QColor topMenuCl{0x14243e};
const QColor iconCl{0x0593a2};
const QColor iconHighLightCl{0xccff00};
const QColor titleCl{0xa15743};
const QColor redCl{0xe3371e};
const QColor disabledCl{0x4d5460};
const QColor tableHeaderCl{0x243a49};

int correct(int src, int dst)
{
    const auto step = 5;

    if (src < dst) {
        src += step;
        if (src > dst) {
            return dst;
        }
        return src;
    }
    if (src > dst) {
        src -= step;
        if (src < dst) {
            return dst;
        }
        return src;
    }
    return dst;
}

QFont registerFont(const QString& fontName)
{
    const int id = QFontDatabase::addApplicationFont(fontName);
    if (id == -1) {
        qWarning() << "не удалось загрузить шрифт" << fontName;
        return {};
    }
    const QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    QFont font(family);
    return font;
}

QFont makeInfoFont()
{
    auto font = registerFont(QLatin1String(":/resources/fonts/Fragua-Pro-Regular.otf"));
    font.setPixelSize(45);
    font.setWeight(QFont::DemiBold);
    return font;
}

QFont makeBaseFont()
{
    auto font = registerFont(QLatin1String(":/resources/fonts/Lato-Regular.ttf"));
    return font;
}

QFont makeDigitalFont()
{
    auto font = registerFont(QLatin1String(":/polus/fonts/TickingTimebombBb.ttf"));
    font.setPixelSize(20);
    font.setWeight(QFont::DemiBold);
    return font;
}

}   // namespace

QColor backgroundColor()
{
    return backgroundCl;
}

QColor foregroundColor()
{
    return foregroundCl;
}

QColor topMenuColor()
{
    return topMenuCl;
}

QColor errorColor()
{
    return redCl;
}

QColor disabledColor()
{
    return disabledCl;
}

QColor textColor()
{
    return 0xBABABA;
}

QColor iconColor()
{
    return iconCl;
}

QColor iconHighLightColor()
{
    return iconHighLightCl;
}

QColor titleColor()
{
    return titleCl;
}

QColor tableHeader()
{
    return tableHeaderCl;
}

QColor mixColors(QColor c1, QColor c2)
{
    const auto mix = [](int a, int b) {
        constexpr auto k1 = 0.15 * 0.85 / (1. - 0.85 * 0.85);
        constexpr auto k2 = 0.15 / (1. - 0.85 * 0.85);
        return int(a * k1 + b * k2);
    };
    return {mix(c1.red(), c2.red()), mix(c1.green(), c2.green()), mix(c1.blue(), c2.blue())};
}

QIcon changeColor(const QIcon& icon, QColor newColor, QSize size)
{
    QPixmap changedIcon = icon.pixmap(size);
    QPainter iconPainter(&changedIcon);
    iconPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    iconPainter.fillRect(changedIcon.rect(), newColor);
    return changedIcon;
}

QIcon makeIcon(QSize size, QColor color, int radius)
{
    QPixmap icon(size);
    icon.fill(Qt::transparent);
    QPainter iconPainter(&icon);
    iconPainter.setRenderHint(QPainter::RenderHint::Antialiasing);
    iconPainter.setBrush(color);
    iconPainter.setPen(Qt::NoPen);
    iconPainter.drawRoundedRect(icon.rect(), radius, radius);
    return icon;
}

QIcon rotateIcon(const QIcon& icon, int angle, QSize size)
{
    QPixmap changedIcon = icon.pixmap(size);
    QTransform tr;
    tr.rotate(angle);
    changedIcon = changedIcon.transformed(tr);
    return changedIcon;
}

QColor correctTo(QColor src, QColor dst)
{
    const int red = correct(src.red(), dst.red());
    const int green = correct(src.green(), dst.green());
    const int blue = correct(src.blue(), dst.blue());
    const int alpha = correct(src.alpha(), dst.alpha());
    return QColor::fromRgba(qRgba(red, green, blue, alpha));
}

QColor lighterColor(const QColor& col, const int level)
{
    return col.lighter(105 + 15 * level);
}

const QIcon& backIcon()
{
    static const QIcon ic(":/polus/icons/previous.png");
    return ic;
}

const QIcon& nextIcon()
{
    static const QIcon ic{rotateIcon(backIcon(), 180)};
    return ic;
}

const QFont& infoFont()
{
    static const auto f = makeInfoFont();
    return f;
}

const QFont& baseFont()
{
    static const auto f = makeBaseFont();
    return f;
}

const QFont& digitalFont()
{
    static const auto f = makeDigitalFont();
    return f;
}

const QIcon& editIcon()
{
    static const QIcon ic(":/resources/icons/pencil.png");
    return ic;
}

const QIcon& settingsIcon()
{
    static const QIcon ic(":/polus/icons/settings.png");
    return ic;
}

const QIcon& deleteIcon()
{
    static const QIcon ic(":/resources/icons/delete.png");
    return ic;
}

const QIcon& plusIcon()
{
    static const QIcon ic(":/polus/icons/plus.png");
    return ic;
}

const QIcon& openFileIcon()
{
    static const QIcon ic(":/resources/icons/folder-open.png");
    return ic;
}

const QIcon& saveFileIcon()
{
    static const QIcon ic(":/resources/icons/save.png");
    return ic;
}

const QIcon& sendIcon()
{
    static const QIcon ic(":/resources/icons/send.png");
    return ic;
}

const QIcon& activeIcon()
{
    static const QIcon ic = changeColor(QIcon(":polus/icons/active.png"), iconHighLightColor());
    return ic;
}

const QIcon& closeIcon()
{
    static const QIcon ic = changeColor(QIcon(":polus/icons/close.png"), iconColor());
    return ic;
}

const QIcon& loadIcon()
{
    static const QIcon ic = changeColor(QIcon(":polus/icons/download.png"), iconColor());
    return ic;
}

const QIcon& uploadIcon()
{
    static const QIcon ic = changeColor(QIcon(":polus/icons/upload.png"), iconColor());
    return ic;
}

QString mainCSS()
{
    const QString css = R"(
QWidget{background:%1; color:%5;}
QWidget:disabled {background: %4;color:%7;}
QLabel{background:transparent}
QDialog{background:%1; color:%5;}
QToolTip {
    border: 1px solid %3;
    padding:5px;
    border-radius: 3px;
    opacity: 200;
    color:%5;
    background-color:%2;
}
QMenu{background:%3;color:%5; border: 1px solid %3;}
QMenu::item::disabled{background: %4; color: gray;}
QMenu::item::selected{background: %3; color: %1;}
QTableView, QTreeView {
    border:none;
    background:transparent;
    alternate-background-color: %1;
    background: %1;
    selection-background-color:transparent;
}
QTreeView::item:hover {
    background: %6;
}
QTreeView::item:selected {
    border: 1px solid %3;
    border-radius:4px;
}
QTreeView::item:selected:!active{
    color:%5;
    background: transparent;
}

QTableView::item:hover {
    background: %6;
}
QTableView::item:selected {
    border: 1px solid %3;
    border-radius:4px;
    background:%6;
}
QTableView::item::disabled{background: %4; color: gray;}
QTableView::item:selected:!active{
    border: 1px solid %3;
    border-radius:4px;
    background:transparent;
    color:%5;
}
QAbstractSpinBox, QLineEdit{
selection-background-color: %6;
}

)";

    // QHeaderView::down-arrow {image:url(\":/polus/icons/down.png\");}
    // QHeaderView::up-arrow {image: url(\":/polus/icons/up.png\");}

    return (css
              .arg(backgroundColor().name())                           // %1 background
              .arg(topMenuColor().name())                              // %2 menu color
              .arg(iconHighLightColor().name())                        // %3 highlighted
              .arg(disabledColor().name())                             // %4 disabled
              .arg(textColor().name())                                 // %5 text
              .arg(mixColors(backgroundColor(), iconColor()).name())   // %6 item highlight
              .arg(titleColor().name())                                // %7 title color

    );
}

QColor foregroundColor_0()
{
    //    return lighterColor(foregroundColor(), 0);
    //    return foregroundColor();
    return 0x212A3B;
}

QColor foregroundColor_1()
{
    //    return lighterColor(foregroundColor_0(), 1);
    return 0x2C3545;
}

QColor foregroundColor_2()
{
    //    return lighterColor(foregroundColor_0(), 2);
    return 0x373F4E;
}

QColor foregroundColor_3()
{
    //    return lighterColor(foregroundColor_0(), 3);
    return lighterColor(foregroundColor_2(), 1);
}

QColor foregroundColor_4()
{
    //    return lighterColor(foregroundColor_0(), 4);
    return lighterColor(foregroundColor_3(), 1);
}

QColor placeHolderColor()
{
    return iconColor().darker(120);
}

const QIcon& copyIcon()
{
    static const QIcon ic = changeColor(QIcon(":polus/icons/copy.png"), iconColor());
    return ic;
}

const QIcon& buttonUncheckedIcon()
{
    static const QIcon ic =
      changeColor(QIcon(":/materials/icons/toggle/ic_check_box_outline_blank_24px.png"), iconColor());
    return ic;
}

const QIcon& buttonCheckedIcon()
{
    static const QIcon ic = changeColor(QIcon(":/materials/icons/toggle/ic_check_box_24px.png"), iconColor());
    return ic;
}

const QIcon& redDeleteIcon()
{
    static const QIcon ic = changeColor(deleteIcon(), errorColor());
    return ic;
}

const QIcon& upperIcon()
{
    static const QIcon ic = changeColor(rotateIcon(backIcon(), 90), iconColor());
    return ic;
}

const QIcon& lowerIcon()
{
    static const QIcon ic = rotateIcon(upperIcon(), 180);
    return ic;
}

const QIcon& undoIcon()
{
    static const QIcon ic(":/resources/icons/undo.png");
    return ic;
}

const QIcon& redoIcon()
{
    static const QIcon ic(":/resources/icons/redo.png");
    return ic;
}

QString titleCellCSS()
{
    const QString titleCellCSS{"background-color: %1; color: %2"};
    return titleCellCSS.arg(backgroundColor().lighter().name()).arg(titleColor().name());
}

QString progressBarCSS()
{
    const QString progressBarCSS{"QProgressBar {"
                                 "background-color: %1;"
                                 "color: %2;"   // differs
                                 "border-style: none;"
                                 "border-radius: %6px;"
                                 "text-align: center;"
                                 "font-size: %3;"
                                 "}"
                                 "QProgressBar::disabled {"
                                 "background-color: %1;"
                                 "color: %7;"   // differs
                                 "border-style: none;"
                                 "border-radius: %6px;"
                                 "text-align: center;"
                                 "font-size: %3;"
                                 "}"
                                 "QProgressBar::chunk {"
                                 "border-radius: %6px;"
                                 "background-color:"
                                 "qlineargradient(x1:0.5, y1:0, x2:0.5, y2:1,"
                                 "stop:0 %4, stop:0.7 %5, stop:0.92 %5, stop:1 %4);"   // differs
                                 "}"
                                 "QProgressBar::chunk::disabled {"
                                 "border-radius: %6px;"
                                 "background-color:"
                                 "qlineargradient(x1:0.5, y1:0, x2:0.5, y2:1,"
                                 "stop:0 %7, stop:0.7 %8, stop:0.92 %8, stop:1 %7);"   // differs
                                 "}"};
    return progressBarCSS.arg(backgroundColor().name())
      .arg(textColor().name())
      .arg(11)
      .arg(iconColor().name())
      .arg(foregroundColor().lighter().name())
      .arg(3)
      .arg(disabledColor().lighter().name())
      .arg(disabledColor().name());
}

const QIcon& muteIcon()
{
    static const QIcon ic(":/resources/icons/mute.png");
    return ic;
}

}   // namespace theme
