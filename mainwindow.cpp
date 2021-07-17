#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QTime"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include<QTreeView>
#include<QString>
#include<string>
#include"dialog.h"
#include"ui_dialog.h"
#include<QDialog>
#include <QCompleter>
#include <QListView>
#include <QMenu>


QMenu *contextMenu;
QMenu *contextMenu_2;
QMenu *delMenu;




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //播放列表中右键菜单
    contextMenu = new QMenu(this);
    contextMenu_2 = new QMenu(this);
    delMenu = new QMenu("删除歌曲",this);
    contextMenu->addAction("添加歌曲",this,SLOT(on_ButtonAdd_clicked()));
    contextMenu->addAction("清空播放列表",this,SLOT(ClearMusicList()));
    contextMenu_2->addAction("清空播放列表",this,SLOT(ClearMusicList_2()));
    contextMenu->addAction("给我搜！",this,SLOT(Search()));
    contextMenu_2->addAction("给我搜！",this,SLOT(Search_2()));
    delMenu->addAction("移除歌曲",this,SLOT(DelMusicFilesFromList()));
    delMenu->addAction("清空播放列表",this,SLOT(ClearMusicList()));
    contextMenu->addMenu(delMenu);
    delMenu->show();

    init();                     //初始化界面


    if(PlayerList!=nullptr)
    {
        //播放控件
        connect(ui->ButtonStop,&QPushButton::clicked,this,&MainWindow::SlotButtonStart);
        connect(ui->ButtonNext,&QPushButton::clicked,this,&MainWindow::SlotButtonNext);
        connect(ui->ButtonPrevious,&QPushButton::clicked,this,&MainWindow::SlotButtonPrevious);
        connect(ui->ButtonMode,&QPushButton::clicked,this,&MainWindow::SlotButtonMode);
        //双击列表
        connect(ui->tableWidget,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(playto(int,int)));
        connect(ui->tableWidget_2,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(playto2(int,int)));
        //单击列表
        connect(ui->tableWidget,SIGNAL(itemClicked (QTableWidgetItem *)),this, SLOT(ItemClicked()) );   //获取选中的歌曲名称
        connect(ui->tableWidget_2,SIGNAL(itemClicked (QTableWidgetItem *)),this, SLOT(ItemClicked_2()) );
        //进度条
        connect(ui->seekSlider,SIGNAL(sliderMoved(int)),this,SLOT(setPlayerPosition(int)));  //进度条改变时播放位置随着改变
        connect(Player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChanged(qint64)));
        connect(Player,SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));// 总时间设置
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    PlayerList = new QMediaPlaylist();               //实例化播放列表和播放功能
    favorPlayerList =new QMediaPlaylist;

    //默认歌单初始化
    Player=new QMediaPlayer;
    Player->setPlaylist(nullptr);
    fileList=getFileNames(this->MusicPath);         //将 默认列表 的歌曲名存入fileList
    qDebug()<<"All Music File:\n"<<fileList;        //调试用 显示filelist中储存的所有歌曲名
    for(int i=0;i<fileList.size();i++)              //输出所有默认位置音频文件到界面
    {
        QString fileName=fileList.at(i);
        SongsAddress<<MusicPath+"\\"+fileName;      //将 默认列表 的歌曲地址存入SongsAddress
        addItem(fileList.at(i));
        PlayerList->addMedia(QUrl::fromLocalFile(MusicPath+"\\"+fileName));  //将 默认列表 的歌曲地址存入 播放列表 中
    }
    PlayerList->setCurrentIndex(0);
    Player->setPlaylist(PlayerList);                //将播放列表加载到播放器

    //收藏歌单初始化
    FavorItem();

    //进度条部分
    connect(ui->seekSlider,SIGNAL(sliderMoved(int)),this,SLOT(setPlayerPosition(int)));//进度条改变时播放位置随着改变
    connect(Player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChanged(qint64)));
    connect(Player,SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
    //显示当前歌曲名
    connect(Player,&QMediaPlayer::currentMediaChanged,this,&MainWindow::SlotShowCurrentMedia);

    //设置图标部分
        QPixmap *fav = new QPixmap(100,100);//收藏图标
        fav->load(Iconpath+"\\fav.png");
        QIcon *i_fav = new QIcon(*fav);
        ui->ButtonFavor->setIcon(*i_fav);    //删除图标
        QPixmap *del = new QPixmap(100,100);
        del->load(Iconpath+"\\del.png");
        QIcon *i_del = new QIcon(*del);
        ui->ButtonRemove->setIcon(*i_del);    //添加图标
        QPixmap *add = new QPixmap(100,100);
        add->load(Iconpath+"\\add.png");
        QIcon *i_add = new QIcon(*add);
        ui->ButtonAdd->setIcon(*i_add);

        //默认播放模式图标
        QPixmap *loop = new QPixmap(100,100);
        loop->load(Iconpath+"\\loop.png");
        QIcon *i_loop = new QIcon(*loop);
        ui->ButtonMode->setIcon(*i_loop);

       // setStyleSheet("QPushButton{background-color: rgba(18, 62, 111, 200);color:rgb(230, 230, 230);}");
}



