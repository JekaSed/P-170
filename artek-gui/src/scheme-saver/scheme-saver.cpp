#include "scheme-saver.h"

#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>

namespace {
QJsonDocument readJsonFile(QString path)
{
    QFile file;
    QByteArray fileContains;
    QJsonDocument doc;

    file.setFileName(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    fileContains = file.readAll();

    doc = QJsonDocument::fromJson(fileContains);
    file.close();

    return doc;
}

bool writeJsonFile(QJsonDocument doc, QString name)
{
    QFile file(name);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QByteArray data = doc.toJson();
    file.resize(0);
    file.write(data);
    file.close();
    return true;
}
}   // namespace

SchemeSaver::SchemeSaver(QWidget* parent)
  : QWidget{parent}
{}

bool SchemeSaver::saveToFile(const QJsonDocument& docToSave)
{
    QString selectedFile{m_filePath};
    if (m_filePath.isEmpty()) {
        selectedFile = QApplication::applicationDirPath() + QString("/default.json");
        selectedFile = QFileDialog::getSaveFileName(this, tr("Сохранить схему"), m_filePath, "JSON (*.json)");
    }

    if (selectedFile.isNull() || selectedFile.isEmpty()) {
        return false;
    }
    setFilePath(selectedFile);
    return writeJsonFile(docToSave, m_filePath);
}

bool SchemeSaver::saveAs(const QJsonDocument& docToSave)
{
    auto prevFileName = m_filePath;
    setFilePath({});
    if (!saveToFile(docToSave)) {
        setFilePath(prevFileName);
        return false;
    }
    return true;
}

QJsonDocument SchemeSaver::importFromFile(bool& ok)
{
    QString selectedFile;
    selectedFile = QApplication::applicationDirPath() + QString("/default.json");
    selectedFile = QFileDialog::getOpenFileName(this, tr("Открыть схему"), m_filePath, "JSON (*.json)");
    if (selectedFile.isEmpty()) {
        ok = false;
        return QJsonDocument();
    }
    setFilePath(selectedFile);
    ok = true;
    return readJsonFile(m_filePath);
}

QString SchemeSaver::filePath() const
{
    return m_filePath;
}

void SchemeSaver::setFilePath(const QString& newFilePath)
{
    if (m_filePath == newFilePath)
        return;
    m_filePath = newFilePath;
    emit filePathChanged(newFilePath);
}
