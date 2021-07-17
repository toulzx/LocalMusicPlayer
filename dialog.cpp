#include "dialog.h"
#include "ui_dialog.h"
#include <QCompleter>
#include <QListView>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDialogButtonBox>
#include <QMessageBox>


Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

Dialog::~Dialog()
{
    delete ui;
}



void Dialog::on_clickedBox_accepted()
{
    emit sendData(ui->lineEdit->text());  //获取lineEdit的输入并且传递出去

}
