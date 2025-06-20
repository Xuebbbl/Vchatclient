#include "dialog.h"
#include "ui_dialog.h"

#include<QDebug>
#include<QMessageBox>
#include<cellmain.h>
#include<QString>
#include<QByteArray>
#include<string>
#include<QGraphicsDropShadowEffect>
#include<QPixmap>
#include<QSize>
#include"signup.h"

//connetion连接最好连接在构造函数中，不要在次函数中
int flag=0;//标记服务器连接状态
bool Dialog::isconnect=false;
Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);

    QPixmap *pix=new QPixmap(":/blue.png");
    QSize sz=ui->label_image->size();
    ui->label_image->setPixmap(pix->scaled(sz));
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setOffset(-3, 0);
    shadow->setColor(QColor("#888888"));
    shadow->setBlurRadius(30);
    ui->label_image->setGraphicsEffect(shadow);


    m_t=my_TcpSocket::instance();
    m_t->connectToServer();
    setWindowTitle("服务器连接中....");
    connect(m_t->socket,&QTcpSocket::connected,this,[=](){
        flag=1;
         setWindowTitle("登陆页面：服务器连接成功");
        isconnect=true;
    });
    connect(m_t->socket,&QTcpSocket::disconnected,this,[=](){
        flag=0;
          setWindowTitle("登陆页面：服务器连接失败");
        isconnect=false;
    });

    connect(m_t->socket, &QTcpSocket::readyRead, this, &Dialog::onReadyRead);



    //设置登录窗口的密码加密
    ui->le_userPassword->setEchoMode(QLineEdit::Password);
}



Dialog::~Dialog()
{

    delete ui;
}

void Dialog::on_btn_login_clicked()
{

    if(flag==0){
        QMessageBox::warning(this,"提示","等待服务器启动中...");
    }else
    {

         m_t->writeToServer(ui->le_userName->text()+":");
         m_t->writeToServer(ui->le_userPassword->text()+'|');

    }

}
void Dialog::onReadyRead() {
    qDebug() << "数据到达";
    QByteArray ba = m_t->recvFromServer();
    QString str(ba);
    qDebug() << "str: " << str;
    if (str == "yes") {  // 服务端登录成功响应处理
        CellMain c;
        c.getStrUser(ui->le_userName->text());

        CellMain::username=ui->le_userName->text();
        chatdb::instance().insertheader(ui->le_userName->text());



        setResult(1);
        hide();
        // 当验证成功以后断开此连接，避免跳入主界面时该信号槽还会触发
        disconnect(m_t->socket, &QTcpSocket::readyRead, this, &Dialog::onReadyRead);
        //因为你已经登录成功，接下来的通信应该交由主界面（如 CellMain）来处理。
        //如果不断开，Dialog::onReadyRead() 会继续响应 socket 数据，会重复触发旧的逻辑。
    } else {
        QMessageBox::warning(this, "提示", "账号或密码错误！");
        ui->le_userName->clear();
        ui->le_userPassword->clear();
    }
}

void Dialog::closeEvent(QCloseEvent *event)
{
    exit(0);
}

//退出
void Dialog::on_btn_exit_clicked()
{
    exit(0);
}
//注册功能
void Dialog::on_pushButton_clicked()
{

    setResult(0);
    //进入登陆页面前只能隐藏此登陆页面，不能调用this.close()进行关闭，不然会触发closeEvent事件，我的closeEvent事件是退出进程
    //会导致整个进程都退出！

    hide();

}
