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
    void setAplhaX(double);
    void setAplhaY(double);
    void setAplhaZ(double);

    double getAlphax();

    double getAlphay();

    double getAlphaz();

protected:
    std::vector<QVector3D> points;
    double radius;
    QVector3D position;
    QImage* img;
    double alphax;
    double alphay;
    double alphaz;

    void doPoints();
    std::vector<QVector3D> scaling(double,std::vector<QVector3D>);
    std::vector<QVector3D> rotation(int,double,std::vector<QVector3D>);
    std::vector<QVector3D> shearing(double,double,std::vector<QVector3D>);


};

#endif // SPHERE_H
