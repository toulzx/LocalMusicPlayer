#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDialogButtonBox>

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

public slots:
    void on_clickedBox_accepted();

private:
    Ui::Dialog *ui;          //创建ui界面
    QString text;           //用以储存输入内容


signals:
    void sendData(QString);   //用来传递数据的信号
};

#endif // DIALOG_H
