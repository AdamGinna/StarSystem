#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "sphere.h"
#include "star.h"
#include <QMainWindow>
#include <QPainter>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    int szer;
    int wys;
    int poczX;
    int poczY;
    QImage *img;
    QVector3D cameraPosition;
    Sphere* sphere;
    Star* sun;
    std::vector<Star*> light;

    void transformation();
    std::vector<QVector3D> projection(std::vector<QVector3D>);
    void prepareImage(QImage*,std::vector<QVector3D>,Sphere*);
    void straight_line(QVector3D,QVector3D,QImage*);
    void paintP(int,int,QImage*);
    void paintC(int,int,QColor,QImage*);
    void teksturing(std::vector<QVector3D >,std::vector<QVector3D >,Sphere*,QImage *img,QImage *img_orginal);
    QVector3D barycentralPoint(std::vector<QVector3D> p1,std::vector<QVector3D> p2,QVector3D point);
    QColor shade(QColor color, QVector3D point, QVector3D N);
    QVector3D interpolationNormal(std::vector<QVector3D>,QVector3D,std::vector<QVector3D>);


private slots:
   void paintEvent(QPaintEvent*);
   void on_tranforamtionX_sliderMoved(int position);
   void on_transformationY_sliderMoved(int position);
   void on_Rotation_X_sliderMoved(int position);
   void on_ScalingHigh_sliderMoved(int position);

   void on_shearingX_sliderMoved(int position);
   void on_shearingY_sliderMoved(int position);
   void on_shearingY_sliderReleased();
   void on_shearingX_sliderReleased();
   void on_ScalingHigh_sliderReleased();
   void on_ScalingWidth_sliderReleased();
   void on_Rotation_X_sliderReleased();
   void on_transformationZ_sliderMoved(int position);
   void on_Rotation_Y_sliderMoved(int position);
   void on_Rotation_Y_sliderReleased();
   void on_Rotation_Z_sliderMoved(int position);
   void on_Rotation_Z_sliderReleased();
};

#endif // MAINWINDOW_H
