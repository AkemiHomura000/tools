#include "widget.h"
#include "./ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    YAML::Node config = YAML::LoadFile("/home/lp1/qt_test/nav.yaml");
    // 提取resolution的值
    resolution = config["resolution"].as<double>();
    std::cout << "Resolution: " << resolution << std::endl;
    // 提取origin的值
    YAML::Node origin_node = config["origin"];
    origin_x=origin_node[0].as<double>();
    origin_y=origin_node[1].as<double>();
    std::cout<<"x,y:"<<origin_x<<","<<origin_y<<std::endl;
    QVBoxLayout *layout = new QVBoxLayout(this);
    QLabel *imageLabel = new QLabel;
    layout->addWidget(imageLabel);
    QPixmap pixmap("/home/lp1/qt_test/nav.pgm");
    img_w=pixmap.size().rwidth();
    img_h=pixmap.size().rheight();
    std::cout<<"w,h:"<<img_w<<","<<img_h<<std::endl;
    k=4;
    bias_x=10+k*abs(origin_x)/resolution;
    bias_y=10+k*(img_h-abs(origin_y)/resolution);
    pixmap = pixmap.scaled(pixmap.width() * k, pixmap.height() * k, Qt::KeepAspectRatio);
    imageLabel->setPixmap(pixmap);
    resize(pixmap.size());
}
Widget::~Widget()
{
    delete ui;
}
void Widget::mousePressEvent(QMouseEvent *event)
{
    // 获取鼠标点击的坐标
    QPoint clickPos = event->pos();
    int x = clickPos.x();
    int y = clickPos.y();
    // 将 x 和 y 值转换为 double 类型
    double x_double = (static_cast<double>(x)-bias_x)*0.05/k;
    double y_double = (-static_cast<double>(y)+bias_y)*0.05/k;
    // 输出鼠标点击的坐标
    target_x=x_double;
    target_y=y_double;
    std::cout<< "点击坐标：" <<x_double<<"   "<<y_double<<std::endl;
}

