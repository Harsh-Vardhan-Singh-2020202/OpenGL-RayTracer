#ifndef _RENDERENGINE_H_
#define _RENDERENGINE_H_

#include "world.h"
#include "camera.h"

class RenderEngine
{
private:
	World *world;
	Camera *camera;
	const Color trace(const int i, const int j, const int samples);
    int samplesPerPixel; // Number of samples per pixel (n)

public:
	RenderEngine(World *_world, Camera *_camera, int samples):
		world(_world), camera(_camera), samplesPerPixel(samples) {}
	bool renderLoop();
};
#endif
