#pragma once

#include <QObject>
#include <QWidget>
#include <QMutex>
#include <QReadWriteLock>
#include <QList>
#include <QMouseEvent>

class Input : public QWidget
{
	Q_OBJECT

public:
	static Input* instance();
	static void release();

	static QList<Qt::Key>* KeyDown;
	static QList<Qt::Key>* KeyUp;
	static QList<Qt::Key>* KeyHold;

	static bool GetKeyDown(Qt::Key key);
	static bool GetKeyHold(Qt::Key key);
	static bool GetKeyUp(Qt::Key key);

private:
	Input()
	{
		this->grabKeyboard();
	};
	Input(const Input&) {}
	Input& operator==(const Input&) {}
	~Input();

	static Input* m_instance;
	virtual void keyPressEvent(QKeyEvent *ev);
	virtual void keyReleaseEvent(QKeyEvent *ev);

public:
	void keyHold();

};

