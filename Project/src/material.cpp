#include "world.h"
#include "camera.h"
#include "material.h"
#include "lightsource.h"
#include "pointlightsource.h"

#include <typeinfo>
#include "sphere.h"
#include "triangle.h"
#include "transformedSurface.h"

#include <iostream>
#include <ostream>
using namespace std;

Color reflect(Ray incident, Vector3D hitPointPosition, Vector3D normal, Color finalColor,
              int max_recursion_depth, float kr, float shadow_factor,
              World *_world)
{
    // Calculate effect due to reflection
    if (kr > 0 && incident.getLevel() < max_recursion_depth)
    {
        // Calculate the reflected ray direction
        Vector3D reflectedDirection = incident.getDirection() - 2 * (dotProduct(incident.getDirection(), normal)) * normal;
        reflectedDirection.normalize();

        // Create a reflected ray
        Ray reflectedRay(hitPointPosition, reflectedDirection);
        reflectedRay.setLevel(incident.getLevel()+1);

        // Trace the reflected ray and accumulate its color
        Color reflectedColor = _world->shade_ray(reflectedRay);
        finalColor = finalColor + reflectedColor * kr * shadow_factor;
    }

    return finalColor;
}

Color refract(Ray incident, Vector3D hitPointPosition, Vector3D normal, Color finalColor,
              int max_recursion_depth, float kt, float kr, float C, float eta, float shadow_factor,
              World* world)
{
    if (kt > 0 && incident.getLevel() < max_recursion_depth)
    {
        // Storing reflected rays values
        Color reflectedColor= reflect(incident, hitPointPosition, normal, finalColor, max_recursion_depth, kr, shadow_factor, world);

        // Calculate the angle of incidence and refractive indices
        float cosTheta = -dotProduct(normal,incident.getDirection());

        // Get the refractive indices
        float nT = eta;
        float n = incident.getRefractiveIndex();

        // For beer's law defining attenuation parameter
        float attenuation = 0.0;

        if (cosTheta < 0)
        {
            // Ray is exiting the material so indices need to swap and signs need to change
            //std::swap(n, nT);
            n = nT;
            cosTheta = -cosTheta;
            attenuation = 1.0;
        }
        else
        {
            // Ray is entering the material
            n = 1.0;  // Change the refractive index back to air
            attenuation = exp(-C*incident.getParameter());
        }

        float etaRatio = (n/nT);
        float sinTheta = sqrt(1-(cosTheta * cosTheta));
        float sinPhi = etaRatio * sinTheta;
        float cosPhi = sqrt(1-(sinPhi * sinPhi));

        // Computing critical angle
        float ca = 1.0 - (etaRatio * etaRatio * sinTheta * sinTheta);

        // Check for total internal reflection
        if (ca < 0)
        {
            finalColor = reflectedColor;
        }
        else
        {
            // Calculate the refracted ray direction
            Vector3D refractedDirection = etaRatio * incident.getDirection() + (etaRatio * cosTheta - sqrt(ca)) * normal;

            // Create the refracted ray
            Ray refractedRay(hitPointPosition, refractedDirection);
            refractedRay.setLevel(incident.getLevel() + 1);
            refractedRay.setRefractiveIndex(eta);

            // Trace the refracted ray and accumulate its color
            Color refractedColor = world->shade_ray(refractedRay);

            // Apply Schlick Approximation
            float R0 = pow(((nT - 1)/(nT + 1)),2);
            float R = R0 + (1-R0)*pow((1-cosTheta),5);

            // Combine the refracted color with attenuation
            finalColor = finalColor + (reflectedColor*R + refractedColor*(1-R)) * attenuation * kt * shadow_factor;

        }
    }

    return finalColor;
}

