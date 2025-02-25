#include "editor.h"
#include "src/menu-button/menu-button.h"
#include <src/slow-shower/slow-shower.h>
#include <qboxlayout.h>
#include <qicon.h>
#include <qkeysequence.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qsettings.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qwidget.h>

namespace {

const QString organizationName{"НПО завод Волна"};
const QString applicationName{"artek-editor"};
const QString docSettingsName{"data"};

QPushButton* makeControl(const QIcon& icon, const QString& toolTip, const QKeySequence& seq)
{
    auto* bt = new MenuButton(icon);
    bt->setToolTip(toolTip);
    bt->setShortcut(seq);
    bt->setCheckable(false);
    bt->setBadgeCircleVisible(false);
    constexpr auto iconSize = QSize(25, 25);
    bt->setIconSize(iconSize);
    return bt;
}

}   // namespace

RadioDataEditor::RadioDataEditor(const QString& title, QWidget* p)
  : Cloud(title, p)
  , m_controlWidget(new SlowShower(this))
  , m_undoBt(makeControl(theme::undoIcon(), "Отменить изменения\n(Ctrl+Z)", QKeySequence::Undo))
  , m_redoBt(makeControl(theme::redoIcon(), "Повторить изменения\n(Ctrl+Y)", QKeySequence(Qt::CTRL | Qt::Key_Y)))
  , m_saveBt(makeControl(theme::saveFileIcon(), "Сохранить изменения\n(Ctrl+S)", QKeySequence::Save))
{
    m_saveBt->setObjectName("saveBt");
    m_undoBt->setObjectName("undoBt");
    m_redoBt->setObjectName("redoBt");
    connect(m_undoBt, &QPushButton::clicked, this, &RadioDataEditor::makeCtrZ);
    connect(m_saveBt, &QPushButton::clicked, this, &RadioDataEditor::makeCtrS);
    connect(m_redoBt, &QPushButton::clicked, this, &RadioDataEditor::makeCtrY);

    m_undoBt->setEnabled(false);
    m_saveBt->setEnabled(false);
    m_redoBt->setEnabled(false);

    m_controlWidget->setStyleSheet("background:transparent;");
    constexpr auto controlSpacing{9};
    m_controlWidget->setFixedSize(m_undoBt->iconSize().width() * 3 + controlSpacing * 3, m_undoBt->iconSize().height());
    auto cLay = new QHBoxLayout(m_controlWidget);
    cLay->setContentsMargins(0, 0, 0, 0);
    cLay->setSpacing(controlSpacing);
    cLay->addWidget(m_saveBt);
    cLay->addWidget(m_undoBt);
    cLay->addWidget(m_redoBt);
    m_controlWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    hideControls();

    titleLayout()->insertWidget(0, m_controlWidget);
}

QJsonDocument RadioDataEditor::loadLastDocument() const
{
    QSettings settings(organizationName, applicationName);
    settings.beginGroup(m_radioTitle);
    auto docFromLocalStorage = settings.value(docSettingsName, QJsonDocument{}).toJsonDocument();
    settings.endGroup();
    return docFromLocalStorage;
}

void RadioDataEditor::setRadioDataWidget(RadioDataWidget* rd)
{
    if (rd == nullptr) {
        return;
    }
    if (m_radioData != nullptr) {
        layout()->removeWidget(m_radioData);
        m_radioData->deleteLater();
    }
    m_radioData = rd;
    m_radioTitle = m_radioData->dataName();
    m_titleLb->setText(m_radioTitle);

    const auto unsavedDoc = loadLastDocument();
    if (!unsavedDoc.isEmpty()) {
        const QSignalBlocker block(m_radioData);
        m_radioData->updateFromDocument(unsavedDoc);
        showControls();
        m_titleLb->setText(m_radioTitle + " *");
        m_saveBt->setEnabled(true);
    }

    m_history.setInitialDoc(m_radioData->currentDocument());
    connect(m_radioData, &RadioDataWidget::documentChanged, this, &RadioDataEditor::updateHistory);

    layout()->addWidget(m_radioData);
}

void RadioDataEditor::hideControls()
{
    m_controlWidget->hide();
}

void RadioDataEditor::showControls()
{
    m_controlWidget->show();
}

void RadioDataEditor::resetLocalStorage()
{
    saveTempActiveDoc({});
    m_history.setInitialDoc(m_radioData->currentDocument());
    checkControls();
    hideControls();
    m_titleLb->setText(m_radioTitle);
}

bool RadioDataEditor::hasUnsavedChanges() const noexcept
{
    return m_saveBt->isVisible() && m_saveBt->isEnabled();
}

void RadioDataEditor::checkControls()
{
    m_undoBt->setEnabled(m_history.undoSize() > 0);
    m_saveBt->setEnabled(m_undoBt->isEnabled());
    if (m_saveBt->isEnabled()) {
        m_titleLb->setText(m_radioTitle + " *");
    } else {
        m_titleLb->setText(m_radioTitle);
        saveTempActiveDoc({});
    }

    static_cast<MenuButton*>(m_undoBt)->setBadgeNum(m_history.undoSize());

    m_redoBt->setEnabled(m_history.redoSize() > 0);
    static_cast<MenuButton*>(m_redoBt)->setBadgeNum(m_history.redoSize());
}

void RadioDataEditor::makeCtrZ()
{
    if (m_radioData == nullptr || m_history.undoSize() == 0) {
        return;
    }
    qDebug() << m_radioTitle << "отмена изменений";
    const auto doc = m_history.ctrlZ();
    saveTempActiveDoc(doc);
    const QSignalBlocker block(m_radioData);
    m_radioData->updateFromDocument(doc);

    checkControls();
}

void RadioDataEditor::makeCtrY()
{
    if (m_radioData == nullptr || m_history.redoSize() == 0) {
        return;
    }
    qDebug() << m_radioTitle << "повтор изменений";
    const auto doc = m_history.ctrlY();
    saveTempActiveDoc(doc);
    const QSignalBlocker block(m_radioData);
    m_radioData->updateFromDocument(doc);

    checkControls();
}

void RadioDataEditor::makeCtrS()
{
    if (m_radioData == nullptr) {
        return;
    }
    qDebug() << m_radioTitle << "сохранение изменений";
    m_radioData->saveCurrentDocument();
    m_history.setInitialDoc(m_radioData->currentDocument());
    checkControls();
    hideControls();
}

void RadioDataEditor::saveTempActiveDoc(const QJsonDocument& doc)
{
    QSettings settings(organizationName, applicationName);
    settings.beginGroup(m_radioTitle);
    settings.setValue(docSettingsName, doc);
    settings.endGroup();
}

void RadioDataEditor::updateHistory(const QJsonDocument& doc)
{
    showControls();
    saveTempActiveDoc(doc);

    m_history.push(doc);
    checkControls();
}
