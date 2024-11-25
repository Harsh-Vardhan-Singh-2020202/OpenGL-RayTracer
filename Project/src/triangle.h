//triangle.h
#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "object.h"
#include "ray.h"
#include "vector3D.h"
#include "color.h"

class Triangle : public Object
{
private:
    // Vertices of the triangle
    Vector3D vertex1;
    Vector3D vertex2;
    Vector3D vertex3;

public:
    Triangle(const Vector3D& v1, const Vector3D& v2, const Vector3D& v3, Material* mat, Camera* cam) :
            Object(mat, cam), vertex1(v1), vertex2(v2), vertex3(v3)
    {
        isSolid = true;
    }

    virtual bool intersect(Ray& r) const;
};
#endif