//点击默认歌单歌曲时
void MainWindow::ItemClicked()
{
    isFavor=false;                                                   //!!!!引入布尔值isFavor判断选中的是哪个歌单的歌曲
    ClickedSong=ui->tableWidget->selectedItems()[0]->text();         //获取选中行的内容（即歌名）
    int rownum=ui->tableWidget->currentRow();                        //（行数-1）
    ClickedAddress=SongsAddress.at(rownum);                          //获取选中歌的地址
    qDebug()<<"void MainWindow::ItemClicked();"<<endl
           <<"获取了ClickedSong（点击的歌名）："<<ClickedSong<<endl
          <<"获取了ClickedAddress(点击的地址):"<<ClickedAddress<<endl
         <<"目标是否位于收藏歌单？"<<isFavor<<endl;                     //调试用
}

//点击收藏歌单歌曲时                    和上面这个不能合并，因为打算用isFavor判断点击信号来源
void MainWindow::ItemClicked_2()
{
    isFavor=true;                                                  //!!!!引入布尔值isFavor判断选中的是哪个歌单的歌曲
    ClickedSong=ui->tableWidget_2->selectedItems()[0]->text();     //获取选中行的内容（即歌名）
    int rownum_2=ui->tableWidget_2->currentRow();                  //（行数-1）
    ClickedAddress=FavorAddress.at(rownum_2);                      //获取选中歌的地址
    qDebug()<<"void MainWindow::ItemClicked_2();"<<endl
           <<"获取了ClickedSong（点击的歌名）："<<ClickedSong<<endl
          <<"获取了ClickedAddress(点击的地址):"<<ClickedAddress<<endl
         <<"目标是否位于收藏歌单？"<<isFavor<<endl;                     //调试用
}

//获取后台播放列表
QStringList MainWindow::getFileNames(const QString &path)
{
    QDir dir(path);
    QStringList nameFilters;
    nameFilters << "*.mp3";
    QStringList files = dir.entryList(nameFilters, QDir::Files|QDir::Readable, QDir::Name);       //获得mp3文件的名字
    return files;
}

//默认歌单显示
void MainWindow::addItem(QString name)
{
    int count=ui->tableWidget->rowCount();                  //获取行数
    ui->tableWidget->setRowCount(count+1);                  //新建一行
    QTableWidgetItem *itemName=new QTableWidgetItem(name);  //新建一个item用来存放新行的内容
    ui->tableWidget->setItem(count,0,itemName);             //将内容存入item（显化）
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);  //禁止编辑单元格
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); //自适应列宽
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch); //自适应行高
}

