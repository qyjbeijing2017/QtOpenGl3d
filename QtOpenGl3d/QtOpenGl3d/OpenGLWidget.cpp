#include "OpenGLWidget.h"
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QTime>
#include "Camera.h"
#include "qopenglwindow.h"
#include "QtOpenGl3d.h"
#include <QtMath>

OpenGLWidget::OpenGLWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
	mousePos = new QPoint();
	this->grabKeyboard();
}	

OpenGLWidget::~OpenGLWidget()
{
	program->release();
	ourTexture->release();
	ourTexture1->release();
}

void OpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();

	creatShader("standard.vert", "standard.frag");
	ourTexture = InitTexture("container.jpg");
	ourTexture1 = InitTexture("awesomeface.png");


	glClearColor(0.5f, 0.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	int w = width();
	int h = height();
	modelMatrix = new QMatrix4x4;
	projectMatrix = new QMatrix4x4;

	projectMatrix->perspective(45.0f, (GLfloat)w / (GLfloat)h, 0.001f, 100.0f);

	//camera = new Camera(QVector3D(0, 0, -3), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
	camera = new Camera(QVector3D(0, 0, 3), 180.0f, 0.0f, QVector3D(0, 1, 0));

	lightPos = QVector3D(1.0f, 1.0f, 1.0f);

	timer.start(12, this);
}

void OpenGLWidget::resizeGL(int w, int h)
{
	projectMatrix = new QMatrix4x4;
	projectMatrix->perspective(45.0f, (GLfloat)w / (GLfloat)h, 0.001f, 100.0f);
}

#pragma region Vertex Data

float positionData[] = {
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,
	0.5f, -0.5f, -0.5f,   1.0f, 0.0f,  0.0f,  0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,   1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	0.5f,  0.5f, -0.5f,   1.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,
	0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  0.0f,  0.0f, 1.0f,
	0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
	0.5f,  0.5f,  0.5f,   1.0f, 1.0f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,

	0.5f,  0.5f,  0.5f,   1.0f, 0.0f,  1.0f,  0.0f,  0.0f,
	0.5f,  0.5f, -0.5f,   1.0f, 1.0f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,   0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f, -0.5f,   0.0f, 1.0f,  1.0f,  0.0f,  0.0f,
	0.5f, -0.5f,  0.5f,   0.0f, 0.0f,  1.0f,  0.0f,  0.0f,
	0.5f,  0.5f,  0.5f,   1.0f, 0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f, -0.5f,   1.0f, 1.0f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
	0.5f, -0.5f,  0.5f,   1.0f, 0.0f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f
};

QVector3D cubePositions[] = {
	QVector3D(0.0f,  0.0f,  0.0f),
	QVector3D(2.0f,  5.0f, -15.0f),
	QVector3D(-1.5f, -2.2f, -2.5f),
	QVector3D(-3.8f, -2.0f, -12.3f),
	QVector3D(2.4f, -0.4f, -3.5f),
	QVector3D(-1.7f,  3.0f, -7.5f),
	QVector3D(1.3f, -2.0f, -2.5f),
	QVector3D(1.5f,  2.0f, -2.5f),
	QVector3D(1.5f,  0.2f, -1.5f),
	QVector3D(-1.3f,  1.0f, -1.5f)
};

//unsigned int indices[] = { // 注意索引从0开始! 
//	3, 1, 0, // 第一个三角形
//	3, 2, 1  // 第二个三角形
//};


//float vertices[] = {
//	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
//	0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
//	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
//	0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
//	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
//	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
//
//	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
//	0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
//	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
//	0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
//	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
//	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
//
//	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
//	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
//	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
//	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
//	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
//	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
//
//	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
//	0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
//	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
//	0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
//	0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
//	0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
//
//	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
//	0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
//	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
//	0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
//	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
//	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
//
//	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
//	0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
//	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
//	0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
//	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
//	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
//};
#pragma endregion


void OpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	program->bind();

#pragma region vbo vao and ebo

	// vertex buffer object
	vbo.create();
	vbo.bind();

	vbo.allocate(positionData, 36 * 5 * sizeof(GLfloat));
	GLuint vPosition = program->attributeLocation("VertexPosition");
	program->setAttributeBuffer(vPosition, GL_FLOAT, 0, 3, 8 * sizeof(GLfloat));
	glEnableVertexAttribArray(vPosition);

	GLuint vTexCoord = program->attributeLocation("vTexCoord");
	program->setAttributeBuffer(vTexCoord, GL_FLOAT, 3 * sizeof(GLfloat), 2, 8 * sizeof(GLfloat));
	glEnableVertexAttribArray(vTexCoord);

	GLuint vNormal = program->attributeLocation("vNormal");
	program->setAttributeBuffer(vNormal, GL_FLOAT, 5 * sizeof(GLfloat), 3, 8 * sizeof(GLfloat));
	glEnableVertexAttribArray(vNormal);

	// index buffer object
	//QOpenGLBuffer* ebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	//ebo->create();
	//ebo->bind();
	//ebo->allocate(indices, 6 * sizeof(unsigned int));
	// vertex positions

	// vertex texCorrd

