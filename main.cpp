
#include<cellmain.h>
#include <QApplication>
#include<dialog.h>
#include"signup.h"
#include<QIcon>
#include"circleoffriends.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // //设置程序图标
    a.setWindowIcon(QIcon(":/weixin.png"));
   //进入登陆页面！
    Dialog l;
    int ret=l.exec();



    while(1){

    if(ret==1){
   //主界面聊天窗口实现
        CellMain l;
        l.show();
        return a.exec();
    }
    else{
        Signup s;
        //执行注册页面
        s.show();
        a.exec();
        //注册页面结束以后，又回到登陆页面等待操作
        ret=l.exec();
     }
 }

    return a.exec();
}


