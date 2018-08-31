#pragma once

#include <QObject>
#include <QBasicTimer>
#include <QMatrix4x4>

class Camera : public QObject
{
	Q_OBJECT

public:
	Camera(const QVector3D position, const QVector3D lookAt, const QVector3D worldUp);
	Camera(const QVector3D position, double yaw, double pitch, const QVector3D worldUp);
	~Camera();

	QVector3D Up;
	QVector3D Forward;
	QVector3D Right;
	QVector3D WorldUp;
	QVector3D Position;
	QMatrix4x4 View;
	QVector3D RotationEuler;
	QMatrix4x4 UpdateViewMatrix();
	void CameraMove(QVector3D pos, double yaw, double pitch);

signals:
	void debugLog(QString log);

};
