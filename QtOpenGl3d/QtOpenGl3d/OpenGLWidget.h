#pragma once

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QBasicTimer>
#include <QMatrix4x4>
#include <QMouseEvent>
#include "ReadGeoFile.h"


//class Input;
class QOpenGLShaderProgram;
class QOpenGLTexture;
class Camera;
class QtOpenGl3d;
class Input;
class ReadGEOFile;
struct GEOVertexData;
struct RfVector2D;
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




	//鼠标点击事件
	void mousePressEvent(QMouseEvent* event);
	//鼠标移动事件
	void mouseMoveEvent(QMouseEvent *event);
	//鼠标释放事件
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

	QVector3D lightDir;
	float lightStrength;

	void creatShader(QString vertexShader, QString fragmentShader);

	static QOpenGLTexture* InitTexture(const QString imagePath);

	float cameraPitch = 0.0f;
	float cameraYaw = 0.0f;

	Input *input;
	const float sensitiveRotation = 0.05f;
	const float sensitiveMove = 0.03f;

	void myUpdate();

	QVector<GEOVertexData> vertex;
	int vertexNum;

signals:
	void debugLog(QString log);

private slots:
	void readFileCallBack(ReadGeoFile* readFile);
};
