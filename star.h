#ifndef STAR_H
#define STAR_H

#include "sphere.h"

class Star : public Sphere
{
public:
    Star(int x, int y, int z, double radius, double lightIntesity)
        :Sphere(x, y, z, radius)
    {this->lightIntensity = lightIntesity;}

private:
    double lightIntensity;
    double mass;
    QVector3D velocity;
};

#endif // STAR_H
