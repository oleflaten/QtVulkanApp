#include "VkCamera.h"

VkCamera::VkCamera() {}

void VkCamera::init()
{
    mProjectionMatrix.setToIdentity();
    mViewMatrix.setToIdentity();
}
void VkCamera::perspective(int degrees, double aspect, double nearplane, double farplane)
{
    mProjectionMatrix.setToIdentity();
    mProjectionMatrix.perspective(degrees, aspect, nearplane, farplane);
    mProjectionMatrix.scale(1.0f, -1.0f, 1.0f);
}

void VkCamera::lookAt(const QVector3D &eye, const QVector3D &at, const QVector3D &up)
{
    mEye = eye;
    mAt = at;
    mUp = up;
    mViewMatrix.setToIdentity();
    mViewMatrix.lookAt(mEye, mAt, mUp);
}

void VkCamera::translate(float dx, float dy, float dz)
{
    mViewMatrix.translate(dx, dy, dz);
    //Flip projection because of Vulkan's -Y axis
    // mViewMatrix.scale(1.0f, -1.0f, 1.0f);
}

void VkCamera::rotate(float t, float x, float y, float z)
{
    mViewMatrix.rotate(t,x,y,z);
}

QMatrix4x4 VkCamera::cMatrix()
{
    return mProjectionMatrix * mViewMatrix;
}
