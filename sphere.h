#ifndef SPHERE_H
#define SPHERE_H

#include <QVector3D>
#include <vector>
#include <cmath>
#include <QImage>

class Sphere
{
public:
    Sphere();
    Sphere(int,int,int,double);
    std::vector<QVector3D> getPoints();
    QVector3D MappingPoint(int);
    QVector3D MappingPoint(QVector3D);
    std::vector<QVector3D> getPoints(double,double,double,double,double,double);
    QVector3D getPosition();
    QImage* getImage();
    void setImage(QString);
    void setPosition(QVector3D);
    void setPosition(int,int,int);

protected:
    std::vector<QVector3D> points;
    double radius;
    QVector3D position;
    QImage* img;

    void doPoints();
    std::vector<QVector3D> scaling(double,std::vector<QVector3D>);
    std::vector<QVector3D> rotation(int,double,std::vector<QVector3D>);
    std::vector<QVector3D> shearing(double,double,std::vector<QVector3D>);


};

#endif // SPHERE_H
