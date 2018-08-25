#pragma once

#include <QtWidgets/QWidget>
#include "ui_QtOpenGl3d.h"

class QtOpenGl3d : public QWidget
{
	Q_OBJECT

public:
	QtOpenGl3d(QWidget *parent = Q_NULLPTR);

private:
	Ui::QtOpenGl3dClass ui;
};
