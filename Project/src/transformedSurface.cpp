//transformedSurface.cpp

#include "transformedSurface.h"

bool TransformedSurface::intersect(Ray& r) const
{
    // Inverse of the transformation matrix to convert to local coordinates
    TransformMatrix invTransform = transform->Inverse();

    // Transform the ray to local coordinates
    r.transform(invTransform);

    // Call the intersect function of the original surface
    bool hit = surface->intersect(r);

    if (hit)
    {
        camera->setIsRenderingObject(true);
        // Transform the intersection point and normal back to global coordinates
        r.transform(*transform);
    }
    else
    {
        camera->setIsRenderingObject(false);
    }

    return hit;
}