#include "workmode-widget.h"
#include "src/workmode-data.h"

#include <src/combobox/qtmaterialcombobox.h>

#include <QJsonObject>
#include <QPainter>

namespace {
const QString title{"Класс излучения"};

const QStringList intListToStrings(const QList<int>& intList)
{
    QStringList r;
    for (const auto i : intList) {
        r.push_back(QString::number(i));
    }
    return r;
}

QComboBox* makeComboBox(const QString& label, const QStringList& items, QWidget* parent)
{
    auto* cb = new QtMaterialComboBox(parent);
    cb->setLabel(label);
    cb->addItems(items);
    return cb;
}

}   // namespace

WorkModeWidget::WorkModeWidget(QWidget* p)
  : Cloud(title, p, theme::foregroundColor_1())
  , m_data(new WorkModeData(this))
  , m_emissionCb(makeComboBox("Тип излучения", emissionTextList(), this))
  , m_deviationCb(makeComboBox("Девиация", {}, this))
  , m_bitrateCb(makeComboBox("Скорость", {}, this))
  , m_sideBandCb(makeComboBox("Боковая полоса", sideBandMap.keys(), this))
{
    setMinimumHeight(250);
    auto* mainLy = contentLayout();
    mainLy->setSpacing(15);

    mainLy->addWidget(m_emissionCb);
    mainLy->addWidget(m_deviationCb);
    mainLy->addWidget(m_bitrateCb);
    mainLy->addWidget(m_sideBandCb);
    mainLy->addSpacerItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding));

    m_emissionCb->setCurrentIndex(-1);
    m_deviationCb->hide();
    m_bitrateCb->hide();
    m_sideBandCb->hide();

    connect(m_emissionCb, &QComboBox::currentIndexChanged, this, [this](int i) {
        m_data->loadDefault(static_cast<EmissionType>(i));
    });

    connect(m_data, &WorkModeData::updateView, this, [this]() {
        m_emissionCb->setCurrentIndex(static_cast<int>(m_data->emission()));
        updateDeviations();
        updateBitrates();
        m_sideBandCb->setVisible(m_data->emission() == EmissionType::J3E);
        m_sideBandCb->setCurrentIndex(m_data->sideBand());
        emitChangedSignal();
    });

    connect(m_deviationCb, &QComboBox::currentIndexChanged, this, [this](int i) {
        if (i == -1) {
            m_data->setDeviation(-1);
            return;
        }
        const auto values = m_data->deviationList();
        Q_ASSERT(i >= 0 && i < values.size());
        m_data->setDeviation(values[i]);
    });

    connect(m_data, &WorkModeData::selectedDeviationChanged, this, [this](int dev) {
        m_deviationCb->setCurrentIndex(m_data->deviationList().indexOf(dev));
        updateBitrates();
        emitChangedSignal();
    });

    connect(m_data, &WorkModeData::selectedBitrateChanged, this, [this](int bRate) {
        m_bitrateCb->setCurrentIndex(m_data->bitrateListAvailable().indexOf(bRate));
        emitChangedSignal();
    });

    connect(m_bitrateCb, &QComboBox::currentIndexChanged, this, [this](int i) {
        if (i == -1) {
            m_data->setBitrate(-1);
            return;
        }
        const auto values = m_data->bitrateListAvailable();
        Q_ASSERT(i >= 0 && i < values.size());
        m_data->setBitrate(values[i]);
    });

    connect(m_sideBandCb, &QComboBox::currentIndexChanged, m_data, &WorkModeData::setSideBand);
    connect(m_data, &WorkModeData::sideBandChanged, this, [this](int i) {
        m_sideBandCb->setCurrentIndex(i);
        emitChangedSignal();
    });
}

QJsonObject WorkModeWidget::toJsonObj() const
{
    return m_data->toJsonObj();
}

void WorkModeWidget::fromJsonObj(const QJsonObject& obj)
{
    QSignalBlocker lock(this);
    QSignalBlocker lockDev(m_emissionCb);
    m_data->fromJsonObj(obj);
}

void WorkModeWidget::updateDeviations()
{
    QSignalBlocker lock(m_deviationCb);
    QList<int> values = m_data->deviationList();
    if (values.isEmpty()) {
        m_deviationCb->hide();
        m_deviationCb->setCurrentIndex(-1);
    } else {
        m_deviationCb->clear();
        m_deviationCb->addItems(intListToStrings(values));
        m_deviationCb->setCurrentIndex(values.indexOf(m_data->deviation()));
        m_deviationCb->show();
    }
}

void WorkModeWidget::updateBitrates()
{
    QSignalBlocker lock(m_bitrateCb);
    QList<int> values = m_data->bitrateListAvailable();
    if (values.isEmpty()) {
        m_bitrateCb->hide();
        m_bitrateCb->setCurrentIndex(-1);
    } else {
        m_bitrateCb->show();
        m_bitrateCb->clear();
        m_bitrateCb->addItems(intListToStrings(values));
        m_bitrateCb->setCurrentIndex(values.indexOf(m_data->bitrate()));
    }
}

void WorkModeWidget::emitChangedSignal()
{
    if (m_data->isValid()) {
        emit changed(toJsonObj());
    }
    update();
}

void WorkModeWidget::paintEvent(QPaintEvent* event)
{
    if (!m_data->isValid()) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        auto p = painter.pen();
        p.setWidth(2);
        p.setColor(Qt::red);
        painter.setPen(p);
        painter.drawRoundedRect(this->rect(), 15, 15);
    }
    QWidget::paintEvent(event);
}
