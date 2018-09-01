#pragma once

#include <QObject>
#include <QWidget>

class Input : public QWidget
{
	Q_OBJECT

public:
	Input(QWidget *parent = Q_NULLPTR);
	Input();
	~Input();

	static Input* instance();

private:
	static Input* Instance;


};