#pragma endregion

#pragma region Texture

	ourTexture->bind(0);
	program->setUniformValue("ourTexture", 0);
	ourTexture1->bind(1);
	program->setUniformValue("ourTexture1", 1);

#pragma endregion


	program->setUniformValue("view", camera->View);
	program->setUniformValue("project", *projectMatrix);
	program->setUniformValue("lightPos", lightPos);

	for (unsigned int i = 0; i < 10; i++)
	{
		modelMatrix = new QMatrix4x4;
		modelMatrix->translate(cubePositions[i]);
		//modelMatrix->rotate(QTime::currentTime().msecsSinceStartOfDay() * 0.01f, 0.5f, 1.0f, 0.0f);
		program->setUniformValue("model", *modelMatrix);


		glDrawArrays(GL_TRIANGLES, 0, 36);
	}
	vbo.release();

	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}



#pragma region Input

void OpenGLWidget::timerEvent(QTimerEvent * e)
{
	update();// GameLoop
}

void OpenGLWidget::mousePressEvent(QMouseEvent * event)
{
	if (event->button() == Qt::RightButton)
	{
		rightDown = true;
		mousePos->setX(event->x());
		mousePos->setY(event->y());
		cameraPitch = camera->RotationEuler.y();
		cameraYaw = camera->RotationEuler.x();
	}

}

void OpenGLWidget::mouseMoveEvent(QMouseEvent * event)
{
	if (rightDown)
	{
		
		float xOffset = event->x() - mousePos->x();
		float yOffset = event->y() - mousePos->y();
		
		cameraYaw += xOffset * sensitiveRotation;
		cameraPitch += yOffset * sensitiveRotation;
		if (cameraPitch > 89.0f)
			cameraPitch = 89.0f;
		if (cameraPitch < -89.0f)
			cameraPitch = -89.0f;
		
		camera->CameraMove(camera->Position, cameraYaw, cameraPitch);
		mousePos->setX(event->x());
		mousePos->setY(event->y());
		//QCursor::setPos(this->mapToGlobal(QPoint(100, 100)));
	}
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent * event)
{
	if (event->button() == Qt::RightButton)
	{
		rightDown = false;
	}
}

void OpenGLWidget::keyPressEvent(QKeyEvent * ev)
{
	QVector3D move;
	
	if (ev->key() == Qt::Key_W)
	{
		move += camera->Forward;
	}
	if (ev->key() == Qt::Key_A)
	{
		move -= camera->Right;
	}
	if (ev->key() == Qt::Key_S)
	{
		move -= camera->Forward;
	}
	if (ev->key() == Qt::Key_D)
	{
		move += camera->Right;
	}	
	if (ev->key() == Qt::Key_Space)
	{
		move += camera->Up;
	}
	if (ev->key() == Qt::Key_X)
	{
		move -= camera->Up;
	}

	camera->CameraMove(camera->Position + move, camera->RotationEuler.x(), camera->RotationEuler.y());

}

void OpenGLWidget::keyReleaseEvent(QKeyEvent * ev)
{
}

void OpenGLWidget::creatShader(QString vertexShader, QString fragmentShader)
{
	QOpenGLShader* m_vertShader;
	QOpenGLShader* m_fragShader;
	debugLog("start Shader Bind!\n");
	m_vertShader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	if (!m_vertShader->compileSourceFile(vertexShader))
	{
		debugLog("Faild to load vertex shader!\n");
		debugLog(m_vertShader->log());
		return;
	}
	m_fragShader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	if (!m_fragShader->compileSourceFile(fragmentShader))
	{
		debugLog("Faild to load fragment shader!\n");
		debugLog(m_fragShader->log());
		return;
	}
	program = new QOpenGLShaderProgram;
	program->addShader(m_vertShader);
	program->addShader(m_fragShader);

	if (!program->link())
	{
		debugLog("Faild to link program!\n");
		debugLog(program->log());
	}

}

QOpenGLTexture* OpenGLWidget::InitTexture(const QString imagePath)
{
	QOpenGLTexture* tex = new QOpenGLTexture(QImage(imagePath).mirrored());
	tex->setMinificationFilter(QOpenGLTexture::Nearest);
	tex->setMagnificationFilter(QOpenGLTexture::Linear);
	tex->setWrapMode(QOpenGLTexture::Repeat);
	//tex->generateMipMaps();
	return tex;
}

#pragma endregion