//收藏歌单显示
void MainWindow::FavorItem()
{
    ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers); //禁止编辑单元格
    ui->tableWidget_2->horizontalHeader()->setStretchLastSection(true);    //获取标题所在列，并使列占用所有剩余空间
    //将收藏歌单的歌曲名和对应地址储存在 ./favor.txt
    QFile file(FilePath);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
        return;
    }
    else
    {
        int i=1;                                                  //i作为txt中定位行的变量（txt种奇数行存放歌名，偶数行存放地址；）
        while(!file.atEnd())
        {
            if(i%2)
            {                                                     //实现隔行取名
                QString line=file.readLine();
                QStringList list=line.split("\n");
                FavorList<<list.first();                                         //存入list中，更方便使用
                int row=ui->tableWidget_2->rowCount();                           //行数
                ui->tableWidget_2->insertRow(row);                               //插入行
                ui->tableWidget_2->setItem(row, 0, new QTableWidgetItem(FavorList.last()));   //显示
            }
            else
            {
                QString line=file.readLine();
                QStringList list=line.split("\n");
                FavorAddress<<list.first();                           //把地址也存到list里，方便使用
            }
            i++;
        }
        for(int k=0;k<FavorAddress.size();k++)                                                     //存入收藏歌单地址列表
            favorPlayerList->addMedia(QUrl::fromLocalFile(FavorAddress.at(k)));
        qDebug()<<"存入FavorList的歌名如下："<<FavorList<<endl<<"存入FavorAddress的地址如下"<<FavorAddress<<endl;         //测试用
    }
    file.close();
}

//从列表移除歌曲
void MainWindow::DelMusicFilesFromList()
{
        int nSelectedRow = ui->tableWidget->currentRow();
        ui->tableWidget->removeRow(nSelectedRow);
        SongsAddress.removeAt(nSelectedRow);
        fileList.removeAt(nSelectedRow);
        PlayerList->removeMedia(nSelectedRow);
}

//右键-清空默认列表
void MainWindow::ClearMusicList()
{
    for (int i=ui->tableWidget->rowCount()-1; i>=0; i--){
        ui->tableWidget->removeRow(i);                            //0歌单界面逐条移除
        fileList.removeAt(i);                                     //1默认歌单 歌名列表 逐条移除
        SongsAddress.removeAt(i);                                 //2默认歌单 歌曲地址 逐条移除
    }
    PlayerList=nullptr;
    qDebug()<<"void MainWindow::ClearMusicList();"<<endl
           <<"清空播放列表。包括playlist、filelist、songsaddress"<<endl;                //调试用

    isPlay=false;                                                                   //播放状态、图标修正
    isFavor=true;                                                                   //切换成默认另一个歌单以备播放
    if(Player->playlist()!=favorPlayerList){
        qDebug()<<"之前播的列表不是收藏列表，已自动切换播放列表以备播放按钮调用。";
        ui->ButtonStop->setIcon(QIcon(":/play.ico"));
        if(favorPlayerList!=nullptr)
        Player->setPlaylist(favorPlayerList);
        else
        Player->stop();
    }
}
//右键-清空收藏列表
void  MainWindow::ClearMusicList_2(){
    for (int i=ui->tableWidget_2->rowCount()-1; i>=0; i--){
        ui->tableWidget_2->removeRow(i);                          //0 歌单界面 逐条移除
        FavorList.removeAt(i);                                    //1 收藏歌单 歌名列表 逐条移除
        FavorAddress.removeAt(i);                                 //2 收藏歌单 歌曲地址 逐条移除
    }
    favorPlayerList=nullptr;
    qDebug()<<"void MainWindow::ClearMusicList();"<<endl
           <<"清空播放列表。包括favorplaylist、favorlist、favoraddress"<<endl;       //调试用

    QFile file(FilePath);
    if(!file.open(QFile::WriteOnly | QIODevice::Truncate)){                         //4通过重写favor.txt的做法清空收藏夹文件
        qDebug()<<"【出错了】文件favor.txt打不开，清空文件失败！"<<endl;                  //调试用
        return;
    }else qDebug()<<"成功打开favor.txt，清空了内容，可以去看看~";                        //调试用

    isPlay=true;                                                                  //播放状态、图标修正
    isFavor=false;                                                                 //lzx切换成默认另一个歌单以备播放
    if(Player->playlist()!=PlayerList){
        qDebug()<<"之前播的列表不是默认列表，已自动切换播放列表以备播放按钮调用。";
        ui->ButtonStop->setIcon(QIcon(":/play.ico"));
        if(PlayerList!=nullptr)
        Player->setPlaylist(PlayerList);
        else
        Player->stop();
    }
}

