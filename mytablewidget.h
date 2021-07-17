#ifndef MYTABLEWIDGET_H
#define MYTABLEWIDGET_H

#include "mainwindow.h"
#include <QWidget>
#include <QTableWidget>
#include <QMouseEvent>

/*
* 定义了一个mytablewidget类，
* 和基类tablewidget相比，
* 只多了一个事件 mousePressEvent
* 的右键弹出菜单,
* 在控件里把原来的TableWidget提升为该类，就可以实现了
*/

class MyTableWidget : public QTableWidget
{
    Q_OBJECT

public:
    explicit MyTableWidget(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event);

signals:

};

class FavorWidget: public QTableWidget
{
public:
    explicit FavorWidget(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event);

};


#endif // MYTABLEWIDGET_H
