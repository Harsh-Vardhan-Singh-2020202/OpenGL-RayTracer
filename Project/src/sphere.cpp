//sphere.cpp

#include "sphere.h"

bool Sphere::intersect(Ray& r) const
{
	Vector3D centerVector = r.getOrigin() - position;
	double a = 1.0;
	double b = 2*dotProduct(r.getDirection(), centerVector);
	double c = dotProduct(centerVector, centerVector) - radius*radius;
	double discriminant = b*b - 4.0*a*c;

	//now check if discrete is positive or zero, then only we have an intersection!
	if(discriminant >=0.0)
	{
        r.setLevel(r.getLevel() + 1);

		if(discriminant == 0)
		{
            camera->setIsRenderingObject(true);
			double t;
			t = -b/(2.0*a);
			r.setParameter(t, this);
            Vector3D hitPoint = r.getOrigin() + t * r.getDirection();
            Vector3D normal = (position - hitPoint);
            normal.normalize();
            r.setNormal(normal);
			return true;
		}
		else
		{
            camera->setIsRenderingObject(true);
			//Calculate both roots
			double D = sqrt(discriminant);
			double t1 = (-b +D)/(2.0*a);
			double t2 = (-b -D)/(2.0*a);

			bool b1 = r.setParameter(t1, this);
			bool b2 = r.setParameter(t2, this);

            // Calculate normals for both intersection points
            Vector3D hitPoint1 = r.getOrigin() + t1 * r.getDirection();
            Vector3D normal1 = (position - hitPoint1);
            normal1.normalize();

            Vector3D hitPoint2 = r.getOrigin() + t2 * r.getDirection();
            Vector3D normal2 = (position - hitPoint2);
            normal2.normalize();

            if(b1 & b2)
                r.setNormal(normal2);
            else if(b1 & !b2)
                r.setNormal(normal1);
            else if(!b1 & !b2)
                r.setNormal(normal2);
            else
                r.setNormal(Vector3D(0,0,0));

            return b1||b2;
		}
	}
    else
    {
        camera->setIsRenderingObject(false);
    }
	return false;

}
