#pragma once

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>



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

private:
	QOpenGLShaderProgram * program;
	QOpenGLBuffer vbo;
	QOpenGLBuffer* ebo;
	QOpenGLTexture* ourTexture;
	QOpenGLTexture* ourTexture1;

	void creatShader(QString vertexShader, QString fragmentShader);

	static QOpenGLTexture* InitTexture(const QString imagePath);

signals:
	void debugLog(QString log);

};
