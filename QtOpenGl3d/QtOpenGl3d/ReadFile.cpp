#include "ReadFile.h"
#include"qfiledialog.h"
#include"qfile.h"
#include"qmessagebox.h"




ReadFile::ReadFile(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	maxX = 0;
	maxY = 0;
	minX = 0;
	minY = 0;
	vertexX = 1600;
	vertexY = 900;
	samplingCount = 10;//���������ã�Խ��ɢ��̶�Խ��

}

ReadFile::~ReadFile()
{

}

void ReadFile::on_pushButtonOpenFile_clicked()
{
	m_path = QFileDialog::getOpenFileName(this);
	ui.lineEdit->setText(m_path);
}

void ReadFile::on_lineEdit_textChanged(const QString &arg1)
{
	m_path = arg1;
}

void ReadFile::on_pushButtonCancel_clicked()
{
	this->close();
}

void ReadFile::on_pushButtonOk_clicked()
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

// ����դ��
void ReadFile::rasterize()
{
	rasterizeVector = new QVector<double>(vertexX * vertexY, 0);
	QVector<double> imageVectorTem(vertexX * vertexY, 0);
	QVector<int> imageVectorNub(vertexX * vertexY, 0);

	for (int i = 0; i < boomPointSize; i++)
	{
		int x = (int)(((boomPointData[i].boomX - minX) / (maxX - minX + 1)) * vertexX);
		int y = (int)(((boomPointData[i].boomY - minY) / (maxY - minY + 1)) * vertexY);
		imageVectorTem.replace(x * vertexX + y, imageVectorTem.at(x * vertexX + y) + boomPointData[i].boomHeight);
		//imageVectorTem.replace(y * ImageX + x, imageVectorTem.at(y * ImageX + x) + (((boomPointData[i].boomHeight - minZ) / (maxZ - minZ)) * 10));
		imageVectorNub.replace(x * vertexX + y, imageVectorNub.at(x * vertexX + y) + 1);
	}

	for (int i = 0; i < detectionPointSize; i++)
	{
		int x = (int)(((detectionPointData[i].detectionX - minX) / (maxX - minX + 1)) * vertexX);
		int y = (int)(((detectionPointData[i].detectionY - minY) / (maxY - minY + 1)) * vertexY);
		imageVectorTem.replace(x * vertexX + y, imageVectorTem.at(x * vertexX + y) + detectionPointData[i].detectionHeight);
		//imageVectorTem.replace(y * ImageX + x, imageVectorTem.at(y * ImageX + x) + (((detectionPointData[i].detectionHeight - minZ) / (maxZ - minZ)) * 10));
		imageVectorNub.replace(x * vertexX + y, imageVectorNub.at(x * vertexX + y) + 1);
	}

	for (int i = 0; i < rasterizeVector->size(); i++)
	{
		if (imageVectorNub.at(i) != 0)
			rasterizeVector->replace(i, imageVectorTem.at(i) / imageVectorNub.at(i));
	}

}
// �ո��ֵ
void ReadFile::removeZeroPointLiner()
{
	removeZeroVector = new QVector<double>(rasterizeVector->size(), 0);
	RfVector2D maxXY = RfVector2D::index2RFv2D(rasterizeVector->size(), vertexX);
	for (int i = 0; i < rasterizeVector->size(); i++)
	{
		double d = 0;
		if (rasterizeVector->at(i) == 0)
		{
			RfVector2D v2 = RfVector2D::index2RFv2D(i, vertexX);
			if (v2.x == 0 && v2.y == 0)//����
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(0, 1), vertexX));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(1, 0), vertexX));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(1, 1), vertexX));

				d = (h1 + h2 + h3) / 3;
			}
			else if (v2.x == vertexX - 1 && v2.y == 0)//����
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 1, 1), vertexX));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, 0), vertexX));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, 1), vertexX));

				d = (h1 + h2 + h3) / 3;
			}
			else if (v2.x == 0 && v2.y == vertexY - 1)//����
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(0, vertexY - 2), vertexX));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(1, vertexY - 1), vertexX));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(1, vertexY - 2), vertexX));

				d = (h1 + h2 + h3) / 3;
			}
			else if (v2.x == vertexX - 1 && v2.y == vertexY - 1)//����
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 1, vertexY - 2), vertexX));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, vertexY - 1), vertexX));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, vertexY - 2), vertexX));

				d = (h1 + h2 + h3) / 3;
			}
			else if (v2.x == vertexX - 1)//�ϱ�Ե
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexX));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexX));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexX));
				int h4 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y - 1), vertexX));
				int h5 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y + 1), vertexX));
				d = (h1 + h2 + h3 + h4 + h5) / 5;
			}
			else if (v2.x == 0)//�±�Ե
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexX));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexX));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexX));
				int h4 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y - 1), vertexX));
				int h5 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y + 1), vertexX));
				d = (h1 + h2 + h3 + h4 + h5) / 5;
			}
			else if (v2.y == vertexY - 1)//�ұ�Ե
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexX));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexX));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexX));
				int h4 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y - 1), vertexX));
				int h5 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y - 1), vertexX));
				d = (h1 + h2 + h3 + h4 + h5) / 5;
			}
			else if (v2.y == 0)//���Ե
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexX));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexX));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexX));
				int h4 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y + 1), vertexX));
				int h5 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y + 1), vertexX));
				d = (h1 + h2 + h3 + h4 + h5) / 5;
			}
			else//����
			{
				int h1 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexX));
				int h2 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y - 1), vertexX));
				int h3 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y + 1), vertexX));
				int h4 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexX));
				int h5 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexX));
				int h6 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexX));
				int h7 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y - 1), vertexX));
				int h8 = rasterizeVector->at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y + 1), vertexX));
				d = (h1 + h2 + h3 + h4 + h5 + h6 + h7 + h8) / 8;
			}

		}
		else
		{
			d = rasterizeVector->at(i);
		}

		d = minZ + d / maxZ - minZ; //�ٷֻ�
		removeZeroVector->replace(i, d);
	}
	lineSmoothStack = new QStack<QVector<double>>();
	lineSmoothStack->push(*removeZeroVector);
}
// ��һ������ƽ��
void ReadFile::linerSmooth()
{
	QVector<double> linerSmoothVector = QVector<double>(lineSmoothStack->top().size(), 0);
	RfVector2D maxXY = RfVector2D::index2RFv2D(lineSmoothStack->top().size(), vertexX);
	for (int i = 0; i < lineSmoothStack->top().size(); i++)
	{
		double d = 0;
		RfVector2D v2 = RfVector2D::index2RFv2D(i, vertexX);
		if (v2.x == 0 && v2.y == 0)//����
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(0, 1), vertexX));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(1, 0), vertexX));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(1, 1), vertexX));

			d = (h1 + h2 + h3) / 3;
		}
		else if (v2.x == vertexX - 1 && v2.y == 0)//����
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 1, 1), vertexX));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, 0), vertexX));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, 1), vertexX));

			d = (h1 + h2 + h3) / 3;
		}
		else if (v2.x == 0 && v2.y == vertexY - 1)//����
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(0, vertexY - 2), vertexX));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(1, vertexY - 1), vertexX));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(1, vertexY - 2), vertexX));

			d = (h1 + h2 + h3) / 3;
		}
		else if (v2.x == vertexX - 1 && v2.y == vertexY - 1)//����
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 1, vertexY - 2), vertexX));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, vertexY - 1), vertexX));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(vertexX - 2, vertexY - 2), vertexX));

			d = (h1 + h2 + h3) / 3;
		}
		else if (v2.x == vertexX - 1)//�ϱ�Ե
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexX));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexX));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexX));
			int h4 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y - 1), vertexX));
			int h5 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y + 1), vertexX));
			d = (h1 + h2 + h3 + h4 + h5) / 5;
		}
		else if (v2.x == 0)//�±�Ե
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexX));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexX));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexX));
			int h4 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y - 1), vertexX));
			int h5 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y + 1), vertexX));
			d = (h1 + h2 + h3 + h4 + h5) / 5;
		}
		else if (v2.y == vertexY - 1)//�ұ�Ե
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexX));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexX));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexX));
			int h4 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y - 1), vertexX));
			int h5 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y - 1), vertexX));
			d = (h1 + h2 + h3 + h4 + h5) / 5;
		}
		else if (v2.y == 0)//���Ե
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexX));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexX));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexX));
			int h4 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y + 1), vertexX));
			int h5 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y + 1), vertexX));
			d = (h1 + h2 + h3 + h4 + h5) / 5;
		}
		else//����
		{
			int h1 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y), vertexX));
			int h2 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y - 1), vertexX));
			int h3 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x - 1, v2.y + 1), vertexX));
			int h4 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y + 1), vertexX));
			int h5 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x, v2.y - 1), vertexX));
			int h6 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y), vertexX));
			int h7 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y - 1), vertexX));
			int h8 = lineSmoothStack->top().at(RfVector2D::RFv2D2index(RfVector2D(v2.x + 1, v2.y + 1), vertexX));
			d = (h1 + h2 + h3 + h4 + h5 + h6 + h7 + h8) / 8;
		}
		linerSmoothVector.replace(i, d);
		lineSmoothStack->push(linerSmoothVector);
	}

}
// ����һ��ƽ��״̬
void ReadFile::reBacklinerSmooth()
{
	if (lineSmoothStack->size() > 1)
	{
		lineSmoothStack->pop();
	}
}

// ת������������ģ������
void ReadFile::data2model()
{
	QVector<double> data = lineSmoothStack->top();
	VertexNum = lineSmoothStack->top().size();
	VertexData = new GEOVertexData[VertexNum];
	for (int i = 0; i < VertexNum; i++)
	{
		GEOVertexData vertex;
		RfVector2D v2 = RfVector2D::index2RFv2D(i,vertexX);
		RfVector2D zero_one = RfVector2D(v2.x / (vertexX - 1), v2.y / (vertexY - 1));
		vertex.position = QVector3D(zero_one.x, data.at(i), zero_one.y);
	}
}


void ReadFile::getMax()
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
