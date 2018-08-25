#include "OpenGLWidget.h"
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>

OpenGLWidget::OpenGLWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
}

OpenGLWidget::~OpenGLWidget()
{
}

void OpenGLWidget::initializeGL()
{
	initializeOpenGLFunctions();
	// vertex shader
	QOpenGLShader *vshader = new QOpenGLShader(QOpenGLShader::Vertex, this);
	vshader->compileSourceFile("standard.vert");
	// fragment shader
	QOpenGLShader *fshader = new QOpenGLShader(QOpenGLShader::Fragment, this);
	fshader->compileSourceFile("standard.frag");

	// shader program
	program = new QOpenGLShaderProgram;
	program->addShader(vshader);
	program->addShader(fshader);

	program->link();
	program->bind();

	glClearColor(0.5f, 0.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
}

void OpenGLWidget::resizeGL(int w, int h)
{

}

void OpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// vertices array
	GLfloat positionData[] = {
		-0.8f, -0.8f, 0.0f,
		0.8f, -0.8f, 0.0f,
		0.0f, 0.8f, 0.0f };
	GLfloat colorData[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f };

	// vertex buffer object
	vbo.create();
	vbo.bind();
	// vertex positions
	vbo.allocate(positionData, 18 * sizeof(GLfloat));
	GLuint vPosition = program->attributeLocation("VertexPosition");
	program->setAttributeBuffer(vPosition, GL_FLOAT, 0, 3, 0);
	glEnableVertexAttribArray(vPosition);


	// vertex colors
	vbo.write(9 * sizeof(GLfloat), colorData, 9 * sizeof(GLfloat));
	GLuint vColor = program->attributeLocation("VertexColor");
	program->setAttributeBuffer(vColor, GL_FLOAT, 9 * sizeof(GLfloat), 3, 0);
	glEnableVertexAttribArray(vColor);

	// transform
	int w = width();
	int h = height();
	QMatrix4x4 matrix;
	matrix.perspective(45.0f, (GLfloat)w / (GLfloat)h, 0.001f, 100.0f);
	matrix.translate(0.0, 0.0, -3.0);
	matrix.rotate(90.0f, 0.0f, 0.0f, 1.0f);

	program->setUniformValue("RotationMatrix", matrix);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}