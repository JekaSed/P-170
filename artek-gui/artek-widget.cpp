#include "artek-widget.h"
#include "qboxlayout.h"

#include <QLabel>

ArtekWidget::ArtekWidget(QWidget* parent)
  : QWidget{parent}
{
    auto mainLy = new QHBoxLayout(this);
    mainLy->addWidget(new QLabel("Р-170П", this), 0, Qt::AlignCenter);
}
