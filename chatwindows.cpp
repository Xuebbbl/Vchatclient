#include "chatwindows.h"
#include "ui_chatwindows.h"
#include<QDebug>
#include<QTextDocument>
#include<QTextEdit>
#include<QMargins>
#include<QResizeEvent>
#include<QTextDocument>
#include<QTextFrame>
#include<emoji.h>
#include<QFileDialog>
#include<QFile>
#include<QMessageBox>
#include<QDateTime>
#include"my_tcpsocket.h"

#include<QJsonDocument>
#include<QJsonObject>
#include<QJsonValue>
#include"cellmain.h"
//对于聊天界面，不过，由于QTextEdit的HTML支持有限，并且直接操作HTML内容可能比较复杂，
//一个更简单的方法可能是使用QGraphicsScene和QGraphicsView来管理聊天界面，
//其中每个消息（包括文本和图片）都可以是一个QGraphicsItem。这样在聊天界面中就可以显示独立的每一个item，对于每一个item又可以
//构造独立的气泡和头像显示！

chatWindows::chatWindows(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::chatWindows)
{
    ui->setupUi(this);
    m_tcpsocket=my_TcpSocket::instance();
    ui->toolButton->setStyleSheet("QToolButton { background-color: transparent; border: none; }");
    ui->toolButton_2->setStyleSheet("QToolButton { background-color: transparent; border: none; }");
    ui->toolButton_3->setStyleSheet("QToolButton { background-color: transparent; border: none; }");
    ui->toolButton_4->setStyleSheet("QToolButton { background-color: transparent; border: none; }");

    //添加设置聊天背景
    m.insert(0,"QListWidget  { "
                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
                "stop: 0 #ff5733, stop: 0.5 #ff8c00, stop: 1 #ffaf00); "
                "border: none; "
                "padding: 10px; "
                "}"
                "QTextEdit  { "
                "background-color: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
                "stop: 0 #ff5733, stop: 0.5 #ff8c00, stop: 1 #ffaf00); "
                "border: none; "
                "padding: 10px; "
                "}"

             );
    m.insert(1,"QListWidget {background-color:black}  QTextEdit {background-color:black}"    );
    m.insert(2,"QListWidget {background-color:white} QTextEdit {background-color:white}");
    m.insert(3,"QListWidget  { "
                "    background-image: url(:/WechatIMG36.jpeg); "
                "    background-repeat: no-repeat; " // 不重复图片
                "    background-position: center; " // 图片居中显示
                "    border: none; " // 去除边框
                "    padding: 10px; " // 内边距
                "    font-family: 'Arial', sans-serif; "
                "    font-size: 14px; "
                "}"

                "QTextEdit  { "
                "    background-image: url(:/WechatIMG36.jpeg); "
                "    background-repeat: no-repeat; " // 不重复图片
                "    background-position: center; " // 图片居中显示
                "    border: none; " // 去除边框
                "    padding: 10px; " // 内边距
                "    font-family: 'Arial', sans-serif; "
                "    font-size: 14px; "
                "}"


             );
    // textCharFormat_h=new QTextCharFormat;
    // textCharFormat_m=new QTextCharFormat;


     // textCharFormat_h->setBackground(Qt::white);
     // textCharFormat_h->setFontPointSize(28);

     // textCharFormat_m->setBackground(QColor("#AEEA00"));
     // textCharFormat_m->setFontPointSize(28);

     textEdit=std::make_unique<TextEditorWindow>();
     //初始化智能回复
     handler=new ApiHandler;
     connect(handler,&ApiHandler::reciveString,[=](QString str){
         if(str!="noAnswer") {ui->textEdit_2->append(str);}
         isTansrom=true;
         ui->pushButton_2->setText("ai智能回复");
         ui->pushButton_2->setEnabled(true);
     });

   //添加模型
     ui->comboBox->addItem(QIcon(":/zhinengyun.png"),"常用基础Ai");
     ui->comboBox->addItem(QIcon(":/zhinengyun.png"),"deepseek_r1");
     ui->comboBox->addItem(QIcon(":/zhinengyun.png"),"deepseek_v3");
     ui->comboBox->setCurrentIndex(0);

    ui->listWidget->clearSelection();  // 设置所有项为未选中
     ui->listWidget->setSelectionMode(QAbstractItemView::NoSelection); // 禁止任何项被选中



}

chatWindows::~chatWindows()
{
    delete ui;
    delete handler;
}

