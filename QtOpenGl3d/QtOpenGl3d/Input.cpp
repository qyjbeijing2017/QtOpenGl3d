#include "Input.h"
#include <QMouseEvent>
#include <QWidget>

Input::Input(QWidget *parent)
	: QWidget(parent)
{
	if (!Instance)
	{
		Instance = this;
	}
	this->grabKeyboard();
}

Input::Input()
{
	if (!Instance)
	{
		Instance = this;
	}
}

Input::~Input()
{

}

Input * Input::instance()
{
	if (!Instance)
	{
		Instance = new Input();
	}
	return Instance;
}