//控件-播放
void MainWindow::SlotButtonStart()
{
    if (!isPlay){
        isPlay=true;
        ui->ButtonStop->setIcon(QIcon(":/stop.ico"));
        Player->play();
        if(isFavor)
            ui->tableWidget_2->selectRow(favorPlayerList->currentIndex());                //歌单追踪显示正在播放【数据来源：播放列表favorPlayList】
        else
            ui->tableWidget->selectRow(PlayerList->currentIndex());                       //歌单追踪显示正在播放【数据来源：播放列表Playlist】
        qDebug()<<"void MainWindow::SlotButtonStart();"<<"停止播放"<<endl;
    }else{
        isPlay=false;
        ui->ButtonStop->setIcon(QIcon(":/play.ico"));
        Player->pause();
        qDebug()<<"void MainWindow::SlotButtonStart();"<<"开始播放"<<endl;
    }
}

//控件-上一首
void MainWindow::SlotButtonPrevious(){
    ui->ButtonPrevious->setIcon(QIcon(":/previous.ico"));
    if(isFavor){                                                                      //!!!!使用了isFavor判断目标是哪个歌单的
        int i=favorPlayerList->mediaCount();
        if(favorPlayerList->currentIndex()==0)                                         //判断是否位于第一首
            favorPlayerList->setCurrentIndex(i-1);
        else{
            favorPlayerList->previous();
        }
        ui->tableWidget_2->selectRow(favorPlayerList->currentIndex());                 //0歌单追踪显示正在播放【数据来源：播放列表playlist】
        ClickedSong=FavorList.at(favorPlayerList->currentIndex());                     //更新ClickedSong【数据来源：FavorList和playlist】
        ClickedAddress=FavorAddress.at(favorPlayerList->currentIndex());               //更新ClickedAddress【数据来源：FavorAddress和playlist】
        qDebug()<<"void MainWindow::SlotButtonPrevious();"<<endl
               <<"当前选中的歌曲是："<<ClickedSong
              <<"数据来源：FavorList和favorPlayerList"<<endl
             <<"对应地址是："<<ClickedAddress
            <<"数据来源：FavorAddress和favorPlayerList"<<endl;                            //调试用
        }else{
            int i=PlayerList->mediaCount();
            if(PlayerList->currentIndex()==0)                                           //判断是否位于第一首
                PlayerList->setCurrentIndex(i-1);
            else{
                PlayerList->previous();
                //PlayerList->previousIndex();                                          //获取上一个序号，这里没用，故而注释了
            }
            ui->tableWidget->selectRow(PlayerList->currentIndex());                     //歌单追踪显示正在播放【数据来源：播放列表PlayerList】
            ClickedSong=fileList.at(PlayerList->currentIndex());                        //更新ClickedSong【数据来源：fileList和PlayerList】
            ClickedAddress=SongsAddress.at(PlayerList->currentIndex());                 //更新ClickedAddress【数据来源：SongsAddress和PlayerList】
            qDebug()<<"void MainWindow::SlotButtonPrevious();"<<endl
                   <<"当前选中的歌曲是："<<ClickedSong
                  <<"数据来源：FavorList和favorPlayerList"<<endl
                 <<"对应地址是："<<ClickedAddress
                <<"数据来源：FavorAddress和favorPlayerList"<<endl;                        //调试用
        }
}
//控件-下一首
void MainWindow::SlotButtonNext(){
    ui->ButtonNext->setIcon(QIcon(":/next.ico"));
    if(isFavor){
        int i=favorPlayerList->mediaCount();
        if(favorPlayerList->currentIndex()==i-1)
            favorPlayerList->setCurrentIndex(0);
        else{
            favorPlayerList->next();
            //favorPlayerList->nextIndex();
        }
        ui->tableWidget_2->selectRow(favorPlayerList->currentIndex());                 //歌单追踪显示正在播放【数据来源：播放列表playlist】
        ClickedSong=FavorList.at(favorPlayerList->currentIndex());                     //更新ClickedSong【数据来源：FavorList和playlist】
        ClickedAddress=FavorAddress.at(favorPlayerList->currentIndex());               //更新ClickedAddress【数据来源：FavorAddress和playlist】
        qDebug()<<"void MainWindow::SlotButtonNext();"<<endl
               <<"当前选中的歌曲是："<<ClickedSong
              <<"数据来源：FavorList和favorPlayerList"<<endl
             <<"对应地址是："<<ClickedAddress
            <<"数据来源：FavorAddress和favorPlayerList"<<endl;                           //调试用
    }else{
        int i=PlayerList->mediaCount();
        if(PlayerList->currentIndex()==i-1)                                            //判断是否位于最后一首
            PlayerList->setCurrentIndex(0);
        else{
            PlayerList->next();
        }
        ui->tableWidget->selectRow(PlayerList->currentIndex());                        //歌单追踪显示正在播放【数据来源：播放列表PlayerList】
        ClickedSong=fileList.at(PlayerList->currentIndex());                           //更新ClickedSong【数据来源：fileList和PlayerList】
        ClickedAddress=SongsAddress.at(PlayerList->currentIndex());                    //更新ClickedAddress【数据来源：SongsAddress和PlayerList】
        qDebug()<<"void MainWindow::SlotButtonNext();"<<endl
               <<"当前选中的歌曲是："<<ClickedSong
              <<"数据来源：FavorList和favorPlayerList"<<endl
             <<"对应地址是："<<ClickedAddress
            <<"数据来源：FavorAddress和favorPlayerList"<<endl;                           //调试用
     }
}


