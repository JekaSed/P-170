#include "qtmaterialcombobox.h"
#include "qtmaterialcombobox_p.h"
#include <QtWidgets/QApplication>
#include <QPainter>
#include "qtmaterialcombobox_internal.h"
#include "lib/qtmaterialstyle.h"
#include <QFocusEvent>
#include <QKeyEvent>
#include <QDebug>
#include <algorithm>
#include <qabstractitemmodel.h>
#include <qaction.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdialog.h>
#include <qframe.h>
#include <qglobal.h>
#include <qlayout.h>
#include <qline.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qpolygon.h>
#include <qpushbutton.h>
#include <qsize.h>
#include <qstylepainter.h>
#include <qwidget.h>
#include "src/menu/contextmenu.h"

/*!
 *  \class QtMaterialTextFieldPrivate
 *  \internal
 */
namespace {
constexpr int OffsetBetweenLabel = 10;
constexpr double PercentWightTriangle = 0.10;
constexpr double PercentHeightTriangle = 0.45;

QPointF calcPointOnLine(QLineF line, qreal percent)
{
    auto downPos = line.p1().y() + line.dy() * percent;
    QLineF intersectLine;
    intersectLine.setP1({line.p1().x(), downPos});
    intersectLine.setP2({line.p1().x() + (line.dx()), downPos});
    QPointF x;
    line.intersects(intersectLine, &x);
    return x;
}

QPolygonF createTriangle(const QRectF& rect)
{
    QPolygonF polygon;
    auto p = rect.center();
    p.setY(rect.bottom());
    polygon << rect.topLeft() << rect.topRight() << p;
    return polygon;
}

}   // namespace

QtMaterialComboBoxPrivate::QtMaterialComboBoxPrivate(QtMaterialComboBox* q)
  : q_ptr(q)
{}

QtMaterialComboBoxPrivate::~QtMaterialComboBoxPrivate()
{}

void QtMaterialComboBoxPrivate::init()
{
    Q_Q(QtMaterialComboBox);

    m_menu = new ContextMenu(q);
    m_menu->setFont(q->font());
    m_menu->setBorderRadius(4);

    stateMachine = new QtMaterialComboBoxStateMachine(m_menu, q);
    label = nullptr;
    labelFontSize = 9.5;
    useThemeColors = true;
    ignoreWheel = false;

    q->setFrame(false);
    q->setStyle(&QtMaterialStyle::instance());
    q->setAttribute(Qt::WA_Hover);
    q->setMouseTracking(true);

    q->setFont(QFont("Roboto", 11, QFont::Normal));

    stateMachine->start();
}

/*!
 *  \class QtMaterialTextField
 */

QtMaterialComboBox::QtMaterialComboBox(QWidget* parent)
  : QComboBox(parent)
  , d_ptr(new QtMaterialComboBoxPrivate(this))
{
    d_func()->init();
}

QtMaterialComboBox::~QtMaterialComboBox()
{}

void QtMaterialComboBox::setUseThemeColors(bool value)
{
    Q_D(QtMaterialComboBox);

    if (d->useThemeColors == value) {
        return;
    }

    d->useThemeColors = value;
    d->stateMachine->setupProperties();
}

bool QtMaterialComboBox::useThemeColors() const
{
    Q_D(const QtMaterialComboBox);

    return d->useThemeColors;
}

void QtMaterialComboBox::setLabelFontSize(qreal size)
{
    Q_D(QtMaterialComboBox);

    d->labelFontSize = size;

    if (d->label != nullptr) {
        QFont font(d->label->font());
        font.setPointSizeF(size);
        d->label->setFont(font);
        d->label->update();
    }
}

qreal QtMaterialComboBox::labelFontSize() const
{
    Q_D(const QtMaterialComboBox);

    return d->labelFontSize;
}

QString QtMaterialComboBox::labelText() const
{
    Q_D(const QtMaterialComboBox);
    if (d->label == nullptr) {
        return {};
    }
    return d->label->text();
}

