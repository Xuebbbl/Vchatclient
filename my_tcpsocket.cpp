#include "my_tcpsocket.h"

//设置为全局静态,单例模式，使在其他类使用时，也只使用该一个套接字
//单例模式：只创建一个 my_TcpSocket 对象（共享连接）
my_TcpSocket* my_TcpSocket::m_instance=nullptr;
QTcpSocket* my_TcpSocket::socket=nullptr;


my_TcpSocket::my_TcpSocket(QObject *parent)
    : QObject{parent}
{

}
// 将 QString 转为字节流发送给服务器
void my_TcpSocket::connectToServer()
{
    socket=new QTcpSocket;
    unsigned short port=10000;
    socket->connectToHost(QHostAddress("192.168.141.128"),port);
}

//读取服务端返回的所有数据
void my_TcpSocket::writeToServer(QString str)
{
    socket->write(str.toUtf8());
}

QByteArray my_TcpSocket::recvFromServer()
{

    return socket->readAll();

}

void my_TcpSocket::closeToSocket()
{

    //释放资源，还需测试
   //  delete socket;
    // socket->close();
}

