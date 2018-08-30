#pragma once

#include <QtWidgets/QWidget>
#include "ui_QtOpenGl3d.h"


class OpenGLWidget;
class QtOpenGl3d : public QWidget
{
	Q_OBJECT

public:
	QtOpenGl3d(QWidget *parent = Q_NULLPTR);

private:
	Ui::QtOpenGl3dClass ui;
	OpenGLWidget* m_openGLWidget;

public slots:
	void myShaderLog(const QString log);
};