void chatWindows::to_show_friendmessage(QString username,QString message)
{

    //cursor_2.movePosition(QTextCursor::End);使用这串代码表示的意思是移动到当前文本的末尾，使用此代码会导致
    //设置的文本格式失效！文本cursor表示光标的意思，可以通过cursor设置当前行！！！！光标对应的文本的样式，换行以后会失效，需重新设置！


    // QTextCursor cursor=ui->textEdit->textCursor();


    // QTextBlockFormat b;
    // b.setAlignment(Qt::AlignCenter);
    // cursor.mergeBlockFormat(b);
    // QTextCharFormat c;
    // cursor.setCharFormat(c);


    // QDateTime curr=QDateTime::currentDateTime();
    // QString time=curr.toString("yyyy-MM-dd hh:mm:ss");
    // cursor.insertText(time+'\n');



    // QTextCursor cursor2=ui->textEdit->textCursor();

    // QTextBlockFormat blockFormat;
    // blockFormat.setAlignment(Qt::AlignLeft);
    // cursor2.mergeBlockFormat(blockFormat);
    // // QTextCharFormat charFormat;
    // // charFormat.setBackground(Qt::white);
    // // charFormat.setFontPointSize(28);
    // cursor2.setCharFormat(*textCharFormat_h);


    qDebug()<<"show message："<<username;
    //判断是群消息还是其他
    // 找到第一个 ':' 字符的位置
    int pos = username.indexOf(':');
    // 提取从字符串开头到第一个 ':' 字符之间的部分
    QString prefix = username.left(pos + 1);  // 包括 ':' 字符
    if(prefix=="group:"){
        int pos = username.lastIndexOf(':');
        // 从该位置开始提取子字符串，即提取':之后的部分'
        QString number = username.mid(pos + 1);  // 从 ':' 后面开始到字符串结束
       // cursor2.insertText("群友"+number+": "+message);
        addMessage(message,false,number);
    }else
    {
        //cursor2.insertText(username+"说: "+message);
        addMessage(message,false,username);
    }




    // cursor2.insertText("\n");
    // ui->textEdit->ensureCursorVisible();

    ai_to_recv=message;

}

void chatWindows::to_show_ai_recv(QString str)
{
    ui->textEdit_2->append(str);
    isTansrom=true;
}

void chatWindows::displayDoc(QString information)
{
    // QTextCursor cursor=ui->textEdit->textCursor();
    // QTextBlockFormat b;
    // b.setAlignment(Qt::AlignCenter);
    // cursor.mergeBlockFormat(b);
    // QTextCharFormat c;
    // cursor.setCharFormat(c);

    // QDateTime curr=QDateTime::currentDateTime();
    // QString time=curr.toString("yyyy-MM-dd hh:mm:ss");

    // cursor.insertText(time+'\n');

    // QTextCursor cursor2=ui->textEdit->textCursor();
    // cursor2=ui->textEdit->textCursor();
    // QTextBlockFormat blockFormat;
    // blockFormat.setAlignment(Qt::AlignRight);
    // cursor2.mergeBlockFormat(blockFormat);
    // QTextCharFormat charFormat;
    // charFormat.setBackground(QColor("#AEEA00"));
    // charFormat.setFontPointSize(28);
    // cursor2.setCharFormat(charFormat);

    // cursor2.insertText(information);


    // cursor2.insertText("\n");
    // ui->textEdit->ensureCursorVisible();
    addMessage(information,true,CellMain::username);

}

void chatWindows::addMessage(const QString &text, bool isOutgoing,QString username)
{
    QSqlQuery query;
    QPixmap pixmap;
    if(username!=""){
      if (!query.exec("select image from header where username='"+username+"';")) {
        qDebug()<<"查询好友头像失败";

        }
         while(query.next()){
             if(query.value("image").toByteArray().isNull()!=true){
                 QByteArray image_data=query.value("image").toByteArray();
                 // 使用 QPixmap::loadFromData 来加载图片
                 pixmap.loadFromData(image_data);
             }

         }

    }

    QListWidgetItem* item = new QListWidgetItem();
    item->setSelected(false);


    ChatMessageItem::MessageType type = isOutgoing ?
                                            ChatMessageItem::Outgoing :
                                            ChatMessageItem::Incoming;

    ChatMessageItem* widget = new ChatMessageItem(
        nullptr,
        text,
        // QPixmap(isOutgoing ? "/Users/a1/Desktop/11.png" : "/Users/a1/Desktop/11.png"),
        pixmap.isNull()==true?QPixmap(":/friend.png"):pixmap,
        type
        );
    // 计算文本的宽度
    // QFontMetrics fm(widget->m_contentLabel->font());
    // int textWidth = fm.horizontalAdvance(text);
    item->setSizeHint(QSize(0, chatSize));  // 设置合适的高度
    if(flag==1){
        widget->setstyle(this->font,this->color,this->fontsize,1);
    }


    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item,widget);
    ui->listWidget->scrollToBottom();//设置强制滚动到最低部

}











