#include "contextmenu.h"
#include <QMouseEvent>
#include <QPainter>
#include <QPixmap>
#include <QStyle>
#include <QStyleOption>
#include <qcoreevent.h>
#include <qevent.h>
#include <qkeysequence.h>
#include <qlayout.h>
#include <qnamespace.h>
#include <qpropertyanimation.h>
#include <qpushbutton.h>
#include <qshortcut.h>
#include <qwidget.h>
#include "src/scroll-bar/scroll-bar.h"
#include "src/theme/theme.h"

namespace {
constexpr auto heightButton = 50;
constexpr auto heightMenu = 350;

void clearLay(QLayout* lay)
{
    if (lay == nullptr) {
        return;
    }
    QLayoutItem* child{};
    while ((child = lay->takeAt(0)) != nullptr) {
        if (auto* w = child->widget()) {
            w->deleteLater();
        }
        clearLay(child->layout());
        delete child;   // delete the layout item
    }
}

QWidget* findParentWidget(QWidget* childWidget)
{
    QWidget* p = childWidget->parentWidget();
    while (p != nullptr) {
        childWidget = p;
        p = p->parentWidget();
    }
    return childWidget;
}

class ActionMenuButton final : public QPushButton
{
public:
    ActionMenuButton(const QIcon& icon, const QString& text, QWidget* parent = nullptr)
      : QPushButton(icon, text, parent)
    {}

    void paintEvent(QPaintEvent* e) override
    {
        QPushButton::paintEvent(e);
        auto btShortCut = shortcut().toString(QKeySequence::NativeText);
        if (!btShortCut.isEmpty()) {
            btShortCut.append(' ');
            QPainter p(this);
            auto r = contentsRect();
            p.drawText(r, Qt::AlignVCenter | Qt::AlignRight, btShortCut);
        }
    }
};

}   // namespace

ContextMenu::ContextMenu(QWidget* parent)
  : QWidget(parent)
  , m_mainLy(new QVBoxLayout(this))
  , m_btLy(new QVBoxLayout)
  , m_scroll(new QScrollArea(this))
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::Popup);
    m_btLy->setContentsMargins(5, 1, 1, 1);
    m_btLy->setSpacing(0);
    setObjectName("ContextMenu");
    setBorderRadius(m_radius);

    // setStyleSheet(QString("QWidget#ContextMenu{border: 1px solid black; border-radius: 15px; background:%1; }")
    //                 .arg(theme::foregroundColor().lighter().name()));
    setAttribute(Qt::WA_TranslucentBackground);

    defineEventFilter();

    m_scroll->setWidgetResizable(true);
    auto* content = new QWidget(this);
    content->setStyleSheet("background: transparent;");
    m_scroll->setObjectName("ContextMenu");

    // m_btLy->setSizeConstraint(QLayout::SetFixedSize);
    content->setLayout(m_btLy);
    m_scroll->setWidget(content);
    m_scroll->setVerticalScrollBar(makeScroll(Qt::Vertical, m_scroll));
    m_scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_mainLy->addWidget(m_scroll);
    m_mainLy->setContentsMargins(0, 0, 0, 0);
    m_mainLy->setSpacing(0);

    m_animateToggle.setTargetObject(this);
    m_animateToggle.setPropertyName("geometry");
    m_animateToggle.setDuration(200);

    connect(&m_animateToggle, &QPropertyAnimation::finished, this, [this]() {
        if (m_animateToggle.direction() == QPropertyAnimation::Backward) {
            hide();
            emit hided();
        } else {
            emit showed();
            m_scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
    });
}

ContextMenu::~ContextMenu()
{
    removeEventFilter();
}

