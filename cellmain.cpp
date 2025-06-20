#include "cellmain.h"
#include "ui_cellmain.h"
#include<QToolBar>
#include<QListWidgetItem>
#include<QSize>
#include<QMessageBox>
#include<QTcpSocket>
#include<QByteArray>
#include"addfriend.h"
#include"deletefriend.h"
#include"checkpassword.h"
#include<QInputDialog>
#include<QFile>
#include<QFileDialog>
#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonValue>

#include<QPushButton>
#include<QCheckBox>
#include<QLabel>
#include<QFileDialog>
#include<QStringList>
#include<QSqlError>
//用listwidget实现好友列表设计的逻辑层级设定
//QListWidget->QListWidgetItem->自定义控件widget(我的是FriendsItem)
#include<QStackedWidget>
#include<QRegularExpression>
#include<QRegularExpressionMatch>
int i=0;

QString CellMain::username;

CellMain::CellMain(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CellMain)
{
    ui->setupUi(this);  // 初始化UI

    c_mysocket = my_TcpSocket::instance();  // 获取通信实例

    // 监听服务器断开
    connect(my_TcpSocket::socket, &QTcpSocket::disconnected, this, [=]() {
        QMessageBox::warning(this, "警告", "服务器断开连接，请启动服务器后重试");
        exit(0);
    });

    // 请求好友数据
    c_mysocket->writeToServer("获得好友聊天消息列表！");

    // 绑定服务器消息处理
    //客户端 socket 检测到“有数据可读”时自动发出信号
    connect(c_mysocket->socket, &QTcpSocket::readyRead, this, &CellMain::handleReadyRead);
    //手动发出“数据准备好了，可以展示”的信号
    connect(this, &CellMain::handleMessage, this, &CellMain::handleMessage_to_text);

    // 添加好友成功信号响应
    connect(&a, &addfriend::transformStr, this, &CellMain::addfriendnow);

    // 设置用户名
    ui->account->setText(CellMain::username);

    // 设置头像圆形样式
    ui->headerl->setFixedSize(60, 60);
    ui->headerl->setStyleSheet("border-radius:30px; border:1px solid #ccc;");
    ui->headerl->setScaledContents(true);

    // 去掉好友列表横向滚动条
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 设置列表选中样式（可选：已在 .ui 中定义，但也可以再此强化）
    ui->listWidget->setStyleSheet(R"(
        QListWidget::item:selected {
            background-color: #e6f7ff;
            border-radius: 5px;
        }
    )");

    // 初始化头像
    findheader();

    // 初始化定时器：定时检查好友在线状态
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &CellMain::updateIsRuning);
    timer->start(2000);

    // 初始化朋友圈页面
    cir = std::make_unique<circleOfFriends>();
}

CellMain::~CellMain()
{
    delete ui;
}

void CellMain::getStrUser(QString str)
{
    this->strUser=str;
    qDebug()<<strUser;
}

//用于添加好友后实时的把好友的给添加到列表中
void CellMain::addfriendnow(QString strid)
{
    //聊天框
    chatWindows *cw=new chatWindows(this);
    ui->stackedWidget->addWidget(cw);
    chatWindowsList.append(cw);

    //创建好友列表！！！！！！！
    //将自定义控件添加到list widget中
    FriendsItem *fi=new FriendsItem(this);
    fi->setId("ID:"+strid);
    //给聊天主界面设置身份
    cw->strId=strid;

    connect(fi, &FriendsItem::clicked, this, &CellMain::handleFriendItemClicked);

    friendsItemList.append(fi);

    fi->setHeadImage("border-radius:30; border-image:url(:/friend.png);");
    //创建好友容器
    QListWidgetItem * m_Item=new QListWidgetItem(ui->listWidget);
    //设置宽高,m_Item的宽高
    m_Item->setSizeHint(QSize(281,90));
    //将自定义好友项添加到widgetItem中，再将widgetItem添加到widgetlist中
    ui->listWidget->setItemWidget(m_Item,fi);
    //改变点击后的样式
    ui->listWidget->setStyleSheet("QListWidget::item:selected{background-color:rgb(200,200,200);};");
}

