#ifndef SCHEMESAVER_H
#define SCHEMESAVER_H

#include <QWidget>

class SchemeSaver : public QWidget
{
    Q_OBJECT
public:
    explicit SchemeSaver(QWidget* parent = nullptr);

    QString filePath() const;
    void setFilePath(const QString& newFilePath);

public slots:
    bool saveToFile(const QJsonDocument& docToSave);
    bool saveAs(const QJsonDocument& docToSave);
    QJsonDocument importFromFile(bool& ok);

signals:
    void filePathChanged(const QString);

private:
    QString m_filePath;
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged FINAL)
};

#endif   // SCHEMESAVER_H
