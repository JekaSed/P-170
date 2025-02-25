#ifndef ARTEKEDITOR_H
#define ARTEKEDITOR_H

#include "src/history-manager/editor.h"
#include <QEvent>
#include <QWidget>

class ArtekEditor : public RadioDataEditor
{
    Q_OBJECT
public:
    explicit ArtekEditor(QWidget* parent = nullptr);

public slots:
    void selectScheme();
    void importScheme();
    bool closeRequest();

private:
    class ArtekRadioWidget* m_radioWidget;
    bool m_firstStart{true};

signals:
    void editionFinished();
    void fileChanged(const QString);

    // QObject interface
public:
    bool event(QEvent* event) override;
};

#endif   // ARTEKEDITOR_H
