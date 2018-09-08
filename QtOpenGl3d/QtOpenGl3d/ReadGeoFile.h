#pragma once

#include <QWidget>
#include "ui_ReadGeoFile.h"
#include <QVector>
#include <qStack>
#include <QtMath>
#include <QVector3D>

struct BoomPointData
{
	int boomNub;
	int boomLineNub;
	int boomStakeNub;
	float boomX;
	float boomY;
	int boomHeight;
	int wellDepth;
};

struct DetectionPointData
{
	int detectionLineNub;
	int detectionStakeNub;
	float detectionX;
	float detectionY;
	int detectionHeight;
};

struct RfVector2D
{

	int x;
	int y;

	RfVector2D() {
		x = 0;
		y = 0;
	}
	RfVector2D(int valueX, int valueY) {
		x = valueX;
		y = valueY;
	}
	static RfVector2D index2RFv2D(int i, int vecY)
	{
		RfVector2D v2;
		v2.x = i / vecY;
		v2.y = i % vecY;
		return v2;
	}
	static int RFv2D2index(RfVector2D v2, int vecY)
	{
		return vecY * v2.x + v2.y;
	}
};

struct Vector3D
{
	float x, y, z;
	Vector3D() {
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}
	Vector3D(float X, float Y, float Z) {
		x = X;
		y = Y;
		z = Z;
	}
	static Vector3D crossProduct(Vector3D a, Vector3D b)
	{
		QVector3D c = QVector3D::crossProduct(QVector3D(a.x, a.y, a.z), QVector3D(b.x, b.y, b.z));
		return Vector3D(c.x(), c.y(), c.z());
	}
	Vector3D operator + (Vector3D a) {
		Vector3D c = Vector3D(x + a.x,x + a.y,x + a.z);
		return c;
	}
	Vector3D operator - (Vector3D a) {
		Vector3D c = Vector3D(x - a.x, x - a.y, x - a.z);
		return c;
	}
};

struct GEOVertexData
{
	Vector3D position;
	Vector3D normal;
	GEOVertexData() {
		position = Vector3D(0.0f, 0.0f, 0.0f);
		normal = Vector3D(0.0f, 0.0f, 0.0f);
	}
};

class ReadGeoFile : public QWidget
{
	Q_OBJECT

public:
	ReadGeoFile(QWidget *parent = Q_NULLPTR);
	~ReadGeoFile();
	void linerSmooth();
	void reBacklinerSmooth();
	void data2model();
	QVector<GEOVertexData>* VertexData;
	unsigned int VertexNum;

private slots:
	void on_pushButtonOpenFile_clicked();
	void on_lineEdit_textChanged(const QString &arg1);
	void on_pushButtonCancel_clicked();
	void on_pushButtonOk_clicked();

private:
	Ui::ReadGeoFile ui;
	QString m_path;
	BoomPointData* boomPointData;
	DetectionPointData*  detectionPointData;
	int boomPointSize;
	int detectionPointSize;
	QVector<double> *rasterizeVector;
	QVector<double> *removeZeroVector;
	QStack<QVector<double>> *lineSmoothStack;
	float maxX;
	float minX;
	float maxY;
	float minY;
	float minZ;
	float maxZ;
	int vertexX;
	int vertexY;
	float samplingCount;
	void rasterize();
	void removeZeroPointLiner();
	void getMax();
signals:
	void readFileCallBack(ReadGeoFile* readFile);
};
