#pragma once

#include "src/gsl/pointers"

#include <QScrollBar>

gsl::owner<QScrollBar*> makeScroll(Qt::Orientation o, QWidget* p);
