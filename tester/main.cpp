#include "quaternion.h"
#include "vector.h"

int main()
{
    Vector3 v(1, 0, 0);
    Quat rotate;
    rotate.SetRotationZ(angle2radian(90));
    Quat u;
    u = rotate.Inverse() * Quat(0, v) * rotate;
    u.Display();
    return 0;
}