//显示当前曲名
void MainWindow::SlotShowCurrentMedia()
{
    if(isFavor){
        int index_2=favorPlayerList->currentIndex();
        CurrentSong=FavorList.at(index_2);                                             //获得当前播放歌曲的名字
        ui->label->setText(CurrentSong);
        qDebug()<<"void MainWindow::SlotShowCurrentMedia();"<<endl
               <<"当前播放的是播放列表的第"<<index_2<<"首（0开始）。"<<endl
              <<"数据来源：favorPlayerList"<<endl
              <<"CurrentSong："<<CurrentSong<<endl
             <<"数据来源：FavorList"<<endl;                                              //调试用
    }else{
        int index=PlayerList->currentIndex();
        CurrentSong=fileList.at(index);                                                //获得当前歌曲的名字
        ui->label->setText(CurrentSong);
        qDebug()<<"void MainWindow::SlotShowCurrentMedia();"<<endl
               <<"当前播放的是播放列表的第"<<index<<"首（0开始）。"<<endl
              <<"数据来源：PlayerList"<<endl
             <<"CurrentSong："<<CurrentSong<<endl
            <<"数据来源：fileList"<<endl;                                                //调试用
    }
}

//双击播放歌曲（默认歌单）
void MainWindow::playto(int i,int )
{
    Player->setPlaylist(PlayerList);
    PlayerList->setCurrentIndex(i);
    isPlay=true;
    ui->ButtonStop->setIcon(QIcon(":/stop.ico"));
    ClickedSong=fileList.at(i);                                                   //ClickedSong、ClickedAddress更新
    ClickedAddress=SongsAddress.at(i);
    Player->play();
    qDebug()<<"void MainWindow::playto(int i,int );"<<endl
         <<"双击，开始播放。"<<endl
        <<"ClickedSong【数据来源fileList】"<<ClickedSong<<endl
       <<"ClickedAddress【数据来源SongsAddress】"<<ClickedAddress<<endl;            //调试用
}

//双击播放歌曲（收藏歌单）
void MainWindow::playto2(int x,int)
{
    Player->setPlaylist(favorPlayerList);
    favorPlayerList->setCurrentIndex(x);
    isPlay=true;
    ui->ButtonStop->setIcon(QIcon(":/stop.ico"));
    ClickedSong=FavorList.at(x);                                                   //ClickedSong、ClickedAddress更新
    ClickedAddress=FavorAddress.at(x);
    Player->play();
    qDebug()<<"void MainWindow::playto_2(int x,int );"<<endl
         <<"双击，开始播放。"<<endl
        <<"ClickedSong【数据来源FavorList】"<<ClickedSong<<endl
       <<"ClickedAddress【数据来源FavorAddress】"<<ClickedAddress<<endl;            //调试用
}

