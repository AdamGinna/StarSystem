 #include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    szer = ui->frame->width();
    wys = ui->frame->height();
    poczX = ui->frame->x();
    poczY = ui->frame->y();
    img = new QImage(szer,wys,QImage::Format_RGB32);
    img->fill(0);

    cameraPosition.setX(0);
    cameraPosition.setY(0);
    cameraPosition.setZ(-1000);


    transformation();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    p.drawImage(poczX,poczY,*img);

}

void MainWindow::transformation()
{
    delete img;
    img = new QImage(szer,wys,QImage::Format_RGB32);
    img->fill(0);


    int index = ui->spinBox_AO->value();
    std::vector<Sphere*> objects1 = objects;
     std::sort(objects1.begin(),objects1.end(),MainWindow::compareSphere);
    for(int i =0; i<objects1.size();i++)
    {
       Sphere *sphere = objects1[i];
       prepareImage( img,sphere->getPoints(),sphere);
    }
    //prepareImage( img,sun->getPoints(scale,alphax,alphay,alphaz,shX,shY),sun);
    update();

}

std::vector<QVector3D> MainWindow::projection(std::vector<QVector3D> points)
{
    QVector3D p;
    for (int i = 0;i<points.size();i++)
    {
        p = points[i];
        if(p.z()>0)
        {
        points[i].setX((p.x()*cameraPosition.z())/(p.z()-cameraPosition.z()));
        points[i].setY((p.y()*cameraPosition.z())/(p.z()-cameraPosition.z()));
        }
    }
    return points;
}

void MainWindow::prepareImage(QImage *img, std::vector<QVector3D> points,Sphere* sphere)
{
    std::vector<int> indices;


    int k1, k2;
    for(int i = 0; i < 37; ++i)
    {
        k1 = i * (18 + 1);     // beginning of current stack
        k2 = k1 + 18 + 1;      // beginning of next stack

        for(int j = 0; j < 18; ++j, ++k1, ++k2)
        {
            // 2 triangles per sector excluding first and last stacks
            // k1 => k2 => k1+1
            QVector3D t1(points[k1].x()-points[k2].x(),points[k1].y()-points[k2].y(),points[k1].z()-points[k2].z());
            QVector3D t2(points[k1].x()-points[k1+1].x(),points[k1].y()-points[k1+1].y(),points[k1].z()-points[k1+1].z());
            QVector3D t;
            t  = t.normal(t1,t2);
            if(i != 0)
            {
                if( t.z()>= 0)  //std::min(points[k1].z(),std::min(points[k2].z(),points[k1+1].z())) - sphere->getPosition().z()
                {


                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
                }

            }

            // k1+1 => k2 => k2+1
            if(i != (37-1))
            {
                if( t.z()>= 0)
                {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2+1);

                }

            }
        }
    }


    int size = indices.size();
    int width = img->width()/2;
    int height = img->height()/2;
    std::vector<QVector3D > p1;
    std::vector<QVector3D > p2;
    //std::vector<QVector3D >pointsProjected = projection(points);
    QImage* img_earth = sphere->getImage();
    for (int i = 0;i<size-2;i = i + 3)
    {

        p1.clear();
        p2.clear();
        p1.push_back(points[indices[i%size]]);
        p1.push_back(points[indices[(i+1)%size]]);
        p1.push_back(points[indices[(i+2)%size]]);
        p2.push_back(sphere->MappingPoint(indices[i%size]));
        p2.push_back(sphere->MappingPoint(indices[(i+1)%size]));
        p2.push_back(sphere->MappingPoint(indices[(i+2)%size]));
        //if(abs(p2[1].first-p2[2].first)>1000  || abs(p2[1].second-p2[2].second)>100)
            //continue;
        teksturing(p1,p2,sphere,img,img_earth);

        //straight_line(points[indices[i%size]],points[indices[(i+1)%size]],img);
        //straight_line(points[indices[(i+1)%size]],points[indices[(i+2)%size]],img);
        //straight_line(points[indices[(i+2)%size]],points[indices[(i+3)%size]],img);
    }
}





