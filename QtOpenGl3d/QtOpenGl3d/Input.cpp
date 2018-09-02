#include "Input.h"
#include <QMouseEvent>
#include <QWidget>

Input* Input::m_instance = NULL;
QList<Qt::Key>* Input::KeyDown = NULL;
QList<Qt::Key>* Input::KeyUp = NULL;
QList<Qt::Key>* Input::KeyHold = NULL;


Input* Input::instance() {
	if (!m_instance) 
	{
		m_instance = new Input;
	}		
	if (!KeyDown) {
			KeyDown = new QList<Qt::Key>();
	}
	if (!KeyUp)
	{
		KeyUp = new QList<Qt::Key>();
	}
	if (!KeyHold)
	{
		KeyHold = new QList<Qt::Key>();
	}
	return m_instance;
}


void Input::release()
{
	if (!m_instance)
	{
		delete m_instance;
		delete KeyDown;
		delete KeyUp;
		delete KeyHold;
		m_instance = NULL;
		KeyDown = NULL;
		KeyUp = NULL;
		KeyHold = NULL;
	}
}

bool Input::GetKeyDown(Qt::Key key)
{
	if (KeyDown->contains(key))
	{
		return true;
	}
	return false;
}

bool Input::GetKeyHold(Qt::Key key)
{
	if (KeyHold->contains(key))
	{
		return true;
	}
	return false;
}

bool Input::GetKeyUp(Qt::Key key)
{
	if (KeyUp->contains(key))
	{
		return true;
	}
	return false;
}

Input::~Input()
{
	m_instance->release();
}

void Input::keyPressEvent(QKeyEvent * ev)
{
	if (!ev->isAutoRepeat())
	{
		if (!KeyDown->contains((Qt::Key)ev->key()) && !KeyHold->contains((Qt::Key)ev->key()))
		{
			KeyDown->append((Qt::Key)ev->key());
		}
	}
}

void Input::keyReleaseEvent(QKeyEvent * ev)
{
	if (!ev->isAutoRepeat())
	{
		if (KeyDown->contains((Qt::Key)ev->key()))
		{
			KeyDown->removeAll((Qt::Key)ev->key());
		}
		if (KeyHold->contains((Qt::Key)ev->key()))
		{
			KeyHold->removeAll((Qt::Key)ev->key());
		}
		if (!KeyUp->contains((Qt::Key)ev->key()))
		{
			KeyUp->append((Qt::Key)ev->key());
		}
	}
}

void Input::keyHold()
{
	for (int i = 0; i < KeyDown->size(); i++)
	{
		if (!KeyHold->contains(KeyDown->at(i)))
		{
			KeyHold->append(KeyDown->at(i));
		}
	}
	KeyUp->clear();
	KeyDown->clear();
}