//控件-播放模式
void MainWindow::SlotButtonMode()
{
    if(n==QMediaPlaylist::Loop) //单曲循环
{
    ui->ButtonMode->setText("单曲循环");        //单曲循环图标
    QPixmap *single = new QPixmap(100,100);
    single->load(Iconpath+"\\single.png");
    QIcon *i_single = new QIcon(*single);
    ui->ButtonMode->setIcon(*i_single);
    n=QMediaPlaylist::CurrentItemInLoop;
    PlayerList->setPlaybackMode(n);
    favorPlayerList->setPlaybackMode(n);
    }else if(n==QMediaPlaylist::CurrentItemInLoop)      //随机播放
{
    ui->ButtonMode->setText("随机播放");        //随机播放图标
    QPixmap *random = new QPixmap(100,100);
    random->load(Iconpath+"\\ran.png");
    QIcon *i_random = new QIcon(*random);
    ui->ButtonMode->setIcon(*i_random);
    n=QMediaPlaylist::Random;
    PlayerList->setPlaybackMode(n);
    favorPlayerList->setPlaybackMode(n);
    }else if(n==QMediaPlaylist::Random)                 //列表循环
{
    ui->ButtonMode->setText("列表循环");        //列表循环图标
    QPixmap *loop = new QPixmap(100,100);
    loop->load(Iconpath+"\\loop.png");
    QIcon *i_loop = new QIcon(*loop);
    ui->ButtonMode->setIcon(*i_loop);
    n=QMediaPlaylist::Loop;
    PlayerList->setPlaybackMode(n);
    favorPlayerList->setPlaybackMode(n);
}

}

//进度条 进度更新
void MainWindow::setPlayerPosition(int position)
{
    Player->setPosition(position);
}/* 当播放歌曲位置改变时  设置对应的位置 对应的播放时间与总时间的显示*/

//时间变化设置
void MainWindow::positionChanged(qint64 position)
{
    ui->seekSlider->setValue(position);//position 单位：毫秒 1000ms=1s
    currentTime=new QTime(0,(position/60000)%60,(position/1000)%60);//构造当前时间（小时,分钟,秒和毫秒）。
    ui->strTime->setText(currentTime->toString("mm:ss"));           //播放时间（输出时间格式eg.09:02）
}

//总时间设置
 void MainWindow::durationChanged(qint64 duration)
 {
     ui->seekSlider->setRange(0,duration);                          //范围从0到duration（总时间）
     totalTime=new QTime(0,(duration/60000)%60,(duration/1000)%60);
     ui->endTime->setText(totalTime->toString("mm:ss"));            //歌曲总时间
 }

//按钮-添加歌曲                                                                                                                       //这里有bug
void MainWindow::on_ButtonAdd_clicked()
{
    if(PlayerList==nullptr) PlayerList = new QMediaPlaylist;                        //清空列表后需重新生成播放列表
    QString initialName=QDir::homePath();                                           //返回用户目录
    QStringList pathList=QFileDialog::getOpenFileNames(this, tr("选择文件"), initialName, tr("MP3 Files (*.mp3)"));
                                                                                    //存放音乐文件的地址，可多选音乐。
    for(int i=0; i<pathList.size(); ++i){
        QString path=QDir::toNativeSeparators(pathList.at(i));                      //把斜杠转换为反斜杠
        if(!path.isEmpty()){                                                        //QUrl::fromLocalFile()方法打开文件中英文路径的文件.
            PlayerList->addMedia(QUrl::fromLocalFile(path));                        //3添加歌曲进入播放列表
            QString fileName=path.split("\\").last();                               //用split获取文件名(包含后缀)
            fileList<<fileName;                                                     //1添加歌名进fileList
            SongsAddress<<path;                                                     //2添加歌曲地址进SongsAddress
            int row=ui->tableWidget->rowCount();                                    //行数
            ui->tableWidget->insertRow(row);                                        //插入空行
            ui->tableWidget->setItem(row, 0, new QTableWidgetItem(fileName));       //0默认列表界面添加歌曲名
            ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
            qDebug()<<"void MainWindow::on_ButtonAdd_clicked();"
                   <<"添加了一首歌曲进入默认歌单"<<endl
                  <<"歌曲名(fileList最后一个）："<<fileList.last()<<endl
                 <<"地址为"<<path<<endl;                                              //调试用
        }
    }
    Player->setPlaylist(PlayerList);
    isPlay=false;
    ui->ButtonStop->setIcon(QIcon(":/play.ico"));

}

