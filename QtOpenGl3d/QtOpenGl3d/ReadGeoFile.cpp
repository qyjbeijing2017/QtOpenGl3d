#include "ReadGeoFile.h"
#include "qfiledialog.h"
#include "qfile.h"
#include "qmessagebox.h"

ReadGeoFile::ReadGeoFile(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	maxX = 0;
	maxY = 0;
	minX = 0;
	minY = 0;
	vertexX = 1600;
	vertexY = 900;
	samplingCount = 0.35;//清晰度设置，越高散点程度越高
	//samplingCount = 0.01;
}

ReadGeoFile::~ReadGeoFile()
{
}

void ReadGeoFile::on_pushButtonOpenFile_clicked()
{
	m_path = QFileDialog::getOpenFileName(this);
	ui.lineEdit->setText(m_path);
}
void ReadGeoFile::on_lineEdit_textChanged(const QString & arg1)
{
	m_path = arg1;
}

void ReadGeoFile::on_pushButtonCancel_clicked()
{
	this->close();
}

void ReadGeoFile::on_pushButtonOk_clicked()
{
	QFile* file = new QFile(m_path);

	if (!file->open(QIODevice::ReadOnly)) {
		QMessageBox::information(NULL, "error", "Failed to open the file.", QMessageBox::Ok, QMessageBox::Ok);
	}

	boomPointSize = 0;
	detectionPointSize = 0;
	int All = 0;

	file->seek(0);
	file->read((char*)&boomPointSize, 4);
	file->read((char*)&detectionPointSize, 4);
	All = boomPointSize + detectionPointSize;

	boomPointData = new BoomPointData[boomPointSize];
	detectionPointData = new DetectionPointData[detectionPointSize];

	for (int i = 0; i < boomPointSize; i++)
	{
		file->read((char*)&boomPointData[i].boomNub, 4);
		file->read((char*)&boomPointData[i].boomLineNub, 4);
		file->read((char*)&boomPointData[i].boomStakeNub, 4);
		file->read((char*)&boomPointData[i].boomX, 4);
		file->read((char*)&boomPointData[i].boomY, 4);
		file->read((char*)&boomPointData[i].boomHeight, 4);
		file->read((char*)&boomPointData[i].wellDepth, 4);
	}

	for (int i = 0; i < detectionPointSize; i++)
	{
		file->read((char*)&detectionPointData[i].detectionLineNub, 4);
		file->read((char*)&detectionPointData[i].detectionStakeNub, 4);
		file->read((char*)&detectionPointData[i].detectionX, 4);
		file->read((char*)&detectionPointData[i].detectionY, 4);
		file->read((char*)&detectionPointData[i].detectionHeight, 4);
	}

	file->close();

	getMax();
	rasterize();
	this->close();
}
// 采样栅格化
void ReadGeoFile::rasterize()
{
	rasterizeVector = new QVector<double>(vertexX * vertexY, 0);
	QVector<double> imageVectorTem(vertexX * vertexY, 0);
	QVector<int> imageVectorNub(vertexX * vertexY, 0);

	for (int i = 0; i < boomPointSize; i++)
	{
		int x = (int)(((boomPointData[i].boomX - minX) / (maxX - minX + 1)) * vertexX);
		int y = (int)(((boomPointData[i].boomY - minY) / (maxY - minY + 1)) * vertexY);
		imageVectorTem.replace(x * vertexX + y, imageVectorTem.at(x * vertexY + y) + boomPointData[i].boomHeight);
		//imageVectorTem.replace(y * ImageX + x, imageVectorTem.at(y * ImageX + x) + (((boomPointData[i].boomHeight - minZ) / (maxZ - minZ)) * 10));
		imageVectorNub.replace(x * vertexX + y, imageVectorNub.at(x * vertexY + y) + 1);
	}

	for (int i = 0; i < detectionPointSize; i++)
	{
		int x = (int)(((detectionPointData[i].detectionX - minX) / (maxX - minX + 1)) * vertexX);
		int y = (int)(((detectionPointData[i].detectionY - minY) / (maxY - minY + 1)) * vertexY);
		imageVectorTem.replace(x * vertexY + y, imageVectorTem.at(x * vertexY + y) + detectionPointData[i].detectionHeight);
		//imageVectorTem.replace(y * ImageX + x, imageVectorTem.at(y * ImageX + x) + (((detectionPointData[i].detectionHeight - minZ) / (maxZ - minZ)) * 10));
		imageVectorNub.replace(x * vertexY + y, imageVectorNub.at(x * vertexY + y) + 1);
	}

	for (int i = 0; i < rasterizeVector->size(); i++)
	{
		if (imageVectorNub.at(i) != 0) {
			rasterizeVector->replace(i, imageVectorTem.at(i) / imageVectorNub.at(i));
		}
		else
		{
			rasterizeVector->replace(i, minZ);
		}
	}
	removeZeroPointLiner();

}
// 空格差值
void ReadGeoFile::removeZeroPointLiner()
{
	removeZeroVector = new QVector<double>(rasterizeVector->size(), 0);
	RfVector2D maxXY = RfVector2D::index2RFv2D(rasterizeVector->size(), vertexY);

	for (int i = 0; i < rasterizeVector->size(); i++)
	{

		double d = 0;
		if (rasterizeVector->at(i) == 0)
		{
			RfVector2D v2 = RfVector2D::index2RFv2D(i, vertexY);
			if (v2.x == 0 && v2.y == 0)//左下
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(0, 1), vertexY));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(1, 0), vertexY));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(1, 1), vertexY));

				d = (h1 + h2 + h3) / 3;
			}
			else if (v2.x == vertexX - 1 && v2.y == 0)//左上
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 1, 1), vertexY));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, 0), vertexY));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, 1), vertexY));

				d = (h1 + h2 + h3) / 3;
			}
			else if (v2.x == 0 && v2.y == vertexY - 1)//右下
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(0, vertexY - 2), vertexY));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(1, vertexY - 1), vertexY));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(1, vertexY - 2), vertexY));

				d = (h1 + h2 + h3) / 3;
			}
			else if (v2.x == vertexX - 1 && v2.y == vertexY - 1)//右上
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 1, vertexY - 2), vertexY));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, vertexY - 1), vertexY));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, vertexY - 2), vertexY));

				d = (h1 + h2 + h3) / 3;
			}
			else if (v2.x == vertexX - 1)//上边缘
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexY));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexY));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexY));
				int h4 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y - 1), vertexY));
				int h5 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y + 1), vertexY));
				d = (h1 + h2 + h3 + h4 + h5) / 5;
			}
			else if (v2.x == 0)//下边缘
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexY));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexY));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexY));
				int h4 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y - 1), vertexY));
				int h5 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y + 1), vertexY));
				d = (h1 + h2 + h3 + h4 + h5) / 5;
			}
			else if (v2.y == vertexY - 1)//右边缘
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexY));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexY));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexY));
				int h4 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y - 1), vertexY));
				int h5 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y - 1), vertexY));
				d = (h1 + h2 + h3 + h4 + h5) / 5;
			}
			else if (v2.y == 0)//左边缘
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexY));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexY));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexY));
				int h4 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y + 1), vertexY));
				int h5 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y + 1), vertexY));
				d = (h1 + h2 + h3 + h4 + h5) / 5;
			}
			else//中心
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexY));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y - 1), vertexY));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y + 1), vertexY));
				int h4 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexY));
				int h5 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexY));
				int h6 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexY));
				int h7 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y - 1), vertexY));
				int h8 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y + 1), vertexY));
				d = (h1 + h2 + h3 + h4 + h5 + h6 + h7 + h8) / 8;
			}

		}
		else
		{
			d = rasterizeVector->at(i);
		}

		d = minZ + d / maxZ - minZ; //百分化
		removeZeroVector->replace(i, d);
	}


	lineSmoothStack = new QStack<QVector<double>>();
	lineSmoothStack->push(*removeZeroVector);
	//linerSmooth();
	//linerSmooth();
	data2model();
	emit readFileCallBack(this);
}
// 进一步线性平滑
void ReadGeoFile::linerSmooth()
{
	QVector<double> linerSmoothVector = QVector<double>(lineSmoothStack->top().size(), 0);
	RfVector2D maxXY = RfVector2D::index2RFv2D(lineSmoothStack->top().size(), vertexY);
	for (int i = 0; i < lineSmoothStack->top().size(); i++)
	{
		double d = 0;
		RfVector2D v2 = RfVector2D::index2RFv2D(i, vertexY);
		if (v2.x == 0 && v2.y == 0)//左下
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(0, 1), vertexY));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(1, 0), vertexY));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(1, 1), vertexY));

			d = (h1 + h2 + h3) / 3;
		}
		else if (v2.x == vertexX - 1 && v2.y == 0)//左上
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 1, 1), vertexY));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, 0), vertexY));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, 1), vertexY));

			d = (h1 + h2 + h3) / 3;
		}
		else if (v2.x == 0 && v2.y == vertexY - 1)//右下
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(0, vertexY - 2), vertexY));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(1, vertexY - 1), vertexY));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(1, vertexY - 2), vertexY));

			d = (h1 + h2 + h3) / 3;
		}
		else if (v2.x == vertexX - 1 && v2.y == vertexY - 1)//右上
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 1, vertexY - 2), vertexY));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, vertexY - 1), vertexY));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, vertexY - 2), vertexY));

			d = (h1 + h2 + h3) / 3;
		}
		else if (v2.x == vertexX - 1)//上边缘
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexY));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexY));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexY));
			int h4 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y - 1), vertexY));
			int h5 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y + 1), vertexY));
			d = (h1 + h2 + h3 + h4 + h5) / 5;
		}
		else if (v2.x == 0)//下边缘
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexY));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexY));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexY));
			int h4 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y - 1), vertexY));
			int h5 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y + 1), vertexY));
			d = (h1 + h2 + h3 + h4 + h5) / 5;
		}
		else if (v2.y == vertexY - 1)//右边缘
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexY));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexY));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexY));
			int h4 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y - 1), vertexY));
			int h5 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y - 1), vertexY));
			d = (h1 + h2 + h3 + h4 + h5) / 5;
		}
		else if (v2.y == 0)//左边缘
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexY));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexY));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexY));
			int h4 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y + 1), vertexY));
			int h5 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y + 1), vertexY));
			d = (h1 + h2 + h3 + h4 + h5) / 5;
		}
		else//中心
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexY));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y - 1), vertexY));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y + 1), vertexY));
			int h4 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexY));
			int h5 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexY));
			int h6 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexY));
			int h7 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y - 1), vertexY));
			int h8 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y + 1), vertexY));
			d = (h1 + h2 + h3 + h4 + h5 + h6 + h7 + h8) / 8;
		}
		linerSmoothVector.replace(i, lineSmoothStack->top().at(i) + (d - lineSmoothStack->top().at(i)*0.3));

	}
	lineSmoothStack->push(linerSmoothVector);
}
// 撤回一次平滑状态
void ReadGeoFile::reBacklinerSmooth()
{
	if (lineSmoothStack->size() > 1)
	{
		lineSmoothStack->pop();
	}
}
// 转换顶点数据至模型数据
void ReadGeoFile::data2model()
{
	QVector<double> data = lineSmoothStack->top();
	int vertexNum = lineSmoothStack->top().size();
	VertexNum = (vertexX - 1)*(vertexY - 1) * 2 * 3;
	VertexData = new QVector<GEOVertexData>(VertexNum, GEOVertexData());
	int j = 0;
	for (int i = 0; i < vertexNum; i++)
	{
		RfVector2D vect1 = RfVector2D::index2RFv2D(i, vertexY);//左下
		if (vect1.x < vertexX - 1 && vect1.y < vertexY - 1)
		{
			RfVector2D vect2 = RfVector2D(vect1.x + 1, vect1.y);//左上角
			RfVector2D vect3 = RfVector2D(vect1.x, vect1.y + 1);//右下角
			RfVector2D vect4 = RfVector2D(vect1.x + 1, vect1.y + 1);//右上
			Vector3D vec1 = Vector3D((float)vect1.y / (float)(vertexY - 1), data[RfVector2D::RFv2D2index(vect1, vertexY)], -(float)vect1.x / (float)(vertexX - 1));
			Vector3D vec2 = Vector3D((float)vect2.y / (float)(vertexY - 1), data[RfVector2D::RFv2D2index(vect2, vertexY)], -(float)vect2.x / (float)(vertexX - 1));
			Vector3D vec3 = Vector3D((float)vect3.y / (float)(vertexY - 1), data[RfVector2D::RFv2D2index(vect3, vertexY)], -(float)vect3.x / (float)(vertexX - 1));
			Vector3D vec4 = Vector3D((float)vect4.y / (float)(vertexY - 1), data[RfVector2D::RFv2D2index(vect4, vertexY)], - (float)vect4.x / (float)(vertexX - 1));
			Vector3D nor1 = Vector3D::crossProduct(vec2 - vec1, vec3 - vec1);
			Vector3D nor2 = Vector3D::crossProduct(vec2 - vec4, vec3 - vec4);


			GEOVertexData geoVertex1;
			geoVertex1.position = vec1;
			geoVertex1.normal = nor1;
			VertexData->replace(j, geoVertex1);
			j++;

			GEOVertexData geoVertex2;
			geoVertex2.position = vec3;
			geoVertex2.normal = nor1;
			VertexData->replace(j, geoVertex2);
			j++;


			GEOVertexData geoVertex3;
			geoVertex3.position = vec2;
			geoVertex3.normal = nor1;
			VertexData->replace(j, geoVertex3);
			j++;

			GEOVertexData geoVertex4;
			geoVertex4.position = vec3;
			geoVertex4.normal = nor2;
			VertexData->replace(j, geoVertex4);
			j++;


			GEOVertexData geoVertex5;
			geoVertex5.position = vec4;
			geoVertex5.normal = nor2;
			VertexData->replace(j, geoVertex5);
			j++;


			GEOVertexData geoVertex6;
			geoVertex6.position = vec2;
			geoVertex6.normal = nor2;
			VertexData->replace(j, geoVertex6);
			j++;

		}
	}
}

