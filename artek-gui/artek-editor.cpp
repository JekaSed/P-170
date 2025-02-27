#include "artek-editor.h"
#include "artek-widget.h"
#include "qjsonobject.h"
#include "src/scheme-saver/scheme-saver.h"
#include <src/menu-button/menu-button.h>
#include <src/dialog/text-dialog.h>
#include <QApplication>
#include <QTimer>
#include <qtmaterialtoggle.h>

namespace {
const QString title{"Радиостанция Р170-П"};

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

class ArtekRadioWidget : public RadioDataWidget
{
    Q_OBJECT
public:
    ArtekRadioWidget(QWidget* parent = nullptr)
      : RadioDataWidget(parent)
      , m_artekWidget(new ArtekWidget(this))
      , m_schemeSaver(new SchemeSaver(this))
    {
        //        setObjectName("ArtekRadioWidget");
        //        setStyleSheet(QString("QWidget#ArtekRadioWidget {background-color: %1}").arg(theme::backgroundColor().name()));
        //        setAttribute(Qt::WA_StyledBackground);
        auto* lay = new QVBoxLayout(this);
        lay->addWidget(m_artekWidget, 0, Qt::AlignTop);
        //        lay->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));

        connect(m_artekWidget, &ArtekWidget::changed, this, [this](auto& o) {
            qDebug() << "changed" << o;
            emit documentChanged(currentDocument());
        });

        connect(m_schemeSaver, &SchemeSaver::filePathChanged, this, &ArtekRadioWidget::fileNameChanged);
    }

public slots:
    bool open()
    {
        bool ok;
        auto fileDoc = m_schemeSaver->importFromFile(ok);
        if (ok) {
            m_artekWidget->fromJsonObj(fileDoc.object());
            return true;
        }
        return false;
    }

    bool saveAs()
    {
        return m_schemeSaver->saveAs(QJsonDocument(m_artekWidget->toJsonObj()));
    }

signals:
    void savingError();
    void fileNameChanged(const QString);

private:
    ArtekWidget* m_artekWidget;
    SchemeSaver* m_schemeSaver;

    // RadioDataWidget interface
public:
    [[nodiscard]] QString dataName() const noexcept override
    {
        return title;
    }

    [[nodiscard]] QJsonDocument currentDocument() const noexcept override
    {
        return QJsonDocument{m_artekWidget->toJsonObj()};
    }

public slots:
    void updateFromDocument(QJsonDocument doc) override
    {
        m_artekWidget->fromJsonObj(doc.object());
    }

    void saveCurrentDocument() override
    {
        if (!m_schemeSaver->saveToFile(QJsonDocument(m_artekWidget->toJsonObj()))) {
            emit savingError();
            QTimer::singleShot(100, [this]() {
                emit documentChanged(currentDocument());
            });
        };
    }
};

ArtekEditor::ArtekEditor(QWidget* parent)
  : RadioDataEditor{title, parent}
  , m_radioWidget(new ArtekRadioWidget(this))
{
    auto* saveAsBt = makeControl(theme::saveAsIcon(), QString("Сохранить как\n(Ctrl+Shift+S)"), QKeySequence::SaveAs);
    auto* openBt = makeControl(theme::openFileIcon(), QString("Открыть\n(Ctrl+O)"), QKeySequence::Open);

    this->titleLayout()->insertWidget(0, saveAsBt);
    this->titleLayout()->insertWidget(0, openBt);

    connect(saveAsBt, &QPushButton::clicked, this, [this]() {
        if (m_radioWidget->saveAs())
            resetLocalStorage();
    });
    connect(openBt, &QPushButton::clicked, this, [this]() {
        if (m_radioWidget->open())
            resetLocalStorage();
    });
    connect(m_radioWidget, &ArtekRadioWidget::savingError, this, [this]() {
        ErrorDialog::showError("Документ не сохранен", "", this);
    });

    connect(m_radioWidget, &ArtekRadioWidget::fileNameChanged, this, &ArtekEditor::fileChanged);

    this->setRadioDataWidget(m_radioWidget);
}

void ArtekEditor::selectScheme()
{
    if (!m_firstStart) {
        return;
    }
    m_firstStart = false;
    const auto lastEditDoc = loadLastDocument();
    if (!lastEditDoc.isEmpty()) {
        {
            if (QuestionDialog::showQuestion("Найдены данные предыдущей сессии", "Что необходимо выполнить?", this,
                                             QPair<QString, QString>{"Загрузить данные", "Начать новую сессию"})) {
                m_radioWidget->updateFromDocument(lastEditDoc);
                return;
            }
        }
    }
    resetLocalStorage();
}

void ArtekEditor::importScheme()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    m_radioWidget->open();
    QApplication::restoreOverrideCursor();
}

bool ArtekEditor::closeRequest()
{
    if (hasUnsavedChanges()) {
        if (!QuestionDialog::showQuestion("Есть несохраненные изменения", "Выйти без сохранения?", this)) {
            return true;
        }
    }
    //    resetLocalStorage();
    return false;
}

bool ArtekEditor::event(QEvent* event)
{
    if (event->type() == QEvent::WindowActivate) {
        selectScheme();
    }
    return QObject::event(event);
}

#include "artek-editor.moc"
