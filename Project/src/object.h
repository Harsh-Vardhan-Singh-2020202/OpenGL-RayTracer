//object.h
#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "ray.h"
#include "vector3D.h"
#include "color.h"
#include "material.h"

class Object
{
protected:
    Material *material;
    Camera *camera;
    bool isSolid;
public:
    Object(Material *mat, Camera *c): material(mat), camera(c) {}
    virtual bool intersect(Ray& ray) const = 0;
    virtual Color shade(const Ray& ray) const
    {
        return material->shade(ray, isSolid);
    }
};

#endif