//按钮-收藏
void MainWindow::on_ButtonFavor_clicked()
{
    if(favorPlayerList==nullptr) favorPlayerList= new QMediaPlaylist;
    if(FavorList.indexOf(ClickedSong)==-1){
        if(fileList.indexOf(ClickedSong)!=-1){                                                //判断默认歌单里有无这首歌
            int row=ui->tableWidget_2->rowCount();
            ui->tableWidget_2->insertRow(row);
            ui->tableWidget_2->setItem(row, 0, new QTableWidgetItem(ClickedSong));            //0界面添加曲名
            FavorList<<ClickedSong;                                                           //1FavorList新增的的歌名【来源ClickedSong】
            FavorAddress<<ClickedAddress;                                                     //2FavorAddress新增地址【来源ClickedAddress】
            favorPlayerList->addMedia(QUrl::fromLocalFile(ClickedAddress));                   //3添加歌曲至播放列表
            qDebug()<<"void MainWindow::on_ButtonFavor_clicked();"<<endl
                   <<"加入收藏的歌名【来源ClickedSong】："<<ClickedSong<<endl
                  <<"对应地址【来源ClickedAddress】："<<ClickedAddress;                            //调试用

            QFile file(FilePath);
            file.open(QIODevice::WriteOnly | QIODevice::Append);
            file.write(ClickedSong.toUtf8()+"\n");                                              //4写入歌名
            file.write(ClickedAddress.toUtf8()+"\n");                                           //4写入地址
            file.close();
            qDebug()<<"favor.txt中写入了【以下格式即文件中格式】"<<endl
                   <<ClickedSong.toUtf8()+"\n"<<ClickedAddress.toUtf8()+"\n"<<endl;     //调试用                                                      //调试用
        }else                                                                                   //如果默认列表没有，这变相表明我选中了收藏列表里的歌~~
            QMessageBox::warning(this, "注意", "从收藏夹添加收藏没有意义啦~");
    }else
        QMessageBox::warning(this, "注意", "收藏夹里面已经有啦~");
}

