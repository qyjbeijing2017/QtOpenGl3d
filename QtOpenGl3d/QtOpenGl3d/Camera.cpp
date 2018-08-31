#include "Camera.h"
#include <QtMath>
#include "OpenGLWidget.h"
#include <QString>

Camera::Camera(const QVector3D position, const QVector3D lookAt, const QVector3D worldUp)
{

	Position = position;


	WorldUp = worldUp;
	WorldUp.normalize();
	Forward = lookAt - position;
	Forward.normalize();
	Right = QVector3D::crossProduct(Forward, WorldUp);
	Right.normalize();
	Up = QVector3D::crossProduct(Right, Forward);
	Up.normalize();
	UpdateViewMatrix();
}

Camera::Camera(const QVector3D position, double yaw, double pitch, const QVector3D worldUp)
{
	Position = position;
	WorldUp = worldUp;
	WorldUp.normalize();
	RotationEuler.setX(yaw);
	RotationEuler.setY(pitch);
	RotationEuler.setZ(0.0f);
	Forward.setX(qCos(qDegreesToRadians(pitch))* qSin(qDegreesToRadians(yaw)));
	Forward.setY(qSin(qDegreesToRadians(pitch)));
	Forward.setZ(qCos(qDegreesToRadians(pitch))*qCos(qDegreesToRadians(yaw)));
	Forward.normalize();

	Right = QVector3D::crossProduct(Forward, WorldUp);
	Right.normalize();
	Up = QVector3D::crossProduct(Right, Forward);
	Up.normalize();
	UpdateViewMatrix();
}


Camera::~Camera()
{
}

QMatrix4x4 Camera::UpdateViewMatrix()
{
	QMatrix4x4 view;
	view.lookAt(Position, Position + Forward, WorldUp);
	View = view;
	return view;
}

void Camera::CameraMove(QVector3D pos, double yaw, double pitch)
{
	Position = pos;
	RotationEuler.setX(yaw);
	RotationEuler.setY(pitch);
	RotationEuler.setZ(0.0f);
	Forward.setX(qCos(qDegreesToRadians(pitch))* qSin(qDegreesToRadians(yaw)));
	Forward.setY(qSin(qDegreesToRadians(pitch)));
	Forward.setZ(qCos(qDegreesToRadians(pitch))*qCos(qDegreesToRadians(yaw)));
	Forward.normalize();

	Right = QVector3D::crossProduct(Forward, WorldUp);
	Right.normalize();
	Up = QVector3D::crossProduct(Right, Forward);
	Up.normalize();
	UpdateViewMatrix();
}