void CellMain::mousePressEvent(QMouseEvent *event) {


    QMenu *menu=new QMenu(this);
    QAction *a=menu->addAction("创建临时群聊");
    connect(a,&QAction::triggered,this,&CellMain::createGroup);
    //显示菜单
    if(event->button()==Qt::RightButton){
        menu->exec(event->globalPos());
    }else if(event->button()==Qt::LeftButton){
        if(ui->headerl->geometry().contains(event->pos()))//(event->pos()返回当前鼠标点击相对于当前控件（如 CellMain）的位置坐标
        {//contains :判断某个点（如鼠标点击点）是否在这个矩形区域内
         //用户左键点击的那个位置，是不是头像 QLabel 的区域
            // 打开文件选择对话框，并设置图片文件过滤器
            QString fileName = QFileDialog::getOpenFileName(
                this,                                 // 父窗口
                tr("Open Image File"),                // 对话框标题
                QDir::homePath(),                     // 初始路径，默认为用户的主目录
                tr("Images (*.png *.xpm *.jpg *.jpeg *.bmp *.gif);;All Files (*)") // 过滤器：图片格式 + 所有文件
                );

            // 如果没有选择文件，返回
            if (fileName.isEmpty()) {
                QMessageBox::information(this, tr("提示:"), tr("没有选择任何图片"));
                return;
            }
            chatdb::instance();

            // 打开图片文件
            QFile file(fileName);
            if (!file.open(QIODevice::ReadOnly)) {//只读形式
                qWarning() << "Failed to open image file!";
                return ;
            }
            QByteArray imageData = file.readAll();  // 读取图片为字节流
            // 插入数据到数据库
            QSqlQuery query;
            query.prepare("UPDATE header SET image = :image_data WHERE username = :username");
            query.bindValue(":image_data", imageData);  // 确保 imageData 是合适的二进制数据
            query.bindValue(":username", CellMain::username);  // 确保 CellMain::username 是有效的字符串或合适的类型
            if (!query.exec()) {

                qDebug() << "插入图片失败:" <<query.lastError().text();
                return ;
            }

            // 输出选择的图片文件路径
            qDebug() << "Selected image file:" << fileName;
            ui->headerl->setScaledContents(true);
            ui->headerl->setPixmap(QPixmap(fileName));


        }
    }

}



