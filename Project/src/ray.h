//ray.h
#ifndef _RAY_H_
#define _RAY_H_

#include <float.h>
#include "vector3D.h"
#include "transformMatrix.h"

class Object;

const float SMALLEST_DIST = 1e-4; //Constant used to dismiss intersections very close to previous
class Ray
{
private:
    Vector3D origin;
    Vector3D direction;
    float t; //Distance travelled along the Ray
    bool hit; //has the ray hit something?
    const Object *object;//The object that has been hit
    int level;//Number of times the ray has been traced recursively
    float refractive_index;
    Vector3D normal; //Normal of the hit object

public:


    bool TIR = false;
	Ray(const Vector3D& o, const Vector3D& d, int _level = 0, float _ref_idx = 1.0):
    		origin(o), direction(d), t(FLT_MAX), hit(false), level(_level), refractive_index(_ref_idx)
	{
		direction.normalize();	
	}
	Vector3D getOrigin() const  {return origin;}
	Vector3D getDirection() const  {return direction;}
	Vector3D getPosition() const {return origin + t*direction;}
	Vector3D getNormal() const {return normal;}
	float getParameter() const {return t;}
    float getRefractiveIndex() {return refractive_index;}
    int getLevel() const {return level;}

    bool setParameter(const float par, const Object *obj);
    void setNormal(const Vector3D& n) { normal = n; }
    void setRefractiveIndex(float ri) {refractive_index = ri;}
    void setLevel(int l) { level = l; }


    bool didHit() const {return hit;}
	const Object* intersected() const {return object;}

    void transform(const TransformMatrix& matrix);
};
#endif