void MainWindow::straight_line(QVector3D point1,QVector3D point2,QImage *img)
{
    double a;
    int p,q;

    if(point1.x() != point2.x() )
    {
     a = (double)(point2.y() - point1.y())/(double)(point2.x() - point1.x());
    }
    else
    {

        p = (point1.y()<point2.y())? point2.y() : point1.y();//(a < b) ? b : a
        q = (point1.y()<point2.y())? point1.y() : point2.y();
        for(int i = q; i<=p;i++)
                paintP(point1.x(),i,img);
        return;
    }



    double b = point2.y() - a*point2.x();
    double j,z;

    if(a>=1 || a<=-1)
    {
        if(point1.y()<=point2.y())
            for(int i = point1.y(); i<=point2.y();i++)
            {
                z=i;
                j=(z-b)/a;
                paintP((int)j,i,img);
            }
        else if(point1.y()>point2.y())
            for(int i = point2.y(); i<=point1.y();i++)
            {
                z=i;
                j=(z-b)/a;
                paintP((int)j,i,img);
            }


    }
    else {
        if(point1.x()<point2.x())
        {
        for(int i = point1.x(); i<=point2.x();i++)
        {
            z=i;
            j=a*z+b;
            paintP(i,(int)j,img);
        }
        }
    else
        {
        for(int i = point2.x(); i<=point1.x();i++)
        {
            z=i;
            j=a*z+b;
            paintP(i,(int)j,img);
        }
        }
    }

}

void MainWindow::paintP(int a,int b,QImage* img)
{
    a = a + szer/2;
    b = -b + wys/2;

    if(a>0 && a<szer && b>0 && b<wys)
    {
    unsigned char *ptr;

    ptr = img->bits();

    ptr[szer*4*b + 4*a]=0; // Skladowa BLUE
    ptr[szer*4*b + 4*a + 1] = 0; // Skladowa GREEN
    ptr[szer*4*b + 4*a + 2] = 255; // Skladowa RED
    }

}

void MainWindow::paintC(int x, int y, QColor color, QImage *img)
{
    x = x +  szer/2;
    y = -y + wys/2;
    if(x>0 && x<szer && y>0 && y<wys)
    {
    unsigned char *ptr;

    ptr = img->bits();

    ptr[szer*4*y + 4*x]=color.blue(); // Skladowa BLUE
    ptr[szer*4*y + 4*x + 1] = color.green(); // Skladowa GREEN
    ptr[szer*4*y + 4*x + 2] = color.red(); // Skladowa RED
    }


}


void MainWindow::teksturing(std::vector<QVector3D > points,std::vector<QVector3D > points1,Sphere* sphere,QImage *img,QImage *img_orginal)
{
    std::vector<QVector3D > points2 = projection(points);
    QColor color;

    int minX = std::min({points2[0].x(),points2[1].x(),points2[2].x()});
    int maxX = std::max({points2[0].x(),points2[1].x(),points2[2].x()});

    int minY = std::min({points2[0].y(),points2[1].y(),points2[2].y()});
    int maxY = std::max({points2[0].y(),points2[1].y(),points2[2].y()});

    std::vector<QVector3D > normals;

    QVector3D N1(points[0].x()-sphere->getPosition().x(),points[0].y()-sphere->getPosition().y(),points[0].z()-sphere->getPosition().z());
    normals.push_back(N1.normalized());

    N1 = QVector3D(points[1].x()-sphere->getPosition().x(),points[1].y()-sphere->getPosition().y(),points[1].z()-sphere->getPosition().z());
    normals.push_back(N1.normalized());

    N1 = QVector3D(points[2].x()-sphere->getPosition().x(),points[2].y()-sphere->getPosition().y(),points[2].z()-sphere->getPosition().z());
    normals.push_back(N1.normalized());


    double mian = (double)((points2[1].x()-points2[0].x())*(points2[2].y()-points2[0].y())-(points2[1].y()-points2[0].y())*(points2[2].x()-points2[0].x()));
    if(mian< 0.0001)
        mian = 0.0001;

    for(int i =minY;i<=maxY;i++)
    {
        for (int j=minX;j<=maxX;j++)
        {

            double v = (double)((j-points2[0].x())*(points2[2].y()-points2[0].y())-(i-points2[0].y())*(points2[2].x()-points2[0].x()))/mian;
            double w = (double)((points2[1].x()-points2[0].x())*(i-points2[0].y())-(points2[1].y()-points2[0].y())*(j-points2[0].x()))/mian;
            double u = 1 - v - w;
            if(v>1 || v<0 || w>1 || w<0 || u>1 || u<0)
                continue;

            int x =(u*points1[0].x()+v*points1[1].x()+w*points1[2].x());
            int y = (u*points1[0].y()+v*points1[1].y()+w*points1[2].y());


            int x1 =(u*points[0].x()+v*points[1].x()+w*points[2].x());
            int y1 = (u*points[0].y()+v*points[1].y()+w*points[2].y());
            int z1 = (u*points[0].z()+v*points[1].z()+w*points[2].z());


            /*
            ptr[450*4*i + 4*j] = ptrA[450*4*y + 4*x]; // Skladowa BLUE
            ptr[450*4*i + 4*j + 1] = ptrA[450*4*y + 4*x+1]; // Skladowa GREEN
            ptr[450*4*i + 4*j + 2] = ptrA[450*4*y + 4*x+2]; // Skladowa RED
            */
            bool sunny = false;
            for(int i=0;i<light.size();i++)
                if(light[i]->getPosition().x() == sphere->getPosition().x() && light[i]->getPosition().y() == sphere->getPosition().y() && light[i]->getPosition().z() == sphere->getPosition().z())
                    sunny = true;
            if(sunny)
            {
                paintC(j,i,img_orginal->pixelColor(x,y),img);
            }
            else {
            QVector3D p(j,i,z1);
            QVector3D Normal = interpolationNormal(points2,p,normals);
            QVector3D SpherePoint(x1,y1,z1);
            color = shade(img_orginal->pixelColor(x,y),SpherePoint,Normal);
            paintC(j,i,color,img);
            //paintC(j,i,img_orginal->pixelColor(x,y),img);
            }
        }
    }
}



