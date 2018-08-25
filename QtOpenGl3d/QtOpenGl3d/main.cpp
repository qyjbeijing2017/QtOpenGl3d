#include "QtOpenGl3d.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QtOpenGl3d w;
	w.show();
	return a.exec();
}