void ContextMenu::addMenuItem(QAction* action)
{
    auto* bt = new ActionMenuButton(action->icon(), action->text(), this);
    bt->setCheckable(action->isCheckable());
    bt->setChecked(action->isChecked());
    bt->setEnabled(action->isEnabled());
    bt->setIconSize(QSize{25, 25});
    bt->setCursor(Qt::PointingHandCursor);
    bt->setToolTip(action->toolTip());
    bt->setShortcut(action->shortcut());
    action->setShortcut({});   // move shortcut from action to button

    // bt->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    bt->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    const auto btStyle =
      QString("QPushButton{color:%1; text-align: left; background:transparent; border-radius: %2;}"
              "QPushButton:hover{background:%3;color:white} QPushButton:disabled{background: %4;color:gray;}")
        .arg(theme::textColor().name())
        .arg(QString::number(m_radius))
        .arg(theme::iconColor().darker().name())
        .arg(theme::disabledColor().name());
    bt->setStyleSheet(btStyle);
    bt->setFont(this->font());
    bt->setMinimumSize(m_minItemWidth, m_minItemHeight);
    m_btLy->addWidget(bt);

    connect(action, &QAction::enabledChanged, bt, &QPushButton::setEnabled);
    connect(bt, &QPushButton::clicked, this, [this, action, bt] {
        action->trigger();
        Q_UNUSED(bt);
#if (ANDROID)
        bt->setAttribute(Qt::WA_UnderMouse, false);
#endif
        closeRequest();
    });
}

void ContextMenu::setActions(const QList<QAction*>& actionsList)
{
    for (auto* a : actions()) {
        removeAction(a);
    }
    addActions(actionsList);
}

void ContextMenu::addActions(const QList<QAction*>& actionsList)
{
    QWidget::addActions(actionsList);
    clearLay(m_btLy);
    for (auto* action : this->actions()) {
        addMenuItem(action);
    }
}

void ContextMenu::setColor(const QColor& color)
{
    const auto styleSheet = QString("color: %1").arg(color.name());
    for (auto* pb : findChildren<QPushButton*>()) {
        pb->setStyleSheet(styleSheet);
    }
}

void ContextMenu::setFont(const QFont& font)
{
    QWidget::setFont(font);
    for (auto* pb : findChildren<QPushButton*>()) {
        pb->setFont(font);
    }
}

void ContextMenu::setMinItemHeight(int height)
{
    m_minItemHeight = height;
}

void ContextMenu::setMinItemWidth(int width)
{
    m_minItemWidth = width;
}

QPoint ContextMenu::calcZeroPoint(const QPoint& givenGlobalPosition)
{
    Q_ASSERT(m_mainWindow);
    QPoint givenPos = m_mainWindow->mapFromGlobal(givenGlobalPosition);

    QPoint newPos;
    QSize targetSize = minimumSizeHint();
    targetSize.setHeight(targetSize.height() > heightMenu ? heightMenu : targetSize.height() + 2);
    targetSize.setHeight(targetSize.height() < heightButton + 2 ? heightButton + 2 : targetSize.height());
    if (givenPos.y() + targetSize.height() > m_mainWindow->height()) {
        const QRect targetRect = QRect(QPoint(0, 0), targetSize);
        newPos = (givenPos - targetRect.bottomLeft());
        newPos.setY(newPos.y() - 3);
    } else {
        //top
        newPos = (givenPos /*- targetRect.topLeft()*/);
        newPos.setY(newPos.y() + 3);
    }
    const QPoint newGlobalPos = m_mainWindow->mapToGlobal(newPos);
    m_animateToggle.setStartValue(QRect(givenGlobalPosition, QSize(targetSize.width(), 0)));
    m_animateToggle.setEndValue(QRect(newGlobalPos, targetSize));
    return newGlobalPos;
}

void ContextMenu::updatePosition(const QPoint& menuGlobalPosition)
{
    saveMenuPos(menuGlobalPosition);
    this->layout()->update();
    this->layout()->activate();
    move(calcZeroPoint(menuGlobalPosition));
}

void ContextMenu::saveMenuPos(const QPoint& point)
{
    m_savedPoint = this->parentWidget()->mapFromGlobal(point);
}

QPoint ContextMenu::restoreMenuPos()
{
    return this->parentWidget()->mapToGlobal(m_savedPoint);
}

