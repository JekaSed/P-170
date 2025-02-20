#pragma once

#include <QAbstractButton>
#include <qglobal.h>

QAbstractButton* makeExpandButton(QPair<qreal, qreal> degrees = {0.0, 180.0}, QWidget* parent = nullptr);
