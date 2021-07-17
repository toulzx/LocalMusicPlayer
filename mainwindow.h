#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlaylist>
#include <QStringList>
#include <QDir>
#include <QMediaPlayer>
#include <QSlider>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include<QDialog>
#include "dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void contextMenuEvent(QContextMenuEvent *event);

private:
    Ui::MainWindow *ui;

    bool isPlay=false;                              //判断是否在播放
    bool isFavor=false;                             //判断是否选中收藏歌单歌曲

    QString MusicPath= "C:\\Users\\lzx\\Desktop\\musicplayer\\music\\Demo songs";   //本地路径                        //请修改！！！！！！！<<<<<<<<--------<<<<<<<---------<<<<<<--------<<<<<<<<<(尽量都用“\\”分割)
    QString FilePath="C:\\Users\\lzx\\Desktop\\musicplayer\\music\\favor.txt";   //请修改此处地址以及文件夹名称！！！！！！！<<<<<<<<--------<<<<<<<---------<<<<<<--------<<<<<<<<<(尽量都用“\\”分割)
    QString Iconpath="C:\\Users\\lzx\\Desktop\\musicplayer\\music";              //请修改！！！！！！！<<<<<<<<--------<<<<<<<---------<<<<<<--------<<<<<<<<<(尽量都用“\\”分割)

    QString CurrentSong;                            //当前播放歌曲名
    QString ClickedSong;                            //鼠标选中歌曲名
    QString ClickedAddress;                         //鼠标选中歌曲名对应的地址

    QStringList fileList;                           //默认位置下扫描出来的歌曲名（载入默认列表）
    QStringList FavorList;                          //收藏列表的歌曲名
    QStringList SongsAddress;                       //默认列表内歌曲地址
    QStringList FavorAddress;                       //收藏列表内歌曲地址

    QMediaPlaylist *PlayerList;
    QMediaPlaylist *favorPlayerList;
    QMediaPlaylist *No;
    QMediaPlayer *Player;
    QTime *currentTime;
    QTime *totalTime;
    QSlider *seekSlider;
    QMediaPlaylist::PlaybackMode n=QMediaPlaylist::Loop;
                                                    //播放模式


private slots:
   void init();
   void addItem(QString name);                      //添加tablewidget 歌单
   void FavorItem();                                //lzx收藏的歌单
   void ItemClicked();                              //lzx单击tablewidget单元格
   void ItemClicked_2();                            //lzx单击tablewidget_2（收藏歌单）单元格时
   QStringList getFileNames(const QString &path);   //获取路径下所有文件类型
   void SlotButtonStart();                          //播放按钮的槽，可以改变形状
   void SlotButtonPrevious();                       //上一首
   void SlotButtonNext();                           //下一首
   void SlotShowCurrentMedia();                     //显示当前歌曲
   void playto(int,int);                            //点播
   void playto2(int,int);
   void setPlayerPosition(int position);            //进度条改变时播放位置随着改变
   void durationChanged(qint64 duration);           //总时间设置
   void positionChanged(qint64 position);           //时间变化设置
   void SlotButtonMode();                           //切换播放模式
   void ClearMusicList();                           //清空列表
   void ClearMusicList_2();                         //清空列表
   void DelMusicFilesFromList();                    //删除列表歌曲
   void Search();                                   //搜索tableWigdet列表歌曲
   void Search_2();                                 //搜索tableWigdet_2列表歌曲

   void receiveData(QString data);   //接收传递过来的数据的槽用来搜索tableWigdet
   void receiveData_2(QString data);//接收传递过来的数据的槽用来搜索tableWigdet_2


   void on_ButtonAdd_clicked();
   void on_ButtonFavor_clicked();
   void on_ButtonRemove_clicked();



};



#endif // MAINWINDOW_H
