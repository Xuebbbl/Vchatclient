#include "emoji.h"
#include "ui_emoji.h"
#include<QToolBar>
#include<QPushButton>
#include<QMessageBox>
emoji::emoji(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::emoji)
{
    ui->setupUi(this);
    //创建一个网格布局
    QGridLayout *layout=new QGridLayout(this);

    QList<QStringList> emojiGrid;
    QStringList row1;
    row1 << "😊" << "😄" << "😍";
    QStringList row2;
    row2 << "😎" << "😢" << "😡";
     QStringList row3;
    row3 << "🤣" << "😅" << "😜";
     QStringList row4;
    row4<<"👍"<<"😒"<<"💛";
     QStringList row5;
    row5<<"🌈"<<"😭"<<"🎁";
     emojiGrid << row1 << row2 << row3<<row4<<row5;

    //遍历每一个元素，并用QLabel来将他们进行存储
    int row=0;
     for( QStringList &rowEmojis:emojiGrid)
    {
        int col=0;
        for(const QString &emoji:rowEmojis){
            QLabel *label=new QLabel(emoji,this);
            QPushButton *p=new QPushButton(this);
            label->setFixedSize(50,50);
            p->setFixedSize(50,50);
            p->setStyleSheet("QPushButton { background-color: transparent; border: none; }");
            connect(p,&QPushButton::clicked,this,[=](){
                emitString(emoji);
            });

            layout->addWidget(label,row,col);
            layout->addWidget(p,row,col);
            col++;
        }
        row++;
    }
    setLayout((layout));
    setWindowTitle("表情");
    setFixedSize(200,200);
}

emoji::~emoji()
{
    delete ui;
}
