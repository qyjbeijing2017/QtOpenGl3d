#pragma once

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QBasicTimer>
#include <QMatrix4x4>


class QOpenGLShaderProgram;
class QOpenGLTexture;
class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	OpenGLWidget(QWidget *parent = 0);
	~OpenGLWidget();

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void timerEvent(QTimerEvent *e);

private:
	QBasicTimer timer;
	QOpenGLShaderProgram * program;
	QOpenGLBuffer vbo;
	QOpenGLBuffer* ebo;
	QOpenGLTexture* ourTexture;
	QOpenGLTexture* ourTexture1;
	QMatrix4x4* modelMatrix;
	QMatrix4x4* viewMatrix;
	QMatrix4x4* projectMatrix;

	void creatShader(QString vertexShader, QString fragmentShader);

	static QOpenGLTexture* InitTexture(const QString imagePath);

signals:
	void debugLog(QString log);

};
