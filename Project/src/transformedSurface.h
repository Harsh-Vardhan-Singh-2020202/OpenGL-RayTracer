// transformedSurface.h
#ifndef _TRANSFORMEDSURFACE_H_
#define _TRANSFORMEDSURFACE_H_

#include "object.h"
#include "ray.h"
#include "vector3D.h"
#include "transformMatrix.h"

class TransformedSurface : public Object
{
private:
    Object* surface; // Reference to the original surface

public:
    TransformMatrix* transform;
    TransformedSurface(Object* obj, TransformMatrix* trans, Material* mat, Camera* cam) :
            Object(mat, cam), surface(obj), transform(trans)
    {
        isSolid = true;
    }

    virtual bool intersect(Ray& r) const;
};

#endif
