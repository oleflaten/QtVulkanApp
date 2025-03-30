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

    void setSpeed(float speed);
    void moveRight(float delta);
    void updateHeigth(float deltaHeigth);
    //QMatrix4x4 cMatrix();

	inline QMatrix4x4 viewMatrix() const { return mViewMatrix; }
	inline QMatrix4x4 projectionMatrix() const { return mProjectionMatrix; }

    void update();
    void setPosition(const QVector3D& position);
    inline QVector3D position() const { return mPosition; }

    void pitch(float degrees);
    void yaw(float degrees);

    inline void setViewMatrix(const QMatrix4x4 &newViewMatrix){ mViewMatrix = newViewMatrix; }
    inline void setProjectionMatrix(const QMatrix4x4 &newProjectionMatrix){ mProjectionMatrix = newProjectionMatrix; }

private:
    QVector3D mEye{0.0, 0.0, 0.0};  // Camera position
    QVector3D mAt{0.0, 0.0, -1.0};   // Forward vector
    QVector3D mUp{0.0, 1.0, 0.0};   // Up vector

    QMatrix4x4 mProjectionMatrix{};
    QMatrix4x4 mViewMatrix{};

    QVector3D mPosition{ 0.f, 0.f, 0.f };
    float mPitch{ 0.f };
    float mYaw{ 0.f };

    float mSpeed{ 0.f }; //camera will move by this speed


};

#endif // CAMERA_H