void ReadGeoFile::getMax()
{
	maxX = boomPointData[0].boomX;
	minX = boomPointData[0].boomX;
	maxY = boomPointData[0].boomY;
	minY = boomPointData[0].boomY;
	maxZ = boomPointData[0].boomHeight;
	minZ = boomPointData[0].boomHeight;

	for (int i = 1; i < boomPointSize; i++)
	{

		if (boomPointData[i].boomX > maxX)
		{
			maxX = boomPointData[i].boomX;
		}
		else if (boomPointData[i].boomX < minX)
		{
			minX = boomPointData[i].boomX;
		}

		if (boomPointData[i].boomY > maxY)
		{
			maxY = boomPointData[i].boomY;
		}
		else if (boomPointData[i].boomY < minY)
		{
			minY = boomPointData[i].boomY;
		}

		if (boomPointData[i].boomHeight > maxZ)
		{
			maxZ = boomPointData[i].boomHeight;
		}
		else if (boomPointData[i].boomHeight < minZ)
		{
			minZ = boomPointData[i].boomHeight;
		}

	}


	for (int i = 0; i < detectionPointSize; i++)
	{

		if (detectionPointData[i].detectionX > maxX)
		{
			maxX = detectionPointData[i].detectionX;
		}
		else if (detectionPointData[i].detectionX < minX)
		{
			minX = detectionPointData[i].detectionX;
		}

		if (detectionPointData[i].detectionY > maxY)
		{
			maxY = detectionPointData[i].detectionY;
		}
		else if (detectionPointData[i].detectionY < minY)
		{
			minY = detectionPointData[i].detectionY;
		}

		if (detectionPointData[i].detectionHeight > maxZ)
		{
			maxZ = detectionPointData[i].detectionHeight;
		}
		else if (detectionPointData[i].detectionHeight < minZ)
		{
			minZ = detectionPointData[i].detectionHeight;
		}

	}

	vertexX = (int)((maxX - minX) / 100 * samplingCount);
	vertexY = (int)((maxY - minY) / 100 * samplingCount);

}
