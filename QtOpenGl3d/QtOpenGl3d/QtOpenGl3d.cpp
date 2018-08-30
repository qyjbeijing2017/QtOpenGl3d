#include "QtOpenGl3d.h"
#include "OpenGLWidget.h"

QtOpenGl3d::QtOpenGl3d(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_openGLWidget = ui.openGLWidget;
	//m_openGLWidget->window = this;
	connect(m_openGLWidget, SIGNAL(debugLog(QString)), this, SLOT(myShaderLog(QString)));



}

void QtOpenGl3d::myShaderLog(QString log)
{
	ui.textBrowser->append(log);
}


