#ifndef PUSHWORKS_H
#define PUSHWORKS_H

#include <QWidget>
#include <QWidget>
#include <QEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QImage>
#include <QDebug>
#include <QFileDialog>
#include"emoji.h"
class ImageEventFilter;

namespace Ui {
class pushWorks;
}

class pushWorks : public QWidget
{
    Q_OBJECT

public:
    explicit pushWorks(QWidget *parent = nullptr);
    ~pushWorks();

private slots:
    void on_toolButton_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

signals:
    void updatefriendCil();

private:
    Ui::pushWorks *ui;
    ImageEventFilter *image=nullptr;
    emoji *e;
};

//事件过滤器

class ImageEventFilter : public QWidget
{
    Q_OBJECT

public:

    ImageEventFilter()  {}


protected:
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        QWidget *widget = qobject_cast<QWidget *>(watched);
        if (!widget) return false;

        // 处理重绘事件
        if (event->type() == QEvent::Paint)

        {



            // if(widget->objectName()=="widget_5"||widget->objectName()=="widget_6"){
            //     if(widget->objectName()=="widget_5"){
            //         widget->setObjectName("widget_7");
            //     }else if(widget->objectName()=="widget_6"){
            //         widget->setObjectName("widget_8");
            //     }
            //     qDebug()<<"11111111111111111111111";
            QPainter painter(widget);
            if(image_1.isNull()==true&&image_2.isNull()==true){
            painter.setRenderHint(QPainter::Antialiasing);  // 启用抗锯齿，提高绘制质量

            // 设置颜色为灰黑色
            QPen pen(QColor(169, 169, 169));  // 灰黑色
            pen.setWidth(4);  // 设置线条宽度
            painter.setPen(pen);

            // 获取当前QWidget的大小
            int width = widget->width();
            int height = widget->height();

            // 计算`+`号的中心和线条的长度
            int centerX = width / 2;
            int centerY = height / 2;
            int lineLength = qMin(width, height) / 2;  // `+`号的线条长度

            // 绘制水平和垂直的线，组成`+`号
            painter.drawLine(centerX - lineLength / 2, centerY, centerX + lineLength / 2, centerY);  // 水平线
            painter.drawLine(centerX, centerY - lineLength / 2, centerX, centerY + lineLength / 2);  // 垂直线

            return true;  // 表示事件已经被处理，不再传递给默认事件处理
            // }else{
            // qDebug()<<"11111111111111111111112";
                // 创建 QPainter 对象
                // QPainter painter(widget);
            }else{

            if(image_1.isNull()!=true&&widget->objectName()=="widget_5"){
                // 获取当前窗口的大小
                QSize widgetSize = widget->size();

                // 根据 QWidget 的大小缩放图像
                QImage scaledImage = image_1.scaled(widgetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                // 绘制缩放后的图像
                painter.drawImage(0, 0, scaledImage);

            // }

            }else if(image_2.isNull()!=true&&widget->objectName()=="widget_6"){

                // 获取当前窗口的大小
                QSize widgetSize = widget->size();

                // 根据 QWidget 的大小缩放图像
                QImage scaledImage = image_2.scaled(widgetSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

                // 绘制缩放后的图像
                painter.drawImage(0, 0, scaledImage);
            }


            }

        }

        // 处理鼠标点击事件
        if (event->type() == QEvent::MouseButtonPress)
        {

            QString filePath = QFileDialog::getOpenFileName(nullptr, "选择图片", "",
                                                            "图片文件 (*.png *.jpg *.jpeg *.bmp *.gif)");

            if (filePath.isEmpty()) {
                return false;  // 如果没有选择文件，则返回一个空的QImage对象
            }

            // 将选择的图片文件路径转换为QImage对象
            QImage image(filePath);

            // 检查图像是否有效
            if (image.isNull()) {
                qWarning() << "无法加载图片: " << filePath;
                return false;
            }
            if(widget->objectName()=="widget_5"){
            image_1=image;
                qDebug()<<"赋值给图片1";
            }else if(widget->objectName()=="widget_6"){
                image_2=image;
                qDebug()<<"赋值给图片2";
            }


            //问题一，updata更新时，更新的是两个widget，只让他更新一个widget
            //问题二，初始化重绘制事件加上判断条件就不绘制了





            return true;  // 事件已经处理
        }

        return false;  // 让默认事件处理程序处理其他事件
    }

public:
    QImage image_1;
    QImage image_2;

};



#endif // PUSHWORKS_H
