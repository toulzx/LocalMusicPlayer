#ifndef QMYLISTWIDGET_H
#define QMYLISTWIDGET_H

#include <QListWidget>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QMouseEvent>

class QMyListWidget
{
public:
    QMyListWidget(QWidget *parent);
        ~QMyListWidget();
protected:
    void mousePressEvent(QMouseEvent *event);
private:
    QMenu *m_contextMenu;
        QAction *m_addAction;
        QAction *m_delAction;
};

#endif // QMYLISTWIDGET_H
