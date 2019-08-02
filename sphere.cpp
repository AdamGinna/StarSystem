#include "sphere.h"


Sphere::Sphere()
{

}

Sphere::Sphere(int x,int y,int z,double radius)
{
    this->radius = radius;
    setPosition(x,y,z);
    //img = new QImage(":/earth_daymap.jpg");
    doPoints();
}

QImage* Sphere::getImage()
{
    return img;
}

void Sphere::setImage(QString a)
{
    img = new QImage(a);
}

std::vector<QVector3D> Sphere::getPoints()
{
    doPoints();
    return points;
}

QVector3D Sphere::MappingPoint(int i)
{
    QVector3D point = points[i];
    point.normalize();
    int u,v;
    u = (((double)atan2(point.z(),point.x()) / (2*3.14)) + 0.5)*(img->width()-1);
    v = (0.5 - (double)asin((double)point.y()/3.14))*(img->height()-1);

    return QVector3D((u)%(img->width()-1),(v)%(img->height()-1),points[i].z());

}

QVector3D Sphere::MappingPoint(QVector3D point)
{
    point.normalize();
    int u,v;
    u = (((double)atan2(point.z(),point.x()) / (2*3.14)) + 0.5)*(img->width()-1);
    v = (0.5 - (double)asin((double)point.y()/3.14))*(img->height()-1);

    return QVector3D(u,v,point.z());

}

std::vector<QVector3D> Sphere::getPoints(double scale,double alpha0 ,double alpha1, double alpha2,double shX, double shY)
{
    doPoints();
    std::vector<QVector3D> poi = points;
    QVector3D pos = this->getPosition();
    poi = shearing(shX,shY, scaling(scale,rotation(2,alpha2,rotation(1,alpha1,rotation(0,alpha0,poi)))));
    for(int i=0;i<poi.size();i++)
    {
        poi[i].setX( poi[i].x() + pos.x());
        poi[i].setY(poi[i].y() + pos.y());
        poi[i].setZ(poi[i].z() + pos.z());
    }
    return poi;
}

void Sphere::doPoints()
{
    points.clear();
    std::vector<QVector3D> poi;
    QVector3D p;


    for(double alpha = 0; alpha<=6.28 + 3.14/9 ;alpha+=3.14/18)
    for(double theta = 0; theta<=3.14 + 3.14/18;theta+=3.14/18)
    {
    p.setX(radius*cos(alpha)*sin(theta));
    p.setY(radius*sin(theta)*sin(alpha));
    p.setZ(radius*cos(theta));
            points.push_back(p);
    }

}

QVector3D Sphere::getPosition()
{
    return position;
}

void Sphere::setPosition(QVector3D pos)
{
    position = pos;
}

void Sphere::setPosition(int x,int y,int z)
{
    position.setX(x);
    position.setY(y);
    position.setZ(z);
}

std::vector<QVector3D> Sphere::scaling(double scale,std::vector<QVector3D> points)
{
    for(int i =0;i<points.size();i++)
    {
        points[i].setX(points[i].x()*scale);
        points[i].setY(points[i].y()*scale);
        points[i].setZ(points[i].z()*scale);
    }
    return points;
}

std::vector<QVector3D> Sphere::rotation(int mode, double alpha, std::vector<QVector3D> points)
{
    QVector3D p;
    std::vector<QVector3D> poi;
    for (int i=0;i<points.size();i++) {

         p = points[i];

        switch (mode) {
        case 0:
        {
            p.setY(points[i].y()*std::cos(alpha)-points[i].z()*std::sin(alpha));
            p.setZ(points[i].y()*std::sin(alpha)+points[i].z()*std::cos(alpha));
            break;
        }
        case 1:
        {
            p.setX(points[i].x()*std::cos(alpha)+points[i].z()*std::sin(alpha));
            p.setZ((-1)*points[i].x()*std::sin(alpha)+points[i].z()*std::cos(alpha));
            break;
        }
        case 2:
        {
            p.setX(points[i].x()*std::cos(alpha)-points[i].y()*std::sin(alpha));
            p.setY(points[i].x()*std::sin(alpha)+points[i].y()*std::cos(alpha));
            break;
        }         
        }
        poi.push_back(p);
    }
    return poi;

}

std::vector<QVector3D> Sphere::shearing(double shX, double shY, std::vector<QVector3D> points)
{
    QVector3D *p;
    for (int i=0;i<points.size();i++) {

        p = &points[i];
        p->setX(p->x()+shX*p->z());
        p->setY(p->y()+shY*p->z());
    }
    return points;
}


/*
QColor Sphere::diffuse(QVector3D * r, float t){
    QColor rgb(0.0f, 0.0f, 0.0f);
    for (int a = 0.0f; a < plights.size(); a++){

        QVector3D surfacepoint = r->source + t*(r->direction);
        QVector3D l = plights[a].source - surfacepoint;
        l.normalize();

        QVector3D n = surfacepoint - center;
        n.normalize();

        float result = n.dot(l);
        rgb.setRgb( rgb.red()+ std::max(result, 0.0f)*diffusecoef[0] * plights[a].red
        ,rgb.green() + std::max(result, 0.0f)*diffusecoef[1] * plights[a].green
        ,rgb.blue() + std::max(result, 0.0f)*diffusecoef[2] * plights[a].blue)
    }
    return rgb;
}

*/

