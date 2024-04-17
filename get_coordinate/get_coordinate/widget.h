#ifndef WIDGET_H
#define WIDGET_H
#include <yaml-cpp/yaml.h>
#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QMouseEvent>
#include <iostream>
#include <QCoreApplication>
#include <QImage>
#include <QDebug>
#include <QFile>
#include <QStringList>
#include <QPainter>
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void loadImage(const QString &filePath);
    void mousePressEvent(QMouseEvent *event);
    double target_x;
    double target_y;
    double resolution;
    double origin_x;
    double origin_y;
    double img_w;
    double img_h;
    double bias_x;
    double bias_y;
    int k;
private:
    Ui::Widget *ui;
    QLabel *imageLabel;
    float m_fScale;
};
#endif // WIDGET_H
