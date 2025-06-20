#ifndef DIALOG_H
#define DIALOG_H

// 引入 Qt 所需模块
#include <QDialog>
#include <QString>
#include <QHostAddress>
#include <QMessageBox>
#include <QByteArray>
#include <QTcpSocket>
#include "my_tcpsocket.h"
#include <QCloseEvent>
#include "chatdb.h"

// 前向声明 UI 命名空间
namespace Ui {
class Dialog;
}

// 登录对话框类，继承自 QDialog
class Dialog : public QDialog
{
    Q_OBJECT

public:
    // 构造函数，创建登录窗口
    explicit Dialog(QWidget *parent = nullptr);

    // 析构函数，释放资源
    ~Dialog();

    // 自定义槽函数：用于接收服务端发送的数据
    void onReadyRead();

    // 重写关闭事件：点击窗口右上角X按钮时强制退出程序
    void closeEvent(QCloseEvent *event);

    // 静态变量：标记是否与服务器建立连接
    static bool isconnect;

private slots:
    // 登录按钮点击事件处理
    void on_btn_login_clicked();

    // 退出按钮点击事件处理
    void on_btn_exit_clicked();

    // 注册按钮点击事件处理（跳转到注册界面）
    void on_pushButton_clicked();

private:
    Ui::Dialog *ui;            // Qt Designer 自动生成的 UI 指针
    my_TcpSocket *m_t;         // 自定义的 TCP 套接字单例，用于与服务器通信
    QByteArray ba;             // 接收从服务器返回的数据缓冲区
};

#endif // DIALOG_H