void QtMaterialComboBox::setLabel(const QString& label)
{
    Q_D(QtMaterialComboBox);

    if (d->label == nullptr) {
        d->label = new QtMaterialComboBoxLabel(this);
        d->stateMachine->setLabel(d->label);
        d->label->show();
    }

    if (!label.isEmpty()) {
        setStyleSheet("QComboBox { margin-top: 23;}");
    } else {
        setStyleSheet("QComboBox { margin-top: 0;}");
    }
    d->label->setLabel(label);
    update();
}

void QtMaterialComboBox::setTextColor(const QColor& color)
{
    Q_D(QtMaterialComboBox);

    d->textColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    d->stateMachine->setupProperties();
}

QColor QtMaterialComboBox::textColor() const
{
    Q_D(const QtMaterialComboBox);

    if (d->useThemeColors || !d->textColor.isValid()) {
        return QtMaterialStyle::instance().themeColor("text");
    }
    return d->textColor;
}

void QtMaterialComboBox::setLabelColor(const QColor& color)
{
    Q_D(QtMaterialComboBox);

    d->labelColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    d->stateMachine->setupProperties();
}

QColor QtMaterialComboBox::labelColor() const
{
    Q_D(const QtMaterialComboBox);

    if (d->useThemeColors || !d->labelColor.isValid()) {
        return QtMaterialStyle::instance().themeColor("accent3");
    }
    return d->labelColor;
}

void QtMaterialComboBox::setInkColor(const QColor& color)
{
    Q_D(QtMaterialComboBox);

    d->inkColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    d->stateMachine->setupProperties();
}

QColor QtMaterialComboBox::inkColor() const
{
    Q_D(const QtMaterialComboBox);

    if (d->useThemeColors || !d->inkColor.isValid()) {
        return QtMaterialStyle::instance().themeColor("primary1");
    }
    return d->inkColor;
}

void QtMaterialComboBox::setInputLineColor(const QColor& color)
{
    Q_D(QtMaterialComboBox);

    d->inputLineColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    d->stateMachine->setupProperties();
}

QColor QtMaterialComboBox::inputLineColor() const
{
    Q_D(const QtMaterialComboBox);

    if (d->useThemeColors || !d->inputLineColor.isValid()) {
        return QtMaterialStyle::instance().themeColor("border");
    }
    return d->inputLineColor;
}

void QtMaterialComboBox::setIgnoreWheel(const bool ignore)
{
    Q_D(QtMaterialComboBox);

    d->ignoreWheel = ignore;
}

QtMaterialComboBox::QtMaterialComboBox(QtMaterialComboBoxPrivate& d, QWidget* parent)
  : QComboBox(parent)
  , d_ptr(&d)
{
    d_func()->init();
}

bool QtMaterialComboBox::dataIsChanged() const
{
    const auto* q = d_func();
    const auto menuActions = q->m_menu->actions();
    if (menuActions.count() != model()->rowCount()) {
        return true;
    }
    for (int index = 0; index < model()->rowCount(); ++index) {
        // TODO: add icons changed check
        // itemIcon(index).cacheKey() != menuActions.at(index)->icon().cacheKey()
        if (itemText(index) != menuActions.at(index)->text()) {
            return true;
        }
        const auto toolTip = itemData(index, Qt::ToolTipRole);
        if (toolTip.isValid() && toolTip.toString() != menuActions.at(index)->toolTip()) {
            return true;
        }

        const auto itemFlags = model()->flags(model()->index(index, modelColumn()));
        const bool itemIsEnabled = itemFlags.testFlag(Qt::ItemIsEnabled);
        if (itemIsEnabled != menuActions.at(index)->isEnabled()) {
            return true;
        }

        if (itemFlags.testFlag(Qt::ItemIsUserCheckable)) {
            const bool itemIsChecked = itemData(index, Qt::CheckStateRole).toInt() == Qt::Checked;
            if (itemIsChecked != menuActions.at(index)->isChecked()) {
                return true;
            }
        }
    }
    return false;
}

