#include "mytablewidget.h"

extern QMenu *contextMenu;

MyTableWidget::MyTableWidget(QWidget *parent) : QTableWidget(parent){}

void MyTableWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()== Qt::RightButton)               //如果右键点击，在当前位置显示菜单
    {
        contextMenu->exec(QCursor::pos());
    }
    QTableWidget::mousePressEvent(event);
}
