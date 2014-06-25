#include "quaternion.h"
#include "vector.h"
#include "Camera.h"

void fun_Quat_test(void)
{
    Vector3 v(1, 1, 1);
    Quat rotate; //= Quat::GetIdentity();
    rotate.SetRotationY(-90);
    //    v = RotateByQuat(v, rotate);
    v.Display();

    Matrix33 m;
    m = rotate.Inverse().GetMatrix33();
    m.Display();
    Matrix33 n;
    n = rotate.GetMatrix33();
    n.Display();

    (v * m).Display();
    (v * n).Display();
}

void fun_Camera_test(void)
{
    Camera c(Vector3(0, 0, 0), Quat::GetIdentity(), 1.0f, 0.5f, 90.0f, 1.0f);

    Matrix44 model_view = c.GetModelViewMatrix();
    Matrix44 perspective = c.GetPerpectivMatrix();

    Vector3 pos(-0.5, 0.5, 0.5);
    Vector3 res = pos * model_view;
    res.Display();

    Vector4 pos_1;
    pos_1.SetVector3(res);
    pos_1.w = 1.0f;
    pos_1.Display();
    (pos_1 * perspective).Display(); // ×óÉÏ

    Vector4 pos_2(-0.5, -0.5, 0.5, 1.0f);
    (pos_2 * perspective).Display(); // ×óÏÂ

    Vector4 pos_3(1, 1, 1, 1);
    (pos_3 * perspective).Display();
}

int main()
{
    fun_Camera_test();

    return 0;
}

