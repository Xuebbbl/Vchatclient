#include "pushworks.h"
#include "ui_pushworks.h"
#include"cellmain.h"
#include<QSqlError>
#include<QVariant>
#include<QBuffer>
pushWorks::pushWorks(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::pushWorks)
{
    ui->setupUi(this);
    setWindowTitle("编辑作品");
    image=new ImageEventFilter;

    ui->widget_5->installEventFilter(image);
    ui->widget_6->installEventFilter(image);

    e=new emoji;
    connect(e,&emoji::emitString,this,[=](QString emoji){
        ui->textEdit->append(emoji);
    });


    //更新到数据库

}

pushWorks::~pushWorks()
{
    delete e;
    delete ui;
}
//表情包
void pushWorks::on_toolButton_clicked()
{
    e->show();
}

//发布作品
void pushWorks::on_pushButton_clicked()
{
    //先更新到数据库
    QSqlQuery query;
    query.prepare(R"(
    INSERT INTO friendsWorks
        (username, Copywriting, image_1, image_2, like, content)
    VALUES
        (:username, :copywriting, :img1, :img2, :like, :content)
      )");
    qDebug()<<CellMain::username;
    query.bindValue(":username", CellMain::username);
    query.bindValue(":copywriting", ui->textEdit->toPlainText());
     query.bindValue(":like", 0);
     query.bindValue(":content","");
    // 处理图像1
    QByteArray img1Data;
    if (!image->image_1.isNull()) {
        QBuffer buffer(&img1Data);
        if (buffer.open(QIODevice::WriteOnly) && image->image_1.save(&buffer, "PNG")) {
            query.bindValue(":img1", img1Data);
        } else {
            qWarning() << "图像1保存失败，使用NULL";
            query.bindValue(":img1", QVariant());
        }
        buffer.close();
    } else {
        query.bindValue(":img1", QVariant());
    }
    QByteArray img1Data1;
    if (!image->image_2.isNull()) {
        QBuffer buffer(&img1Data1);
        if (buffer.open(QIODevice::WriteOnly) && image->image_2.save(&buffer, "PNG")) {
            query.bindValue(":img2", img1Data1);
        } else {
            qWarning() << "图像1保存失败，使用NULL";
            query.bindValue(":img2", QVariant());
        }
        buffer.close();
    } else {
        query.bindValue(":img2", QVariant());
    }


    if (!query.exec()) {
        qCritical() << "Error:" << query.lastError().text();
    }

    //发送信号表示要更新朋友圈
    emit updatefriendCil();

    close();



}

//取消发布
void pushWorks::on_pushButton_2_clicked()
{
    close();
}

