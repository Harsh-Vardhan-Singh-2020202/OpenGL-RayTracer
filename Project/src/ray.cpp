#include "ray.h"
#include "transformMatrix.h"

bool Ray::setParameter(const float par, const Object *obj)
{
	if(par < t && par > SMALLEST_DIST)
	{
		hit = true;
		t = par;
		object = obj;
		return true;
	}
	return false;
}

void Ray::transform(const TransformMatrix& matrix)
{
    origin = matrix * origin;
    direction = matrix * (direction * t);
    normal = matrix * (normal);
    direction.normalize();
    normal.normalize();
}