Color Material::shade(const Ray& incident, const bool isSolid) const
{
    // The final color which will be returned
    Color finalColor = Color(0.0,0.0,0.0);

    if(!dp)
    {
        int max_recursion_depth = 8;

        Color objectColor   = color;
        Color ambientColor  = Color(0.0,0.0,0.0);
        Color diffuseColor  = Color(0.0,0.0,0.0);
        Color specularColor = Color(0.0,0.0,0.0);

        // Calculate the normal
        Vector3D normal = incident.getNormal();
        normal.normalize();

        // Get hit point
        Vector3D hitPointPosition = incident.getOrigin() + incident.getParameter() * incident.getDirection();

        // Calculate the view direction from the camera
        Vector3D viewDirection = hitPointPosition - camera->get_position();
        viewDirection.normalize();

        // Keep a color to aggregate the effect of all lights
        Color totalLightColor = Color(0.0,0.0,0.0);

        // We obtain the list of light sources from the World class
        const std::vector<LightSource*>& lightSources = world->getLightSources();

        // Iterate through the light sources
        for (const LightSource* lightSource : lightSources)
        {
            if (const PointLightSource* pointLight = dynamic_cast<const PointLightSource*>(lightSource))
            {
                // Get the position of the point light source
                Vector3D lightPos = pointLight->getPosition();

                // Get the color of light
                Color lightColor = pointLight->getIntensity();

                // Calculate the direction from the intersection point to the light source
                Vector3D lightDirection = hitPointPosition - lightPos;
                lightDirection.normalize();

                // Check for shadows
                Ray shadowRay(hitPointPosition, -lightDirection);
                bool inShadow = false;

                for (const Object* object : world->getObjectList())
                {
                    if (dynamic_cast<const Sphere*>(object) != nullptr)
                    {
                        // The object is a Sphere
                        const Sphere* sphere = dynamic_cast<const Sphere*>(object);
                        if (sphere->intersect(shadowRay))
                        {
                            // The shadow ray intersects an object; the point is in shadow
                            inShadow = true;
                            break;
                        }
                    }
                    else if (dynamic_cast<const Triangle*>(object) != nullptr)
                    {
                        // The object is a Triangle
                        const Triangle* triangle = dynamic_cast<const Triangle*>(object);
                        if (triangle->intersect(shadowRay))
                        {
                            // The shadow ray intersects an object; the point is in shadow
                            inShadow = true;
                            break;
                        }
                    }
                    else if (dynamic_cast<const TransformedSurface*>(object) != nullptr)
                    {
                        // The object is a TransformedSurface
                        const TransformedSurface* transformedSurface = dynamic_cast<const TransformedSurface*>(object);

                        // Access the transform matrix
                        const TransformMatrix* transformMatrix = transformedSurface->transform;

                        shadowRay.transform(*transformMatrix);

                        if (transformedSurface->intersect(shadowRay))
                        {
                            // The shadow ray intersects an object; the point is in shadow
                            inShadow = false;
                            break;
                        }
                    }
                }

                if(inShadow)
                {
                    // (0[full effect] to 1[no effect])
                    float shadow_factor = 1.0;

                    // Calculate effect due to reflection in lit region
                    finalColor = reflect(incident, hitPointPosition, normal, finalColor,
                                         max_recursion_depth, kr, shadow_factor,
                                         world);

                    // Calculate effect due to refraction in lit region
                    finalColor = refract(incident, hitPointPosition, normal, finalColor,
                                         max_recursion_depth, kt, kr, C, eta, shadow_factor,
                                         world);

                    // Managing softer shadows for dielectric objects
                    if(kt>0)
                    {

                    }

                    // Accumulate the color of the current light into the totalLightColor
                    totalLightColor = totalLightColor + objectColor * lightColor;
                }

                else
                {
                    // no affect due to shadows so parameter is fixed at 1.0
                    float shadow_factor = 1.0;

                    // Calculate effect due to reflection in lit region
                    finalColor = reflect(incident, hitPointPosition, normal, finalColor,
                                         max_recursion_depth, kr, shadow_factor,
                                         world);

                    // Calculate effect due to refraction in lit region
                    finalColor = refract(incident, hitPointPosition, normal, finalColor,
                                         max_recursion_depth, kt, kr, C, eta, shadow_factor,
                                         world);

                    // Calculate half vector
                    Vector3D halfVector = (lightDirection + viewDirection);
                    halfVector.normalize();

                    // Accumulate the color of the current light into the totalLightColor
                    totalLightColor = totalLightColor + objectColor * lightColor;

                    // Calculate Diffuse lighting (affected by object's color)
                    Color diffuseColor = objectColor * lightColor * kd * std::max(dotProduct(lightDirection, normal), 0.0);

                    // Calculate Specular lighting (not affected by object's color)
                    Color specularColor = lightColor * ks * (pow(std::max(dotProduct(normal, halfVector), 0.0), n));

                    // Combine Diffuse, Specular
                    finalColor = finalColor  + diffuseColor + specularColor;
                }
            }
        }

        // Calculate Ambient lighting
        ambientColor = totalLightColor * ka;

        // Set ambient in the world
        world->setAmbient(ambientColor);

        // Finally add the amalgamated ambient lighting
        finalColor = finalColor + ambientColor;
    }
    else
    {
        // Initialize min and max depth
        float minDepth = 0;
        float maxDepth = 25;
        // Later on depth can be fixed dynamically by calculating nearest and furthest point

        // Calculate your depth value, which should be a floating-point value
        float depth = incident.getParameter();

        // Normalize the depth value to the range [0, 1]
        float depthNormalized = std::max(0.0f, std::min(1.0f, (depth - minDepth) / (maxDepth - minDepth)));

        // Convert the normalized depth value to an 8-bit integer
        uint8_t depth8Bit = static_cast<uint8_t>(depthNormalized * 1);

        // Create a grayscale color based on the depth
        Color depthGreyScale(1-depthNormalized, 1-depthNormalized, 1-depthNormalized);

        finalColor = finalColor + depthGreyScale;
    }

    return finalColor;
}