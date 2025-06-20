#ifndef SHOWMESSAGE_H
#define SHOWMESSAGE_H

#include <QListWidget>
#include <QListWidgetItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPixmap>

// 自定义聊天消息项
class ChatMessageItem : public QWidget {
    Q_OBJECT
public:
    enum MessageType {
        Incoming,  // 好友消息（左侧）
        Outgoing    // 自己消息（右侧）
    };
    ChatMessageItem(QWidget* parent = nullptr,
                    const QString& text = "",
                    //这里默认改为自己头像
                    const QPixmap& avatar = QPixmap(":/friend.png"),
                    MessageType type = Incoming)
        : QWidget(parent), m_type(type)
    {
        setupUI(text, avatar);
        applyStyle();
    }
public:
    void setupUI(const QString& text, const QPixmap& avatar) {
        // 头像标签
        m_avatarLabel = new QLabel(this);
        m_avatarLabel->setPixmap(avatar.scaled(40, 40, Qt::KeepAspectRatio));
        m_avatarLabel->setFixedSize(40, 40);
        // 消息内容
        m_contentLabel = new QLabel(text, this);
        m_contentLabel->setWordWrap(true);
       m_contentLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        // 主布局
        QHBoxLayout* mainLayout = new QHBoxLayout(this);






        if (m_type == Incoming) {

            mainLayout->addWidget(m_avatarLabel);
            mainLayout->addWidget(m_contentLabel, 1);
              mainLayout->addStretch(1);
        } else {
             mainLayout->addStretch(1);
            mainLayout->addWidget(m_contentLabel, 1);
            mainLayout->addWidget(m_avatarLabel);

        }
        this->setLayout(mainLayout);
    }
    void applyStyle() {
        // 气泡样式
        QString style;
        if (m_type == Incoming) {


            style = "QLabel {"
                    //"  background: #95EC69;"
                    "  background: #FFFFFF;"
                    "  border-radius: 8px;"
                    "  padding: 8px 12px;"
                    "  margin: 2px 5px;"
                    "}";
        } else {
            style = "QLabel {"
                    //"  background: #FFFFFF;"
                    "  background: #95EC69;"
                    "  border-radius: 8px;"
                    "  padding: 8px 12px;"
                    "  margin: 2px 5px;"
                    "}";
        }
        m_contentLabel->setStyleSheet(style);
    }

    void setstyle(QFont font,QColor color,int fontsize,int flag=0){
        if(flag!=0){
            font.setPointSize(fontsize);
            m_contentLabel->setFont(font);
            // 设置字体颜色为绿色
            QPalette palette;
            palette.setColor(QPalette::Text, color);  // 绿色
            m_contentLabel->setPalette(palette);
            m_contentLabel->setStyleSheet("QLabel {font-size: "+QString::number(fontsize)+"pt}");
        }

    }
    MessageType m_type;
    QLabel* m_avatarLabel;
    QLabel* m_contentLabel;
};





#endif // SHOWMESSAGE_H