void CellMain::createGroup()
{
    qDebug()<<"被点击！";
    //弹出一个对话框，显示出所有的好友(简单的可以用QPushButton或者QToolButton来显示),选完以后
    //遍历所有的按键，看那些被选中了，被选中的id取出，取一个群名，然后建立群聊！发送信号，建立对应的item以及聊天框
    // 创建对话框
    QDialog dialog;//创建了一个对话框窗口对象，名字叫 dialog，不会自动显示，只有调用 .exec() 或 .show() 才会弹出来。
    QVBoxLayout layout(&dialog);//创建了一个垂直布局器，名字叫 layout；
    //构造时指定 &dialog 为父容器，表示这个布局器是 dialog 对话框的主布局
    //接下来往 layout 添加的控件（如 QCheckBox、QLineEdit、QPushButton），都会自动在 dialog 上显示出来并按垂直顺序排列。

    // 创建复选框，将listwidget里面的好友加入到复选框中
    QList<QCheckBox*> l;
    for(int i=0;i<ui->listWidget->count();i++){
        QListWidgetItem *item = ui->listWidget->item(i);
        QWidget *widget = ui->listWidget->itemWidget(item);  // 获取QWidget
        QLabel *labelStr=widget->findChild<QLabel*>("label_2");
        qDebug()<<labelStr->text();

        QStringList parts = labelStr->text().split(':');
        if(parts[0]!="群聊"){
            auto pointer=new QCheckBox(labelStr->text());
            l.append(pointer);
            layout.addWidget(pointer);
        }

    }

    // 确认按钮
    QPushButton *okButton = new QPushButton("确认");

    //群的群名
    QLineEdit *lineEdit=new QLineEdit;
    lineEdit->setPlaceholderText("请输入群名……");

    layout.addWidget(lineEdit);
    layout.addWidget(okButton);


    // 点击按钮时打印选中的项
    QObject::connect(okButton, &QPushButton::clicked, [&]() {
        if(!lineEdit->text().isEmpty()){
            //进行好友群聊消息的添加
            QString groupUserName="~";

            //聊天框
            chatWindows *cw=new chatWindows(this);
            ui->stackedWidget->addWidget(cw);
            chatWindowsList.append(cw);

            //创建好友列表！！！！！！！
            //将自定义控件添加到list widget中
            FriendsItem *fi=new FriendsItem(this);
            fi->setId("群聊:"+lineEdit->text());
            //给聊天主界面设置身份
            cw->strId="group:"+lineEdit->text();

            connect(fi, &FriendsItem::clicked, this, &CellMain::handleFriendItemClicked);
            connect(this,&CellMain::updateIsRuning,fi,&FriendsItem::updateIsRuning);
            friendsItemList.append(fi);


            fi->setHeadImage("border-radius:30; border-image:url(:/qunliao.png);");
            //创建好友容器
            QListWidgetItem * m_Item=new QListWidgetItem(ui->listWidget);
            //设置宽高,m_Item的宽高
            m_Item->setSizeHint(QSize(281,90));
            //将自定义好友项添加到widgetItem中，再将widgetItem添加到widgetlist中
            ui->listWidget->setItemWidget(m_Item,fi);
            //改变点击后的样式
            ui->listWidget->setStyleSheet("QListWidget::item:selected{background-color:rgb(200,200,200);};");

            for(auto &x:l){
                if(x->isChecked()){
                    qDebug()<<x->text().mid(3,x->text().count()-3);
                    groupUserName+=x->text().mid(3,x->text().count()-3)+"~";
                }

            }
            qDebug()<<"发送自己的:"<<CellMain::username;
            groupUserName+=CellMain::username;

            //像服务端发生创建群聊的消息

            my_TcpSocket::instance()->writeToServer(groupUserName+"~"+lineEdit->text()+'~');
            //在客户端中添加这个群聊群



            for(auto &x:l) x->deleteLater();
            dialog.accept();
        }else{
            QMessageBox::information(this,"提示","请给群聊起一个名字");
        }
    });

    dialog.move(QCursor::pos());  // 将对话框移到鼠标位置
    dialog.setFixedSize(200,300);
    dialog.setStyleSheet(QString("background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:0, stop:0 rgba(251,102,102, 220), stop:1 rgba(20,196,188, 230));border:none;border-radius:10px;"));
    dialog.exec();  // 显示对话框并等待用户操作



}

//查表设置头像
void CellMain::findheader()
{
    chatdb::instance();
    QSqlQuery query;


    if (!query.exec("select image from header where username='"+CellMain::username+"';")) {
        qDebug()<<"查询头像失败";
        //设置默认头像
        ui->headerl->setScaledContents(true);
        ui->headerl->setPixmap(QPixmap(":/friend.png"));
        qDebug() << "Query failed:" <<query.lastError().text();
        return ;
    }

    while(query.next()){
        if(query.value("image").toByteArray().isNull()==true){
            //设置默认头像
            ui->headerl->setScaledContents(true);
            ui->headerl->setPixmap(QPixmap(":/friend.png"));

        }else{
            image_data=query.value("image").toByteArray();
            // 使用 QPixmap::loadFromData 来加载图片
            QPixmap pixmap;
            if (pixmap.loadFromData(image_data)) {
                qDebug() << "Image loaded successfully!";
                // 可以在这里使用 QPixmap，例如将其显示在 QLabel 上
                ui->headerl->setScaledContents(true);
                ui->headerl->setPixmap(pixmap);

            } else {
                qDebug() << "Failed to load image from data.";
            }

        }
    }


}



