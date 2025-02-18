#ifndef CAMERA_H
#define CAMERA_H
#include <QMatrix4x4>

class Camera
{

public:
    Camera();
    ~Camera() {}

    void init();
    void perspective(int degrees, double aspect, double nearplane, double farplane);
    void lookAt(const QVector3D& eye, const QVector3D& at, const QVector3D& up);

    void translate(float dx, float dy, float dz);
    void rotate(float t, float x, float y, float z);
    QMatrix4x4 cMatrix();

    //Oles version
    void pitch(float degrees);
    void yaw(float degrees);
    void updateRightVector();
    void updateForwardVector();
    void update();

    void setPosition(const QVector3D &position);

    void setSpeed(float speed);
    void updateHeigth(float deltaHeigth);
    void moveRight(float delta);

    QVector3D position() const;
    QVector3D up() const;

private:
    QVector3D mEye{0.0, 0.0, 0.0};  // Camera position
    QVector3D mAt{0.0, 0.0, -1.0};   // Forward vector
    QVector3D mUp{0.0, 1.0, 0.0};   // Up vector

    QVector3D mForward{ 0.f, 0.f, -1.f };
    QVector3D mRight{ 1.f, 0.f, 0.f };

    QVector3D mPosition{ 0.f, 0.f, 0.f };
    float mPitch{ 0.f };
    float mYaw{ 0.f };

    float mSpeed{ 0.f }; //camera will move by this speed along the mForward vector

    QMatrix4x4 mProjectionMatrix{};
    QMatrix4x4 mViewMatrix{};

    //QVector3D did not have a rotateVector function
    QVector3D rotateVector(const QVector3D& vector, float angle, const QVector3D& axis);

    QMatrix4x4 mYawMatrix;
    QMatrix4x4 mPitchMatrix;
};

#endif // CAMERA_H