void QtMaterialComboBox::updateItems()
{
    Q_D(QtMaterialComboBox);

    if (!dataIsChanged()) {
        return;
    }

    QList<QAction*> newItems;
    for (int index = 0; index < model()->rowCount(); ++index) {
        const auto name = itemText(index);
        auto* a = new QAction(itemIcon(index), name, this);

        const auto itemFlags = model()->flags(model()->index(index, modelColumn()));
        a->setCheckable(itemFlags.testFlag(Qt::ItemIsUserCheckable));
        const bool itemIsChecked = itemData(index, Qt::CheckStateRole).toInt() == Qt::Checked;
        a->setChecked(itemIsChecked);
        a->setEnabled(itemFlags.testFlag(Qt::ItemIsEnabled));
        a->setToolTip(itemData(index, Qt::ToolTipRole).toString());

        connect(a, &QAction::triggered, this, [this, name, index]() {
            emit activated(index);
            this->setCurrentText(name);
        });
        newItems.append(a);
    }
    d->m_menu->setActions(newItems);
}

/*!
 *  \reimp
 */
bool QtMaterialComboBox::event(QEvent* event)
{
    Q_D(QtMaterialComboBox);

    switch (event->type()) {
    case QEvent::Resize:
    case QEvent::Move: {
        if (d->label != nullptr) {
            d->label->setGeometry(rect());
        }
    }
    case QEvent::Wheel:
        if (d->ignoreWheel) {
            event->ignore();
            return false;
        }
        break;
    default:
        break;
    }
    return QComboBox::event(event);
}

void QtMaterialComboBox::showPopup()
{
    Q_D(QtMaterialComboBox);
    if (d->m_menu->isVisible() || count() == 0) {
        return;
    }

    updateItems();

    auto p = mapToGlobal(rect().bottomLeft());

    d->m_menu->setMinItemWidth(width());
    d->m_menu->showMenu(p);
}

void QtMaterialComboBox::paintEvent(QPaintEvent* /*e*/)
{
    Q_D(QtMaterialComboBox);

    QStylePainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const qreal progress = d->stateMachine->progress();
    const bool progressIsFinished = progress == 1;

    if (currentText().isEmpty() && progress < 1) {
        painter.setOpacity(1 - progress);
        painter.fillRect(rect(), parentWidget()->palette().color(backgroundRole()));
    }
    auto textRect = rect();
    const int offset = d->label == nullptr ? 0 : d->label->heightLabel() + OffsetBetweenLabel;
    textRect.setY(textRect.y() + offset);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(inputLineColor());
    painter.setPen(pen);
    QStyleOptionComboBox opt;
    initStyleOption(&opt);
    if (currentIndex() < 0 && !placeholderText().isEmpty()) {
        opt.palette.setBrush(QPalette::ButtonText, opt.palette.placeholderText());
        opt.currentText = placeholderText();
    }
    // draw the icon and text
    painter.drawControl(QStyle::CE_ComboBoxLabel, opt);

    // triangle draw
    if (isEnabled()) {
        const auto tw = textRect.width() * PercentWightTriangle;
        const auto th = textRect.height() * PercentHeightTriangle;
        const auto size = std::min(tw, th);
        const auto tx = textRect.x() + textRect.width() - size - 4;
        const auto triangleRect = QRectF(tx, textRect.y() + 4, size, size);
        const auto triangle = createTriangle(triangleRect);

        painter.setPen(Qt::NoPen);
        painter.setBrush(progressIsFinished ? inkColor() : Qt::white);

        painter.drawPolygon(triangle);

        if (!progressIsFinished) {
            QPolygonF fillTriangle;
            fillTriangle << triangle.at(1);
            fillTriangle << triangle.at(0);
            fillTriangle << calcPointOnLine(QLineF(triangle.at(0), triangle.last()), progress);
            fillTriangle << calcPointOnLine(QLineF(triangle.at(1), triangle.last()), progress);
            painter.setBrush(inkColor());
            painter.drawPolygon(fillTriangle);
        }
    }

    // input line
    const int inputLineY = height() - 1;
    const int wd = width() - 5;

    if (!isEnabled()) {
        pen.setStyle(Qt::DashLine);
    }

    painter.setPen(pen);
    painter.setOpacity(1);
    painter.drawLine(QLineF(2.5, inputLineY, wd, inputLineY));

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(inkColor());

    if (progress > 0) {
        painter.setPen(Qt::NoPen);
        painter.setBrush(brush);
        const int w = (1 - progress) * static_cast<qreal>(wd / 2);
        painter.drawRect(w + 3, height() - 2, wd - w * 2, 2);
    }
}
