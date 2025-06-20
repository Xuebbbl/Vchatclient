#ifndef CELLMAIN_H
#define CELLMAIN_H

#include <QWidget>
#include <QString>
#include "friendsitem.h"         // 好友列表项的自定义控件
#include "chatwindows.h"         // 聊天窗口
#include "my_tcpsocket.h"        // TCP通信类（单例）
#include <QList>
#include <QStringList>
#include <QMap>
#include <QCloseEvent>
#include "addfriend.h"           // 添加好友窗口
#include <QMouseEvent>
#include <QAction>
#include <QMenu>
#include "chatdb.h"              // 数据库操作封装类
#include <QSqlQuery>
#include <QTimer>                // 用于定时检测好友在线状态
#include "circleoffriends.h"     // 朋友圈功能
#include <memory>

namespace Ui {
class CellMain;
}

// 聊天主窗口类，显示好友列表和聊天窗口
class CellMain : public QWidget
{
    Q_OBJECT

public:
    explicit CellMain(QWidget *parent = nullptr); // 构造函数
    ~CellMain();                                   // 析构函数

    void getStrUser(QString str);                 // 获取当前登录的用户名
    void addfriendnow(QString strid);             // 登录后添加好友项（实时更新列表）

    // 重写鼠标右键点击事件，用于显示群聊创建菜单
    void mousePressEvent(QMouseEvent *event) override;

    void createGroup();                           // 创建临时群聊的逻辑
    static QString username;                      // 静态成员，记录当前用户名

    void findheader();                            // 从数据库中查找并设置头像


public slots:
    void handleReadyRead();                       // 处理服务器发来的好友/消息数据
    void handleFriendItemClicked();               // 好友项点击，切换聊天窗口
    void handleMessage_to_text();                 // 将收到的消息显示在聊天窗口中
    void closeEvent(QCloseEvent *event);          // 程序关闭事件，更新在线状态

signals:
    void handleMessage();                         // 收到新消息信号
    void updateIsRuning();                        // 用于刷新好友在线状态信号

private slots:
    void on_addFriend_triggered(QAction *arg1);   // 预留的添加好友动作（可忽略）
    void on_addFriend_clicked();                  // 点击“添加好友”按钮事件
    void on_deleteFriend_clicked();               // 点击“删除好友”按钮事件
    void on_applyFriend_clicked();                // 点击“修改密码”按钮事件
    void on_friendsitem_clicked();                // 点击“朋友圈”按钮事件

private:
    Ui::CellMain *ui;                             // Qt Designer 生成的 UI 指针
    QString strUser;                              // 当前用户 ID（内部用）
    my_TcpSocket *c_mysocket;                     // TCP 套接字对象（用于通信）

    int index = 0;                                // 当前聊天窗口索引（可能用于控制聊天页切换）
    QList<chatWindows *> chatWindowsList;         // 聊天窗口对象列表（与好友一一对应）
    QList<FriendsItem *> friendsItemList;         // 好友项对象列表
    QMap<QString, QString> userAndMessage;        // 用于缓存未显示的消息（按用户）

    addfriend a;                                  // 添加好友对话框对象
    int index_ai;                                 // AI聊天对应的聊天窗口索引
    QByteArray image_data;                        // 当前用户头像二进制数据

    QTimer *timer = nullptr;                      // 用于定时查询好友在线状态
    std::unique_ptr<circleOfFriends> cir;         // 朋友圈界面智能指针
};

#endif // CELLMAIN_H
