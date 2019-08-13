 #include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    sphere = new Sphere(0,0,2000,200);
    sphere->setImage(":/earth_daymap.jpg");
    sun = new Star(700,0,2500,250,2.5);
    sun->setImage(":/2k_sun.jpg");
    light.push_back(sun);
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
    double scale = ui->ScalingHigh->value()/5.;
    double alphax = (double)ui->Rotation_X->value()/360*3.14*2;
    double alphay = (double)ui->Rotation_Y->value()/360*3.14*2;
    double alphaz = (double)ui->Rotation_Z->value()/360*3.14*2;
    double shX = ui->shearingX->value()/40.;
    double shY = ui->shearingY->value()/40.;
    prepareImage( img,sphere->getPoints(scale,alphax,alphay,alphaz,shX,shY),sphere);
    //prepareImage( img,sun->getPoints(scale,alphax,alphay,alphaz,shX,shY),sun);
    update();

}

std::vector<QVector3D> MainWindow::projection(std::vector<QVector3D> points)
{
    QVector3D p;
    for (int i = 0;i<points.size();i++)
    {
        p = points[i];
        if(p.z()-cameraPosition.z()>0.01)
        {
        points[i].setX((p.x()*cameraPosition.z())/(p.z()+cameraPosition.z()));
        points[i].setY((p.y()*cameraPosition.z())/(p.z()+cameraPosition.z()));
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



void MainWindow::on_Rotation_X_sliderMoved(int position)
{

    transformation();
}
void MainWindow::on_Rotation_X_sliderReleased()
{
    transformation();
}


void MainWindow::on_ScalingHigh_sliderMoved(int position)
{
    transformation();
}


void MainWindow::on_shearingX_sliderMoved(int position)
{
    transformation();
}

void MainWindow::on_shearingY_sliderMoved(int position)
{
    transformation();
}

void MainWindow::on_shearingY_sliderReleased()
{
    transformation();
}

void MainWindow::on_shearingX_sliderReleased()
{
    transformation();
}

void MainWindow::on_ScalingHigh_sliderReleased()
{
    transformation();
}

void MainWindow::on_ScalingWidth_sliderReleased()
{
    transformation();
}


void MainWindow::on_tranforamtionX_sliderMoved(int x)
{
    sphere->setPosition(x,sphere->getPosition().y(),sphere->getPosition().z());
    sun->setPosition(x,sun->getPosition().y(),sun->getPosition().z());
    transformation();
}

void MainWindow::on_transformationY_sliderMoved(int y)
{
    sphere->setPosition(sphere->getPosition().x(),y,sphere->getPosition().z());
    sun->setPosition(sun->getPosition().x(),y,sun->getPosition().z());

    transformation();
}

void MainWindow::on_transformationZ_sliderMoved(int z)
{
    sphere->setPosition(sphere->getPosition().x(),sphere->getPosition().y(),z);
     sun->setPosition(sun->getPosition().x(),sun->getPosition().y(),z);
    transformation();
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
    x = x + szer/2;
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
    /*
    QVector3D N1(points[0].x()-points[1].x(),points[0].y()-points[1].y(),points[0].z()-points[1].z());
    QVector3D N2(points[0].x()-points[2].x(),points[0].y()-points[2].y(),points[0].z()-points[2].z());
    normals.push_back(N1.normal(N1,N2));
    N1 = QVector3D(points[1].x()-points[0].x(),points[1].y()-points[0].y(),points[1].z()-points[0].z());
    N2 = QVector3D(points[1].x()-points[2].x(),points[1].y()-points[2].y(),points[1].z()-points[2].z());
    normals.push_back(N1.normal(N1,N2));
    N1 = QVector3D(points[2].x()-points[0].x(),points[2].y()-points[0].y(),points[2].z()-points[0].z());
    N2 = QVector3D(points[2].x()-points[1].x(),points[2].y()-points[1].y(),points[2].z()-points[1].z());
    normals.push_back(N1.normal(N1,N2));
    */

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
            QVector3D SpherePoint(j,i,z1);
            color = shade(img_orginal->pixelColor(x,y),SpherePoint,points2,normals);
            paintC(j,i,color,img);
            //paintC(j,i,img_orginal->pixelColor(x,y),img);
        }
    }
}



void MainWindow::on_Rotation_Y_sliderMoved(int position)
{
    transformation();
}

void MainWindow::on_Rotation_Y_sliderReleased()
{
    transformation();
}

void MainWindow::on_Rotation_Z_sliderMoved(int position)
{
    transformation();
}

void MainWindow::on_Rotation_Z_sliderReleased()
{
    transformation();
}


QVector3D MainWindow::barycentralPoint(std::vector<QVector3D> p1,std::vector<QVector3D> p2,QVector3D point)
{
    double u,v,w;
    //u = ;
    //v = ;
    //w = ;
    return QVector3D(u,v,w);
}

QColor MainWindow::shade(QColor color, QVector3D point, std::vector<QVector3D> triangle, std::vector<QVector3D> normals)
{

    double sot = triangle[1].y() - triangle[0].y();
    if(abs(sot) < 0.1)
        sot=1;

    QVector3D N1 = normals[0]*(triangle[1].y() - point.y())/(sot)  +  normals[1]*(point.y() - triangle[0].y())/(sot);

    sot = triangle[2].y() - triangle[0].y();
        if(abs(sot) < 0.1)
            sot=1;
    QVector3D N2 = normals[0]*(triangle[2].y() - point.y())/(sot)  +  normals[2]*(point.y() - triangle[0].y())/(sot);

    double mian = triangle[0].y()-triangle[1].y();
    if(abs(mian) > 0.1)
        mian = 1/mian;
    else
        mian = 1;
    double xD = (triangle[0].x()-triangle[1].x())*mian*point.y() + (triangle[0].x()-((triangle[0].x()-triangle[1].x())*mian*triangle[0].y()));

    mian = triangle[0].y()-triangle[2].y();
        if(abs(mian) > 0.1)
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

    QVector3D V(point.x() - cameraPosition.x(),point.y() - cameraPosition.y(),point.z() - cameraPosition.z());
    V.normalize();

    for(int i=0;i<light.size();i++)
    {
        QVector3D L1 = light[i]->getPosition();
        QVector3D L(point.x()-L1.x(),point.y()-L1.y(),point.z()-L1.z());
        L.normalize();
        QVector3D R = 2*(L*N)*N-L;
        R.normalize();

        double IP = 0.7*L.dotProduct(L,N)*light[i]->getIntensity() + 0.01*pow(R.dotProduct(R,V),200)*light[i]->getIntensity();

        /*
        color.setRed(std::max(color.red() + IP,0.));
        color.setGreen(std::max(color.green() + IP,0.));
        color.setBlue(std::max(color.blue() + IP,0.));
        */

        IP = std::max(0.,IP);

        color.setRed(std::max(std::min(color.red()*IP,255.),0.));
        color.setGreen(std::max(std::min( color.green()*IP,255.),0.));
        color.setBlue(std::max(std::min( color.blue()*IP,255.),0.));
        return color;
    }



}


