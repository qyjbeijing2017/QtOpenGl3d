#include "OpenGLWidget.h"
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QMatrix4x4>
#include <QOpenGLTexture>

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

	creatShader("standard.vert", "standard.frag");
	ourTexture = InitTexture("container.jpg");
	ourTexture1 = InitTexture("awesomeface.png");

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
		//  ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
	};
	GLfloat colorData[] = {
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 1.0f };

	unsigned int indices[] = { // 注意索引从0开始! 
		3, 1, 0, // 第一个三角形
		3, 2, 1  // 第二个三角形
	};
	program->bind();


	// vertex buffer object
	vbo.create();
	vbo.bind();

	// index buffer object
	QOpenGLBuffer* ebo = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
	ebo->create();
	ebo->bind();
	ebo->allocate(indices, 6 * sizeof(unsigned int));

	// vertex positions
	vbo.allocate(positionData, 32 * sizeof(GLfloat));
	GLuint vPosition = program->attributeLocation("VertexPosition");
	program->setAttributeBuffer(vPosition, GL_FLOAT, 0, 3, 8 * sizeof(GLfloat));
	glEnableVertexAttribArray(vPosition);


	// vertex colors
	GLuint vColor = program->attributeLocation("vColor");
	program->setAttributeBuffer(vColor, GL_FLOAT, 3 * sizeof(GLfloat), 3, 8 * sizeof(GLfloat));
	glEnableVertexAttribArray(vColor);

	// vertex texCorrd
	GLuint vTexCoord = program->attributeLocation("vTexCoord");
	program->setAttributeBuffer(vTexCoord, GL_FLOAT, 6 * sizeof(GLfloat), 2, 8 * sizeof(GLfloat));
	glEnableVertexAttribArray(vTexCoord);


	//// vertex colors
	//vbo.write(9 * sizeof(GLfloat), colorData, 9 * sizeof(GLfloat));
	//GLuint vColor = program->attributeLocation("VertexColor");
	//program->setAttributeBuffer(vColor, GL_FLOAT, 9 * sizeof(GLfloat), 3, 0);
	//glEnableVertexAttribArray(vColor);

	// transform
	int w = width();
	int h = height();
	QMatrix4x4 matrix;
	matrix.perspective(45.0f, (GLfloat)w / (GLfloat)h, 0.001f, 100.0f);
	matrix.translate(0.0, 0.0, -3.0);
	//matrix.rotate(90.0f, 0.0f, 0.0f, 1.0f);
	//matrix.scale(0.5f, 0.5f, 0.5f);

	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_FRONT, GL_FILL);
	glEnable(GL_CULL_FACE);

	program->setUniformValue("RotationMatrix", matrix);

	ourTexture->bind(0);
	program->setUniformValue("ourTexture", 0);
	ourTexture1->bind(1);
	program->setUniformValue("ourTexture1", 1);
	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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


