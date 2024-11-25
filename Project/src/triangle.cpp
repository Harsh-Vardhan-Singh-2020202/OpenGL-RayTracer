//triangle.cpp

#include "triangle.h"

bool Triangle::intersect(Ray& r) const
{
    // Calculate edges and vectors
    Vector3D edge1 = vertex2 - vertex1;
    Vector3D edge2 = vertex3 - vertex1;
    Vector3D h = crossProduct(r.getDirection(), edge2);
    double a = dotProduct(edge1, h);

    // Check if the ray is parallel to the triangle
    if (a > -SMALLEST_DIST && a < SMALLEST_DIST)
    {
        camera->setIsRenderingObject(false);
        return false;
    }

    double f = 1.0 / a;
    Vector3D s = r.getOrigin() - vertex1;
    double beta = f * dotProduct(s, h);

    if (beta < 0.0 || beta > 1.0)
        return false;

    Vector3D q = crossProduct(s, edge1);
    double gamma = f * dotProduct(r.getDirection(), q);

    if (gamma < 0.0 || beta + gamma > 1.0)
        return false;

    double t = f * dotProduct(edge2, q);

    if (beta > 0.0 && gamma > 0.0 && beta + gamma < 1.0)
    {
        r.setLevel(r.getLevel() + 1);

        if (t > SMALLEST_DIST && t < r.getParameter())
        {
            r.setParameter(t, this);
            Vector3D normal = crossProduct(edge2, edge1);
            normal.normalize();
            r.setNormal(normal);
            camera->setIsRenderingObject(true);
            return true;
        }
        else
        {
            camera->setIsRenderingObject(false);
        }
    }
    else
    {
        camera->setIsRenderingObject(false);
    }

    return false;
}
