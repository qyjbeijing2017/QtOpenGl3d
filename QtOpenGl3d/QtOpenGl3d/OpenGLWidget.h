#pragma once

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QBasicTimer>
#include <QMatrix4x4>
#include <QMouseEvent>


//class Input;
class QOpenGLShaderProgram;
class QOpenGLTexture;
class Camera;
class QtOpenGl3d;
class Input;
class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT

public:
	OpenGLWidget(QWidget *parent = 0);
	~OpenGLWidget();
	QtOpenGl3d* window;

protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void timerEvent(QTimerEvent *e);

	


	//������¼�
	void mousePressEvent(QMouseEvent* event);
	//����ƶ��¼�
	void mouseMoveEvent(QMouseEvent *event);
	//����ͷ��¼�
	void mouseReleaseEvent(QMouseEvent *event);

	
	//virtual void keyPressEvent(QKeyEvent *ev);
	//virtual void keyReleaseEvent(QKeyEvent *ev);


private:
	QBasicTimer timer;
	QOpenGLShaderProgram * program;
	QOpenGLBuffer vbo;
	QOpenGLBuffer* ebo;
	QOpenGLTexture* ourTexture;
	QOpenGLTexture* ourTexture1;
	QMatrix4x4* modelMatrix;
	QMatrix4x4* projectMatrix;

	Camera* camera;

	QPoint* mousePos;
	bool rightDown = false;

	QVector3D lightPos;

	void creatShader(QString vertexShader, QString fragmentShader);

	static QOpenGLTexture* InitTexture(const QString imagePath);
	
	float cameraPitch = 0.0f;
	float cameraYaw = 0.0f;

	Input *input;
	const float sensitiveRotation = 0.2f;
	const float sensitiveMove = 0.3f;
signals:
	void debugLog(QString log);

};