void CellMain::handleReadyRead()
{

    QByteArray ba=c_mysocket->recvFromServer();
    qDebug()<<"服务端发送消息过来为："<<ba;
    QString strUserId(ba);
    //over表示结束加载好友列表的信号槽
    if(strUserId=="over"){
        //断开前用于加载好友聊天消息的列表！
        disconnect(c_mysocket->socket, &QTcpSocket::readyRead, this, &CellMain::handleReadyRead);
        connect(c_mysocket->socket,&QTcpSocket::readyRead,this,[=](){
            QByteArray showMessage=c_mysocket->recvFromServer();
            QString recvStr(showMessage);
            qDebug()<<recvStr;

            //这儿添加新功能时，这儿记得添加逻辑
            if(recvStr[0]!='$'&&recvStr[0]!='@'&&recvStr[0]!='&'&&recvStr[0]!='*'&&recvStr[0]!='~'){
                QStringList parts=recvStr.split('|');
                //将账号和消息取出，显示到对应的聊天框里面去
                userAndMessage.insert(parts[0],parts[1]);
                //发送信号，通知客户端处理消息
                emit handleMessage();
                qDebug()<<"接受消息为："<<showMessage;

            }
            //当前逻辑用于判断添加的好友是否存在！！！！！！！！
            else if(recvStr[0]=='@'||recvStr[0]=='&'){
                //添加好友时，好友不存在,自定义发出信号
                if(recvStr[0]=='@'){
                    QMessageBox::warning(this,"提示","添加的好友不存在");
                }else if(recvStr[0]=='&'){

                    emit a.transformStr(a.returnStr());
                    QMessageBox::information(this,"提示","添加成功");
                }

            }else if(recvStr[0]=='*') {
                recvStr.remove(0,1);
                QJsonDocument doc = QJsonDocument::fromJson(recvStr.toUtf8());
                // 获取JSON对象
                QJsonObject obj = doc.object();
                QJsonValue resultValue = obj.value("result");
                QString s = resultValue.toString();
                qDebug() << "Result:" << s;
                chatWindows *c=chatWindowsList.at(index_ai);
                c->to_show_ai_recv(s);

            }
            //建立群聊的消息
            else if(recvStr[0]=='~'){
                qDebug()<<recvStr.remove(0,1);
                //聊天框
                chatWindows *cw=new chatWindows(this);
                ui->stackedWidget->addWidget(cw);
                chatWindowsList.append(cw);

                //创建好友列表！！！！！！！
                //将自定义控件添加到list widget中
                FriendsItem *fi=new FriendsItem(this);
                fi->setId("群聊:"+recvStr);
                //给聊天主界面设置身份
                cw->strId="group:"+recvStr;

                connect(fi, &FriendsItem::clicked, this, &CellMain::handleFriendItemClicked);
                friendsItemList.append(fi);


                fi->setHeadImage("border-radius:30; border-image:url(:/qunliao.png);");
                //创建好友容器
                QListWidgetItem * m_Item=new QListWidgetItem(ui->listWidget);
                //设置宽高,m_Item的宽高
                m_Item->setSizeHint(QSize(281,90));
                //将自定义好友项添加到widgetItem中，再将widgetItem添加到widgetlist中
                ui->listWidget->setItemWidget(m_Item,fi);
                //改变点击后的样式
                ui->listWidget->setStyleSheet("QListWidget::item:selected{background-color:rgb(200,200,200);};");


            }
            //.txt结尾的文件传输
            else {

                // 使用正则表达式提取'111'
                QRegularExpression regex("\\+([0-9]+)");  // 匹配以'+'开头的数字
                QRegularExpressionMatch match = regex.match(showMessage);

                QString extracted;
                if (match.hasMatch()) {
                    extracted = match.captured(1); // 获取匹配的第一组
                    qDebug() << "提取的字符串:" << extracted;
                } else {
                    qDebug() << "没有匹配到字符串";
                    extracted="";
                }
                //移除账号提示内容
                int pos = showMessage.indexOf('\n');
                if (pos != -1) {
                    showMessage.remove(0, pos + 1);  // 移除从开始到 \n 的所有内容
                }

                // 创建一个 Yes/No 对话框
                QMessageBox msgBox;
                msgBox.setText("好友"+extracted+"向您发来了文件，请确认是否接收");
                msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
                msgBox.setDefaultButton(QMessageBox::No);

                // 显示对话框并获取返回值
                int ret = msgBox.exec();

                // 判断用户选择的是 Yes 还是 No
                if (ret == QMessageBox::Yes) {

                    //让用户输入保存传过来的字符串
                    QString inputText;
                    bool ok;
                    //windows路径下是以C,D盘开头
                    inputText=QFileDialog::getExistingDirectory(nullptr, "选择要保存文件的路径", "/Users/a1/",  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
                    if(!inputText.isEmpty()){
                        QString inputText_2 = QInputDialog::getText(nullptr, "输入对话框", "请输入一个保存文件的文件名且以.txt结尾:",QLineEdit::Normal, "", &ok);
                        QFile file(inputText+'/'+inputText_2);
                        qDebug()<<inputText+inputText_2;
                        if(!file.open(QIODevice::WriteOnly)){
                            QMessageBox::warning(this,"提示","无法写入该文件");
                        }else{
                            file.write(showMessage);
                            file.close();
                            for(auto &x:chatWindowsList){
                                if(x->strId==extracted){
                                    x->displayDoc("[已收到对方文件]");
                                    break;
                                }
                            }




                        }

                    }else{
                        QMessageBox::warning(this,"提示","所选路径有误或者未选择，请重新选择");
                    }



                }else{
                    for(auto &x:chatWindowsList){
                        if(x->strId==extracted){
                            x->displayDoc("[已拒收对方文件]");
                            break;
                        }
                    }
                }
            }
        });
        return;
    }
    //聊天框
    chatWindows *cw=new chatWindows(this);
    ui->stackedWidget->addWidget(cw);
    chatWindowsList.append(cw);

    //创建好友列表！！！！！！！
    //将自定义控件添加到list widget中
    FriendsItem *fi=new FriendsItem(this);

    // 使用split()方法，以冒号作为分隔符
    QStringList parts = strUserId.split(':');

    // 检查是否成功分割
    if (parts.size() == 2) {
        if(parts[0]=="group"){
            fi->setId("群聊:"+parts[1]);
        }else{
            fi->setId("ID:"+strUserId);
        }

    } else {
        fi->setId("ID:"+strUserId);
    }

    //给聊天主界面设置身份
    cw->strId=strUserId;

    connect(fi, &FriendsItem::clicked, this, &CellMain::handleFriendItemClicked);
    connect(this,&CellMain::updateIsRuning,fi,&FriendsItem::updateIsRuning);
    friendsItemList.append(fi);

    QSqlQuery query;

    if (!query.exec("select image from header where username='"+strUserId+"';")) {
        qDebug()<<"查询好友头像失败";
        //设置默认头像
          if(parts[0]!="group"){
        fi->setHeadImage("border-radius:30; border-image:url(:/friend.png);");
          }else{
              fi->setHeadImage("border-radius:30; border-image:url(:/qunliao.png);");
          }
        qDebug() << "Query failed:" <<query.lastError().text();

    }
    //fi->setHeadImage("border-radius:30; border-image:url(:/friend.png);");
    int flag=0;
    while(query.next()){
        if(query.value("image").toByteArray().isNull()==true){
            //设置默认头像
            //设置默认头像
            if(parts[0]!="group"){
                fi->setHeadImage("border-radius:30; border-image:url(:/friend.png);");
            }else{
                fi->setHeadImage("border-radius:30; border-image:url(:/qunliao.png);");
            }
            flag=1;

        }else{
            image_data=query.value("image").toByteArray();
            // 使用 QPixmap::loadFromData 来加载图片
            QPixmap pixmap;
            if (pixmap.loadFromData(image_data)) {
                qDebug() << "Image loaded successfully!";
                // 可以在这里使用 QPixmap，例如将其显示在 QLabel 上
                fi->setheader(pixmap);
                flag=1;


            } else {
                //设置默认头像
                if(parts[0]!="group"){
                    fi->setHeadImage("border-radius:30; border-image:url(:/friend.png);");
                }else{
                    fi->setHeadImage("border-radius:30; border-image:url(:/qunliao.png);");
                }
                qDebug() << "Failed to load image from data.";
                flag=1;
            }

        }
    }
    if(flag==0) {       //设置默认头像
        if(parts[0]!="group"){
            fi->setHeadImage("border-radius:30; border-image:url(:/friend.png);");
        }else{
            fi->setHeadImage("border-radius:30; border-image:url(:/qunliao.png);");
        }
    }






    //创建好友容器
    QListWidgetItem * m_Item=new QListWidgetItem(ui->listWidget);
    //设置宽高,m_Item的宽高
    m_Item->setSizeHint(QSize(281,90));
    //将自定义好友项添加到widgetItem中，再将widgetItem添加到widgetlist中
    ui->listWidget->setItemWidget(m_Item,fi);
    //改变点击后的样式
    ui->listWidget->setStyleSheet("QListWidget::item:selected{background-color:rgb(200,200,200);};");
    //
}

void CellMain::handleFriendItemClicked()
{
    //点击谁，谁就出现聊天框的逻辑为
    //将frienditem对象和chatwidget同时存储在对象的链表上，因为是同时存储，所以索引相同
    //当点击一个item对象，获得他在对象链表中的索引，该索引也是对应的chatwidget的索引位置！
    //因此获得其索引，就可调出对应的chat widget窗口
    FriendsItem *fi = qobject_cast<FriendsItem *>(sender()); // 获取发送信号的对象
    if (!fi)
        return;


    int index = friendsItemList.indexOf(fi);
    if (index != -1 && index < chatWindowsList.size()) {
        ui->stackedWidget->setCurrentWidget(chatWindowsList.at(index));
        index_ai=index;
        qDebug() << "自定义控件被点击"<<index_ai;
    }
}

void CellMain::handleMessage_to_text()
{
    //遍历每一客户端发过来的消息
    //在上一级中，便利每一个list容器中的
    QMapIterator<QString, QString> i(userAndMessage);
    while (i.hasNext()) {
        i.next();
        qDebug() << "Key:" << i.key() << ", Value:" << i.value();
        QList<chatWindows *>::iterator it;
        for (it = chatWindowsList.begin(); it != chatWindowsList.end(); ++it) {
            chatWindows *cw=*it;
            // 找到第一个 ':' 字符的位置
            int pos = i.key().indexOf(':');
            // 提取从字符串开头到第一个 ':' 字符之间的部分
            QString prefix = i.key().left(pos + 1);  // 包括 ':' 字符

            if(prefix!="group:"){
                if(cw->strId==i.key()){
                    cw->to_show_friendmessage(i.key(),i.value());
                    userAndMessage.remove(i.key());
                    break;
                }
            }else{
                // 找到第一个和第二个 ':' 字符的位置
                int firstColonPos = i.key().indexOf(':');
                int secondColonPos = i.key().indexOf(':', firstColonPos + 1);

                // 提取从字符串开头到第二个 ':' 之前的部分
                QString result = i.key().left(secondColonPos);
                if(cw->strId==result){
                    cw->to_show_friendmessage(i.key(),i.value());
                    userAndMessage.remove(i.key());
                    break;
                }



            }

        }

    }

}

void CellMain::closeEvent(QCloseEvent *event)
{
    QSqlQuery query;
    query.prepare("update isUserRuning set isRuning=:isRuning where username=:username");
    query.bindValue(":isRuning",1);
    query.bindValue(":username",username);
    if(query.exec()){
        qDebug()<<"用户退出,在线用户更新成功";
    }else{
        qDebug()<<"用户退出，用户在线列表更新失败";
    }


    exit(0);
}




void CellMain::on_addFriend_clicked()
{

    a.exec();
}
//删除好友
void CellMain::on_deleteFriend_clicked()
{
    deletefriend df;
    df.exec();
}
//修改密码
void CellMain::on_applyFriend_clicked()
{
    checkpassword cp;
    cp.exec();
}




void CellMain::on_addFriend_triggered(QAction *arg1)
{

}





//朋友圈
void CellMain::on_friendsitem_clicked()
{

    cir->initfriends();
    cir->show();


}

