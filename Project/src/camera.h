#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "vector3D.h"
#include "color.h"
#include "ray.h"

class Camera
{
private:
	Vector3D position;
	Vector3D target; //Look-at point
	Vector3D up;

	Vector3D line_of_sight;
	Vector3D u, v, w; //Camera basis vectors

	unsigned char *bitmap;
	int width, height;
	float fovy;// expressed in degrees: FOV-Y; angular extent of the height of the image plane
	float focalDistance; //Distance from camera center to the image plane
	float focalWidth, focalHeight;//width and height of focal plane
	float aspect;

    bool IsRenderingObject; // To check if object is being rendered

	int samplesPerPixel; // Number of samples per pixel (n)

public:
	Camera(const Vector3D& _pos, const Vector3D& _target, const Vector3D& _up, float fovy, int w, int h, int n);
	~Camera();
	const Vector3D get_ray_direction(const int i, const int j, int sampleIndex) const;
	const Vector3D& get_position() const { return position; }
	void drawPixel(int i, int j, Color c);
	unsigned char * getBitmap() {return bitmap; }
	int getWidth() {return width;}
	int getHeight(){return height;}
    void setIsRenderingObject(bool value){IsRenderingObject = value;}

};
#endif
