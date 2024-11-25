//main.cpp

#include "imgui_setup.h"
#include "camera.h"
#include "renderengine.h"
#include "world.h"
#include "material.h"
#include "object.h"
#include "sphere.h"
#include "triangle.h"
#include "transformedSurface.h"
#include "lightsource.h"
#include "pointlightsource.h"
#include "transformMatrix.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../depends/stb/stb_image.h"
#include "../depends/stb/stb_image_write.h"

// Number of columns to render in a single go. Increase to gain some display/render speed!
#define RENDER_BATCH_COLUMNS 50

#include <iostream>
#include <ostream>
using namespace std;

Camera *camera;
RenderEngine *engine;

// This is window size, used to display scaled ray traced image.
int screen_width = 800, screen_height = 600;
// This is ray traced image size. Change it if needed.
int image_width = 3840, image_height = 2160;
GLuint texImage;

// Function to assign material
Material* assignMaterial(World *world, Camera *camera, Color _color, float _ka, float _kd, float _ks, float _kr, float _kt, float _eta, float _C, int _n, bool _dp)
{
    Material *mat = new Material(world, camera);
    mat->color = _color;
    mat->ka = _ka;
    mat->kd = _kd;
    mat->ks = _ks;
    mat->kr = _kr;
    mat->kt = _kt;
    mat->eta = _eta;
    mat->C = _C;
    mat->n = _n;
    mat->dp = _dp;
    return mat;
}

