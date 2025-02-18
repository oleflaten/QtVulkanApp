#include "Camera.h"

Camera::Camera() {}

void Camera::init()
{
    mProjectionMatrix.setToIdentity();
    mViewMatrix.setToIdentity();
}
void Camera::perspective(int degrees, double aspect, double nearplane, double farplane)
{
    mProjectionMatrix.setToIdentity();
    mProjectionMatrix.perspective(degrees, aspect, nearplane, farplane);
    //Flip projection because of Vulkan's -Y axis
    mProjectionMatrix.scale(1.0f, -1.0f, 1.0f);
}

void Camera::lookAt(const QVector3D &eye, const QVector3D &at, const QVector3D &up)
{
    mEye = eye;
    mAt = at;
    mUp = up;
    mViewMatrix.setToIdentity();
    mViewMatrix.lookAt(mEye, mAt, mUp);
}

//Translate camera in world coordinates
void Camera::translate(float dx, float dy, float dz)
{
    mViewMatrix.translate(dx, dy, dz);
}

void Camera::rotate(float t, float x, float y, float z)
{
    mViewMatrix.rotate(t,x,y,z);
}

QMatrix4x4 Camera::cMatrix()
{
    return mProjectionMatrix * mViewMatrix;
}

//Oles version
void Camera::pitch(float degrees)
{
    //  rotate around mRight
    mPitch -= degrees;
    updateForwardVector();
}

void Camera::yaw(float degrees)
{
    // rotate around mUp
    mYaw -= degrees;
    updateForwardVector();
}

void Camera::updateRightVector()
{
    mRight = QVector3D::crossProduct(mForward, mUp);
    mRight.normalize();
    //    qDebug() << "Right " << mRight;
}

void Camera::updateForwardVector()
{
    mRight = QVector3D(1.f, 0.f, 0.f);
    mRight = rotateVector(mRight, mYaw, QVector3D(0.f, 1.f, 0.f));
    mRight.normalize();
    mUp = QVector3D(0.f, 1.f, 0.f);
    mUp = rotateVector(mUp, mPitch, QVector3D(1.f, 0.f, 0.f));
    mUp = rotateVector(mUp, mYaw, QVector3D(0.f, 1.f, 0.f));
    mUp.normalize();
    mForward = QVector3D::crossProduct(mUp, mRight);

    updateRightVector();
}

QVector3D Camera::rotateVector(const QVector3D& vector, float angle, const QVector3D& axis)
{
    QMatrix4x4 rotationMatrix;
    rotationMatrix.setToIdentity();
    rotationMatrix.rotate(angle, axis);

    return rotationMatrix * vector;
}

void Camera::update()
{
    mYawMatrix.setToIdentity();
    mPitchMatrix.setToIdentity();

    mPitchMatrix.rotate(mPitch, 1.f, 0.f, 0.f);
    mYawMatrix.rotate(mYaw, 0.f, 1.f, 0.f);

    mPosition -= mForward * mSpeed;

    mViewMatrix = mPitchMatrix * mYawMatrix;
    mViewMatrix.translate(-mPosition);
}

void Camera::setPosition(const QVector3D &position)
{
    mPosition = position;
    update();
}

void Camera::setSpeed(float speed)
{
    mSpeed = speed;
}

void Camera::updateHeigth(float deltaHeigth)
{
    mPosition.setY(mPosition.y() + deltaHeigth);
}

void Camera::moveRight(float delta)
{
    //This fixes a bug in the up and right calculations
    //so camera always holds its height when straifing
    //should be fixed thru correct right calculations!
    QVector3D right{ mRight };
    right.setY(0.f);
    mPosition += right * delta;
}

QVector3D Camera::position() const
{
    return mPosition;
}

QVector3D Camera::up() const
{
    return mUp;
}
