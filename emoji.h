#ifndef EMOJI_H
#define EMOJI_H

#include <QDialog>
#include<QGridLayout>
#include<QLabel>
#include<QStringList>
#include<QString>
#include<QMouseEvent>
namespace Ui {
class emoji;
}

class emoji : public QDialog
{
    Q_OBJECT

public:
    explicit emoji(QWidget *parent = nullptr);
    ~emoji();

signals:
    void emitString(QString emoji);
private:
    Ui::emoji *ui;
};





#endif // EMOJI_H
