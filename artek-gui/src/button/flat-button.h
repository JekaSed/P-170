#pragma once

#include <QAbstractButton>
#include <QIcon>
#include <QWidget>

QAbstractButton* makeFlatButton(const QString& text, const QIcon& icon = {}, QWidget* parent = nullptr);
