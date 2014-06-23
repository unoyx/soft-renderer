#include "quaternion.h"
#include "vector.h"

int main()
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


    return 0;
}