// Main code
int main(int, char**)
{
    // Setup window
    GLFWwindow *window = setupWindow(screen_width, screen_height);

    ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    // Set some constant parameters
    float ambient = 0.25f;
    bool depthMap = false;

    // Selecting ray tracer type and selecting scene respectively
    int choice1, choice2;

    // Simple user interface 1
    std::cout << "" << std::endl;
    std::cout << "1. Normal ray tracer" << std::endl;
    std::cout << "2. Depth map" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Select Ray Tracing Type: ";
    std::cin >> choice1;

    // Option to select rendering type
    switch (choice1)
    {
        case 1: depthMap = false; break;
        case 2: depthMap = true; break;
        default:std::cout << "Invalid choice." << std::endl; break;
    }

    // Create a world
    World *world = new World;
    world->setAmbient(Color(1));
    if(!depthMap)
        world->setBackground(Color(0.53, 0.81, 0.92));
    else
        world->setBackground(Color(0.0, 0.0, 0.0));

    // Setup raytracer camera. This is used to spawn rays.
    Vector3D camera_position(0, 0, 10);
    Vector3D camera_target(0, 0, 0); //Looking down -Z axis
    Vector3D camera_up(0, 1, 0);
    float camera_fov_y =  45;
    int samplesPerPixel = 4; // jittered super sampling
    camera = new Camera(camera_position, camera_target, camera_up, camera_fov_y, image_width, image_height, samplesPerPixel);

    // Create materials
    Material *mat1 = assignMaterial(world, camera, Color(1.0, 0.1, 0.1), ambient, 0.75, 0.50, 0.00, 0.00, 0.00, 0.00, 64, depthMap); // SIMPLE RED
    Material *mat2 = assignMaterial(world, camera, Color(0.1, 1.0, 0.1), ambient, 0.75, 0.50, 0.00, 0.00, 0.00, 0.00, 64, depthMap); // SIMPLE GREEN
    Material *mat3 = assignMaterial(world, camera, Color(0.1, 0.1, 1.0), ambient, 0.75, 0.50, 0.00, 0.00, 0.00, 0.00, 64, depthMap); // SIMPLE BLUE
    Material *mat4 = assignMaterial(world, camera, Color(0.0, 0.0, 0.0), ambient, 0.05, 0.75, 0.80, 0.00, 0.00, 0.00, 64, depthMap); // MIRROR
    Material *mat5 = assignMaterial(world, camera, Color(0.2, 0.2, 0.2), ambient, 0.05, 0.75, 0.05, 0.85, 1.52, 0.05, 64, depthMap); // GLASS
    Material *mat6 = assignMaterial(world, camera, Color(0.2, 0.2, 0.2), ambient, 0.05, 0.75, 0.25, 0.85, 0.65, 0.00, 64, depthMap); // LIGHTER AIR
    Material *mat7 = assignMaterial(world, camera, Color(0.2, 0.2, 0.2), ambient, 0.05, 0.75, 0.05, 0.85, 1.52, 0.25, 64, depthMap); // IMPURE GLASS

    //Create light sources

    // Point Lights
    LightSource *light1 = new PointLightSource(world, Vector3D(5.0, 5.0, -0.5), Color(0.750, 0.750, 0.750)); // mild white light
    LightSource *light2 = new PointLightSource(world, Vector3D(0.0, 10.0, -5.0), Color(0.992, 0.984, 0.827)); // sunlight

    // Create transform matrices

    // Creating a transformation matrix 1
    TransformMatrix* transformationMatrix = new TransformMatrix();
    // Initialize the transformation matrix as needed
    *transformationMatrix = transformationMatrix->Identity();
    *transformationMatrix = *transformationMatrix * transformationMatrix->Scaling(2.0, 1.0, 1.0);
    *transformationMatrix = *transformationMatrix * transformationMatrix->RotationZ(25);

    // Create objects

    // Spheres
    Object *sphere1 = new Sphere(Vector3D(0.0, 0.0, -5.0),1.5,mat1, camera);
    Object *sphere2 = new Sphere(Vector3D(-4.0, 0.0, -5.0),1.5,mat2, camera);
    Object *sphere3 = new Sphere(Vector3D(8.0, 0.0, -15.0),1.5,mat3, camera);
    Object *sphere4 = new Sphere(Vector3D(0.0, 0.0, -5.0),1.5,mat4, camera);
    Object *sphere5 = new Sphere(Vector3D(0.0, 0.0, -2.5),1.5,mat5, camera);
    Object *sphere6 = new Sphere(Vector3D(0.0, 0.0, -2.5),1.5,mat6, camera);
    Object *sphere7 = new Sphere(Vector3D(0.0, 0.0, -2.5),1.5,mat7, camera);
    Object *sphere8 = new Sphere(Vector3D(0.0, 2.0, -5.0),1.5,mat1, camera);
    Object *sphere9 = new Sphere(Vector3D(0.0, 0.0, -10.0),6,mat1, camera);
    Object *sphere10 = new Sphere(Vector3D(0.0, 0.0, -5.0),2,mat1, camera);

    // Triangles
    Object *triangle1 = new Triangle(Vector3D(-7.0, 4.0, -5.0),Vector3D(-7.0, -6.0, -5.0),Vector3D(1.0, -6.0, -11.0),mat2, camera);
    Object *triangle2 = new Triangle(Vector3D(0.0, 4.0, -4.5),Vector3D(-0.5, 2.5, -4.0),Vector3D(0.5, 2.5, -4.0),mat3, camera);
    Object *triangle3 = new Triangle(Vector3D(5.0, -4.0, -5.0),Vector3D(5.0, 6.0, -5.0),Vector3D(-3.0, 6.0, -11.0),mat4, camera);

    Object *triangle4 = new Triangle(Vector3D(-10.0, 8.0, -5.0),Vector3D(-10.0, -8.0, -5.0),Vector3D(0.0, 0.0, -15.0),mat2, camera);
    Object *triangle5 = new Triangle(Vector3D(10.0, 8.0, -5.0),Vector3D(10.0, -8.0, -5.0),Vector3D(0.0, 0.0, -15.0),mat2, camera);
    Object *triangle6 = new Triangle(Vector3D(-10.0, 8.0, -5.0),Vector3D(10.0, 8.0, -5.0),Vector3D(0.0, 0.0, -15.0),mat2, camera);
    Object *triangle7 = new Triangle(Vector3D(-10.0, -8.0, -5.0),Vector3D(10.0, -8.0, -5.0),Vector3D(0.0, 0.0, -15.0),mat2, camera);

    // Transformed Surfaces
    Object* transformedSurface = new TransformedSurface(sphere10, transformationMatrix, mat1, camera);

    // Simple user interface 2
    std::cout << "" << std::endl;
    std::cout << "1. Triangles in a ray-tracer" << std::endl;
    std::cout << "2. Blinn-Phong shading for the shapes" << std::endl;
    std::cout << "3. Shadows in ray-tracer" << std::endl;
    std::cout << "4. Reflective materials in ray-tracer" << std::endl;
    std::cout << "5. Dielectric materials in ray-tracer" << std::endl;
    std::cout << "6. Total inter reflection in ray-tracer" << std::endl;
    std::cout << "7. Affect of Beer's law in ray-tracer" << std::endl;
    std::cout << "8. Transformed primitives" << std::endl;
    std::cout << "9. Depth map showcase" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Select scene: ";
    std::cin >> choice2;

    // Option to select scene
    switch (choice2)
    {

        case 1: // SCENE 1: Triangles in a ray-tracer.
            // Add objects in the world
            world->addObject(triangle1);

            // Add lights in the world
            world->addLight(light1);

            break;

        case 2: // SCENE 2: Blinn-Phong shading for the shapes.
            // Add objects in the world
            world->addObject(sphere1);

            // Add lights in the world
            world->addLight(light1);
            world->addLight(light2);

            break;

        case 3: // SCENE 3: Shadows in ray-tracer.
            // Add objects in the world
            world->addObject(sphere1);
            world->addObject(triangle1);
            world->addObject(triangle2);

            // Add lights in the world
            world->addLight(light1);
            world->addLight(light2);

            break;

        case 4: // SCENE 4: Reflective materials in ray-tracer.
            // Add objects in the world
            world->addObject(sphere2);
            world->addObject(sphere3);
            world->addObject(sphere4);
            world->addObject(triangle3);

            // Add lights in the world
            world->addLight(light1);

            break;

        case 5: // SCENE 5: Dielectric materials in ray-tracer.
            // Add objects in the world
            world->addObject(sphere8);
            world->addObject(triangle1);
            world->addObject(sphere5);

            // Add lights in the world
            world->addLight(light1);

            break;

        case 6: // SCENE 6: Total inter reflection in ray-tracer.
            // Add objects in the world
            world->addObject(sphere8);
            world->addObject(triangle1);
            world->addObject(sphere6);

            // Add lights in the world
            world->addLight(light1);

            break;

        case 7: // SCENE 7: With beers law applied.
            // Add objects in the world
            world->addObject(sphere8);
            world->addObject(triangle1);
            world->addObject(sphere7);

            // Add lights in the world
            world->addLight(light1);

            break;

        case 8: // SCENE 8: Transformed primitives.
            // Add objects in the world
            world->addObject(transformedSurface);

            // Add lights in the world
            world->addLight(light1);
            world->addLight(light2);

            break;

        case 9: // SCENE 9: Depth map showcase.
            // Add objects in the world
            world->addObject(sphere9);
            world->addObject(triangle4);
            world->addObject(triangle5);
            world->addObject(triangle6);
            world->addObject(triangle7);

            // Add lights in the world
            world->addLight(light1);
            world->addLight(light2);

            break;

        default:
            std::cout << "Invalid choice." << std::endl;
            break;
    }

    // Initializing engine
    engine = new RenderEngine(world, camera, samplesPerPixel);

    //Initialise texture
    glGenTextures(1, &texImage);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texImage);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, camera->getBitmap());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool render_status;
        for(int i=0; i<RENDER_BATCH_COLUMNS; i++)
            render_status = engine->renderLoop(); // RenderLoop() ray traces 1 column of pixels at a time.
        if(!render_status)
        {
            // Update texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texImage);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width, image_height, GL_RGB, GL_UNSIGNED_BYTE, camera->getBitmap());
        }

        ImGui::Begin("Lumina", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Size: %d x %d", image_width, image_height);
        if(ImGui::Button("Save")){
          char filename[] = "img.png";
          stbi_write_png(filename, image_width, image_height, 3, camera->getBitmap(),0);
        }
        //Display render view - fit to width
        int win_w, win_h;
        glfwGetWindowSize(window, &win_w, &win_h);
        float image_aspect = (float)image_width/(float)image_height;
        float frac = 0.95; // ensure no horizontal scrolling
        ImGui::Image((void*)(intptr_t)texImage, ImVec2(frac*win_w, frac*win_w/image_aspect), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteTextures(1, &texImage);

    cleanup(window);

    return 0;
}