//按钮-移除歌曲
void MainWindow::on_ButtonRemove_clicked()
{
    if(isFavor){                                                                                             //!!!!利用单击反馈的isFavor判断
        int SelectedRow_2 = ui->tableWidget_2->currentRow();                                                 //界面中获得选中的(行数-1)
        qDebug()<<"准备移除收藏列表第"<<SelectedRow_2<<"首歌，tablewidget上显示名叫"
               <<ui->tableWidget_2->item(SelectedRow_2,0)->text()<<" 存放在qstringlist对应位置叫"
              <<FavorList.at(SelectedRow_2)<<"对应地址为"<<FavorAddress.at(SelectedRow_2)<<endl;               //调试用
        ui->tableWidget_2->removeRow(SelectedRow_2);                                                         //0在界面中删除这首歌
        FavorList.removeAt(SelectedRow_2);                                                                   //1在 收藏歌单 歌名列表 里也删除这首歌歌名
        FavorAddress.removeAt(SelectedRow_2);                                                                //2在 收藏歌单 地址列表 里删除这首歌地址
        favorPlayerList->removeMedia(SelectedRow_2);                                                         //3在播放列表里面删除这首歌
        qDebug()<<"void MainWindow::on_ButtonRemove_clicked();"<<endl
               <<"FavorList、FavorAddress、favorPlayerList相应内容已移除！"<<endl
              <<"如今的favorlist为"<<FavorList<<endl
             <<"如今的FavorADdress为"<<FavorAddress;                                                     //测试用

        QFile file(FilePath);
        if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){                                              //4通过重写favor.txt的做法更新收藏夹，达到删除歌名地址目的
            qDebug()<<"【出错了】移除歌曲后想要重写favor.txt，但是打不开！！！"<<endl;                                //调试用
            return;
        }else{
            for(int i=0;i<=FavorList.indexOf(FavorList.last());i++){                                          //这里计算最后一项的下标，可能绕弯子了
            file.write(FavorList.at(i).toUtf8()+"\n");                                                        //利用list重新写入，串尾添加换行符
            file.write(FavorAddress.at(i).toUtf8()+"\n");                                                     //利用list重新写入，串尾添加换行符
            }
            qDebug()<<"成功打开favor.txt。用两个FavorList和FavorAddress重新写了一遍txt，可以去看看~";                  //调试用
        }

    }else{
        int SelectedRow = ui->tableWidget->currentRow();                                                      //界面中获得选中的(行数-1)
        qDebug()<<"准备移除默认列表第"<<SelectedRow<<"首歌，tablewidget上显示名叫"
               <<ui->tableWidget->item(SelectedRow,0)->text()<<" 存放在qstringlist对应位置叫"
              <<fileList.at(SelectedRow)<<"对应地址为"<<SongsAddress.at(SelectedRow)<<endl;                    //调试用
        ui->tableWidget->removeRow(SelectedRow);                                                             //0在界面中删除这首歌
        fileList.removeAt(SelectedRow);                                                                      //1在 默认歌单 歌名列表 里也删除这首歌歌名
        SongsAddress.removeAt(SelectedRow);                                                                  //2在 默认歌单 歌曲地址 列表删除这首歌地址
        PlayerList->removeMedia(SelectedRow);
        qDebug()<<"void MainWindow::on_ButtonRemove_clicked();"<<endl
              <<"fileList、SongsAddress、PlayerList移除！"<<endl
             <<"如今的fileList内为"<<fileList<<endl
            <<"如今SongsList内为"<<SongsAddress<<endl;                                                     //调试用
    }
}

//右键菜单（识别收藏歌单）
void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QPoint i = ui->tableWidget_2->mapFromGlobal(event->globalPos());             //把鼠标位置转化成相对位置，然后判断是否在tableWigdet_2内
    QRect rect(QPoint(0,0),ui->tableWidget_2->size());
    if (rect.contains(i)) contextMenu_2->exec(event->globalPos());
}


//搜索功能（默认歌单）
void MainWindow::Search()      //右键调用语句在本页顶部
{
    Dialog *dlg = new Dialog(this);  //新建窗口对象，确定父子关系
    dlg->setModal(true);             //非模态默认值为true,必须先处理该窗口
    //qDebug()<<"OK!"<<isFavor;      //想借用isFavor识别窗体整合相似函数的[待优化-lzx]
    dlg->show();
    connect(dlg, SIGNAL(sendData(QString)), this, SLOT(receiveData(QString)));
}
void MainWindow::receiveData(QString data)
{
    qDebug()<<fileList;
    int rowindex=fileList.indexOf(data);
    if(rowindex==-1) QMessageBox::warning(this,"抱歉","未找到该歌曲\n请检查文件名是否完整且包含后缀！");
    qDebug()<<rowindex;
    ui->tableWidget->selectRow(rowindex);     //在默认歌单高亮查找到的歌曲
}

//搜索功能（收藏歌单）
void MainWindow::Search_2()
{
    Dialog *dlg = new Dialog(this);  //新建窗口对象，确定父子关系
    dlg->setModal(true);             //非模态默认值为true,必须先处理该窗口
    dlg->show();
    connect(dlg, SIGNAL(sendData(QString)), this, SLOT(receiveData_2(QString)));  //信号源声明在dialog.h写在dialog.cpp
}
void MainWindow::receiveData_2(QString data)
{
    qDebug()<<FavorList;
    int rowindex=FavorList.indexOf(data);
    if(rowindex==-1) QMessageBox::warning(this,"抱歉","未找到该歌曲\n请检查文件名是否完整且包含后缀！");
    qDebug()<<rowindex;
    ui->tableWidget_2->selectRow(rowindex);    //在收藏歌单高亮查找到的歌曲
}