void MainWindow::on_Rotation_Y_sliderMoved(int position)
{
    if(!objects.empty())
    {
    int index = ui->spinBox_AO->value();
    objects[index]->setAplhaY((double)position/360*3.14*2);
    transformation();
    }
}

void MainWindow::on_Rotation_Y_sliderReleased()
{
    if(!objects.empty())
    {
    transformation();
    }
}

void MainWindow::on_Rotation_Z_sliderMoved(int position)
{
    if(!objects.empty())
    {
    int index = ui->spinBox_AO->value();
    objects[index]->setAplhaZ((double)position/360*3.14*2);
    transformation();
    }
}

void MainWindow::on_Rotation_Z_sliderReleased()
{
    if(!objects.empty())
    {
    transformation();
    }
}

void MainWindow::on_Rotation_X_sliderMoved(int position)
{
    if(!objects.empty())
    {
    int index = ui->spinBox_AO->value();
    objects[index]->setAplhaX((double)position/360*3.14*2);
    transformation();
    }
}
void MainWindow::on_Rotation_X_sliderReleased()
{
    if(!objects.empty())
    {
    int index = ui->spinBox_AO->value();
    objects[index]->setAplhaX((double)ui->Rotation_X->value()/360*3.14*2);
    transformation();
    }
}



void MainWindow::on_tranforamtionX_sliderMoved(int x)
{
    if(!objects.empty())
    {
    int index = ui->spinBox_AO->value();
    objects[index]->setPosition(x,objects[index]->getPosition().y(),objects[index]->getPosition().z());
    transformation();
    }
}

void MainWindow::on_transformationY_sliderMoved(int y)
{
    if(!objects.empty())
    {
    int index = ui->spinBox_AO->value();
    objects[index]->setPosition(objects[index]->getPosition().x(),y,objects[index]->getPosition().z());
    transformation();
    }
}

void MainWindow::on_transformationZ_sliderMoved(int z)
{
    if(!objects.empty())
    {
    int index = ui->spinBox_AO->value();
    objects[index]->setPosition(objects[index]->getPosition().x(),objects[index]->getPosition().y(),z);
    transformation();
    }
}