void ContextMenu::removeEventFilter()
{
    if (m_mainWindow != nullptr) {
        m_mainWindow->removeEventFilter(this);
    }
}

void ContextMenu::defineEventFilter()
{
    removeEventFilter();
    m_mainWindow = findParentWidget(this);
    m_mainWindow->installEventFilter(this);
}

void ContextMenu::slowHide()
{
    m_scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_animateToggle.setDirection(QPropertyAnimation::Backward);
    m_animateToggle.start();
}

void ContextMenu::slowShow()
{
    emit aboutToShow();
    m_animateToggle.setDirection(QPropertyAnimation::Forward);
    m_animateToggle.start();
}

void ContextMenu::closeRequest()
{
    if (!this->isVisible()) {
        return;
    }
    slowHide();
}

void ContextMenu::updateCurrentPosition()
{
    this->updatePosition(restoreMenuPos());
}

void ContextMenu::showMenu(const QPoint& menuGlobalPosition)
{
    updatePosition(menuGlobalPosition);
    this->show();
}

bool ContextMenu::event(QEvent* event)
{
    switch (event->type()) {
        //    case QEvent::Leave:
    case QEvent::WindowDeactivate:
        if (m_animateToggle.state() != QPropertyAnimation::Running) {
            closeRequest();
        }
        break;
    case QEvent::KeyPress:
        if (static_cast<QKeyEvent*>(event)->key() == Qt::Key_Escape) {
            closeRequest();
        }
        break;
    case QEvent::Show:
        slowShow();
        break;
    case QEvent::ParentChange:
        defineEventFilter();
        break;
    default:
        break;
    }
    return QWidget::event(event);
}

bool ContextMenu::eventFilter(QObject* watched, QEvent* event)
{
    switch (event->type()) {
    case QEvent::ParentChange:
        defineEventFilter();
        break;
    case QEvent::Resize:
    case QEvent::Move:
        updateCurrentPosition();
        break;
    default:
        break;
    }

    return QWidget::eventFilter(watched, event);
}

QSize ContextMenu::minimumSizeHint() const
{
    int width = m_minItemWidth;
    int itemHeight = m_minItemHeight;
    for (const auto* pb : findChildren<QPushButton*>()) {
        const QFontMetrics fm(pb->fontMetrics());
        auto shortcutText = pb->shortcut().toString(QKeySequence::NativeText);
        if (!shortcutText.isEmpty()) {
            shortcutText.append(' ');
        }
        const int requiredLength = fm.horizontalAdvance(pb->text() + shortcutText) + pb->iconSize().width() * 2;
        if (width < requiredLength) {
            width = requiredLength;
        }
        if (itemHeight < qMax(fm.height(), pb->iconSize().height())) {
            itemHeight = qMax(fm.height(), pb->iconSize().height());
        }
    }
    return QSize(width, actions().count() * itemHeight + 4);
}

void ContextMenu::closeEvent(QCloseEvent* event)
{
    event->ignore();
    closeRequest();
}

void ContextMenu::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
#if (ANDROID)
    //для андроида прозрачный цвет - прозрачный до рабочего стола. поэтому срисуем фон для меню с основного окна
    QPixmap backgroundPixmap = m_mainWindow->grab(QRect(m_mainWindow->mapToGlobal(this->pos()), this->size()));
    p.drawPixmap(this->rect(), backgroundPixmap);
#endif
    QStyleOption opt;
    opt.initFrom(this);
    p.setRenderHint(QPainter::Antialiasing);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ContextMenu::setBorderRadius(int radius)
{
    m_radius = radius;
    setStyleSheet(QString("QWidget#ContextMenu{background-color:%1; border-radius: %2px; border:1px solid "
                          "%4;} QPushButton{color:%3; text-align: left; background:transparent; border-radius: "
                          "%2px;} QPushButton:hover{background-color:%5; color:white;}")
                    .arg(theme::foregroundColor().name())
                    .arg(m_radius)
                    .arg(theme::textColor().name())
                    .arg(theme::iconColor().name())
                    .arg(theme::iconColor().darker().name()));
}