void chatWindows::on_pushButton_clicked()
{

    if(isTansrom==true){
        if(ui->textEdit_2->toPlainText().isEmpty()==true) return;

        m_tcpsocket->writeToServer(":"+strId+":"+ui->textEdit_2->toPlainText()+":");
        qDebug()<<strId;
        // QTextCursor cursor=ui->textEdit->textCursor();


        // QTextBlockFormat b;
        // b.setAlignment(Qt::AlignCenter);
        // cursor.mergeBlockFormat(b);
        // QTextCharFormat c;
        // cursor.setCharFormat(c);



        // QDateTime curr=QDateTime::currentDateTime();
        // QString time=curr.toString("yyyy-MM-dd hh:mm:ss");

        // cursor.insertText(time+'\n');



        // QTextCursor cursor2=ui->textEdit->textCursor();
        // cursor2=ui->textEdit->textCursor();
        // QTextBlockFormat blockFormat;
        // blockFormat.setAlignment(Qt::AlignRight);
        // cursor2.mergeBlockFormat(blockFormat);
        // // QTextCharFormat charFormat;
        // // charFormat.setBackground(QColor("#AEEA00"));
        // // charFormat.setFontPointSize(28);
        // cursor2.setCharFormat(*textCharFormat_m);

        // cursor2.insertText(ui->textEdit_2->toPlainText());
        addMessage(ui->textEdit_2->toPlainText(),true,CellMain::username);


        // cursor2.insertText("\n");
        // ui->textEdit->ensureCursorVisible();



        ui->textEdit_2->clear();
    }else{
        QMessageBox::warning(this,"提示","请等待ai助手回复后发送！");

    }
    //qDebug()<<111;

}



void chatWindows::on_toolButton_clicked()
{
    emoji e;
    connect(&e,&emoji::emitString,this,[=](QString str){
        ui->textEdit_2->append(str);
    });
    e.exec();
}


void chatWindows::on_toolButton_2_clicked()
{
    QString fileName=QFileDialog::getOpenFileName(this,"选择文件",".","所有文件 (*);;文本文件 (*.txt)");
    if(!fileName.isEmpty()){
        qDebug()<<"你选择的文件为："<<fileName;
    }
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly)){
        qDebug() << "无法打开文件：" << fileName;
        return;
    }
    QByteArray fileData = file.readAll();
    my_TcpSocket::instance()->writeToServer(QString('$'+strId+'+'+CellMain::username+fileData));
    displayDoc("[文件发送成功！]");


}


void chatWindows::on_toolButton_3_clicked()
{
    i++;
    if(i>=4) {i=0;}

    ui->listWidget->setStyleSheet(m[i]);

    ui->textEdit_2->setStyleSheet(m[i]);

    qDebug()<<i;
}

//智能回复这里
void chatWindows::on_pushButton_2_clicked()
{


    if(ai_to_recv.isEmpty()!=true){
        //my_TcpSocket::instance()->writeToServer(ai_to_recv+",请非常精简的回答");

          handler->setProblem(ai_to_recv);

          //判断用户选择的什么模型
          switch (ui->comboBox->currentIndex()) {
          case 0:
              handler->startProcess(aiModel::commonAi);
              break;
          case 1:
              handler->startProcess(aiModel::deepseek_r1);
              break;
          case 2:
              handler->startProcess(aiModel::deepseek_v3);
              break;
          default:
              break;
          }
          // if(ui->comboBox->currentText()=="常用基础Ai"){

          // }else if(ui->comboBox->currentText()=="deepseek_r1"){
          // handler->startProcess(aiModel::deepseek_r1);
          // }else if()


           isTansrom=false;
           ui->pushButton_2->setText("等待ai回复中");
           ui->pushButton_2->setEnabled(false);

    }else{
        QMessageBox::warning(this,"提示","好友未发送消息到达");
    }







}

//字体选择
void chatWindows::on_toolButton_4_clicked()
{


    textEdit->setWindowTitle("聊天文字字体设置");
    textEdit->exec();
    // textCharFormat_m->setFont(textEdit->font);
    // textCharFormat_m->setFontPointSize(textEdit->fontsize);
    // textCharFormat_m->setForeground(textEdit->color);

    // textCharFormat_h->setFont(textEdit->font);
    // textCharFormat_h->setFontPointSize(textEdit->fontsize);
    // textCharFormat_h->setForeground(textEdit->color);
    this->font=textEdit->font;



    this->fontsize=textEdit->fontsize;
    chatSize=(this->fontsize*2);
    this->color=(textEdit->color);

    flag=1;

}