QColor MainWindow::shade(QColor color, QVector3D point, QVector3D N)
{



    //QVector3D V(point.x() - cameraPosition.x(),point.y() - cameraPosition.y(),point.z() - cameraPosition.z());
    QVector3D V( cameraPosition.x()-point.x(), cameraPosition.y()-point.y() ,cameraPosition.z()-point.z());
    V.normalize();
    if(light.empty())
    {
        QColor col;
        col.setRgb(0,0,0);
        return col;
    }
double IP =0;
    for(int i=0;i<light.size();i++)
    {
        QVector3D L1 = light[i]->getPosition();
        //QVector3D L(point.x()-L1.x(),point.y()-L1.y(),point.z()-L1.z());
        QVector3D L(L1.x()-point.x(),L1.y()-point.y(),L1.z()-point.z());

        L.normalize();
        QVector3D R = 2*(L*N)*N-L;
        R.normalize();

        IP = IP + 1.2 * std::max(0.f, L.dotProduct(L,N)) * light[i]->getIntensity() + 0.01*pow(std::max(0.f,R.dotProduct(R,V)),200)*light[i]->getIntensity();

        /*
        color.setRed(std::max(color.red() + IP,0.));
        color.setGreen(std::max(color.green() + IP,0.));
        color.setBlue(std::max(color.blue() + IP,0.));
        */
        IP = std::max(0.,IP);
        }


        color.setRed(std::max(std::min(color.red()*IP,255.),0.));
        color.setGreen(std::max(std::min( color.green()*IP,255.),0.));
        color.setBlue(std::max(std::min( color.blue()*IP,255.),0.));


    return color;
}

QVector3D MainWindow::interpolationNormal(std::vector<QVector3D> triangle,QVector3D point,std::vector<QVector3D> normals)
{
    double sot = triangle[1].y() - triangle[0].y();
    if(abs(sot) < 0)
        sot=1;

    QVector3D N1 = normals[0]*(triangle[1].y() - point.y())/(sot)  +  normals[1]*(point.y() - triangle[0].y())/(sot);

    sot = triangle[2].y() - triangle[0].y();
        if(abs(sot) < 0)
            sot=1;
    QVector3D N2 = normals[0]*(triangle[2].y() - point.y())/(sot)  +  normals[2]*(point.y() - triangle[0].y())/(sot);

    double mian = triangle[0].y()-triangle[1].y();
    if(abs(mian) > 0)
        mian = 1/mian;
    else
        mian = 1;
    double xD = (triangle[0].x()-triangle[1].x())*mian*point.y() + (triangle[0].x()-((triangle[0].x()-triangle[1].x())*mian*triangle[0].y()));

    mian = triangle[0].y()-triangle[2].y();
        if(abs(mian) > 0)
            mian = 1/mian;
        else
             mian = 1;

    double xF = ((triangle[0].x()-triangle[2].x())*mian*point.y()) + (triangle[0].x()-((triangle[0].x()-triangle[2].x())*mian*triangle[0].y()));

    QVector3D N;

    if(abs(xF-xD) > 0)
      N = N1*(xF-point.x())/(xF-xD) + N2*(point.x()-xD)/(xF-xD);
    else
       N = N1*(xF-point.x()) + N2*(point.x()-xD);


    N.normalize();

    return N;

}

void MainWindow::on_planetButton_clicked()
{
    int x = ui->spinBox_X->value();
    int y = ui->spinBox_Y->value();
    int z = ui->spinBox_Z->value();
    int r = ui->spinBox_R->value();


    Sphere*  sphere = new Sphere(x,y,z,r);
    sphere->setImage(":/earth_daymap.jpg");
    if(!objects.empty())
        ui->spinBox_AO->setMaximum(ui->spinBox_AO->maximum()+1);
    objects.push_back(sphere);
    transformation();
}

void MainWindow::on_starButton_clicked()
{
    int x = ui->spinBox_X->value();
    int y = ui->spinBox_Y->value();
    int z = ui->spinBox_Z->value();
    int r = ui->spinBox_R->value();
    double lightIntensity = ui->lightIntensity->value();

    Star* sun = new Star(x,y,z,r,lightIntensity);
    sun->setImage(":/2k_sun.jpg");
    if(!objects.empty())
        ui->spinBox_AO->setMaximum(ui->spinBox_AO->maximum()+1);
    light.push_back(sun);
    objects.push_back(sun);
    transformation();
}

bool MainWindow::compareSphere(Sphere* i1, Sphere* i2)
{
    return (i1->getPosition().z() > i2->getPosition().z());
}
