#pragma once

#include <QColor>
#include <QPixmap>
#include <QIcon>
#include <QSize>
#include <QFont>
#include <QString>
#include <qicon.h>
#include <qsize.h>

namespace theme {

QString mainCSS();
QString titleCellCSS();
QString progressBarCSS();

QColor backgroundColor();
QColor foregroundColor();
QColor topMenuColor();
QColor errorColor();
QColor disabledColor();
QColor textColor();
QColor tableHeader();
QColor placeHolderColor();

QColor iconColor();
QColor iconHighLightColor();
QColor titleColor();

QColor mixColors(QColor c1, QColor c2);
QIcon changeColor(const QIcon& icon, QColor newColor, QSize size = {40, 40});
QIcon rotateIcon(const QIcon& icon, int angle, QSize size = {40, 40});

QIcon makeIcon(QSize size, QColor color, int radius = 5);
// корректировка цвета
QColor correctTo(QColor src, QColor dst);
/**
* @brief осветленный цвет по уровню
*/
QColor lighterColor(const QColor& col, const int level);
QColor foregroundColor_0();
QColor foregroundColor_1();
QColor foregroundColor_2();
QColor foregroundColor_3();
QColor foregroundColor_4();

const QIcon& backIcon();
const QIcon& nextIcon();
const QIcon& editIcon();
const QIcon& settingsIcon();
const QIcon& deleteIcon();
const QIcon& redDeleteIcon();
const QIcon& plusIcon();
const QIcon& copyIcon();
const QIcon& upperIcon();
const QIcon& lowerIcon();
const QIcon& undoIcon();
const QIcon& redoIcon();

const QIcon& openFileIcon();
const QIcon& saveFileIcon();
const QIcon& activeIcon();
const QIcon& closeIcon();
const QIcon& sendIcon();

const QIcon& loadIcon();
const QIcon& uploadIcon();

const QIcon& buttonUncheckedIcon();
const QIcon& buttonCheckedIcon();
const QIcon& muteIcon();

const QFont& infoFont();
const QFont& baseFont();
const QFont& digitalFont();

}   // namespace theme
