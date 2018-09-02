#pragma once

#include <QWidget>
#include "ui_ReadFile.h"
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
	static RfVector2D index2RFv2D(int i, int vecX) 
	{
		RfVector2D v2;
		v2.x = i / vecX;
		v2.y = i % vecX;
		return v2;
	}
	static int RFv2D2index(RfVector2D v2, int vecX) 
	{
		return vecX * v2.x +v2.y;
	}
};
struct GEOVertexData
{
	QVector3D position;
	QVector3D normal;
};

class ReadFile : public QWidget
{
	Q_OBJECT

public:
	ReadFile(QWidget *parent = Q_NULLPTR);
	~ReadFile();
	GEOVertexData* VertexData;
	unsigned int VertexNum;


private slots:
	void on_pushButtonOpenFile_clicked();
	void on_lineEdit_textChanged(const QString &arg1);
	void on_pushButtonCancel_clicked();
	void on_pushButtonOk_clicked();




private:
	Ui::ReadFile ui;
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
	void linerSmooth();
	void reBacklinerSmooth();
	void data2model();
	void getMax();



};
