#include "mainwindow.h"
#include "player.h"
#include "ui_mainwindow.h"
#include "playlistmodel.h"
#include <QBoxLayout>
#include <QFileDialog>
#include <QToolButton>
#include <QLabel>
#include <QSlider>
#include <QMouseEvent>
#include <QTimer>
#include <QStandardItem>
#include <QStringList>
#include <QStringListModel>
#include <QAbstractItemView>
#include <QDebug>
#include <stddef.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->m_pauseButton->setVisible(false);
    m_baseDir = QLatin1String("/home/lukasz/Music");

    fileSystemModel = new QFileSystemModel(this);
    fileSystemModel->setRootPath(m_baseDir);

    fileInfoList = new QFileInfoList();

    QStringList sDriveFilters;
    sDriveFilters << "*.aac" << "*.wmv" << "*.avi" << "*.mpeg" << "*.mov" << "*.3gp" << "*.flv" << "*.mp3" << "*.mp4" ;

    fileSystemModel->setNameFilters(sDriveFilters);
    fileSystemModel->setNameFilterDisables(false);

    ui->treeView->setModel(fileSystemModel);
    ui->treeView->setRootIndex(fileSystemModel->index(m_baseDir));

    connect(ui->treeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));

    ui->gridLayout_2->setMargin(5);
    ui->gridLayout->setMargin(5);

    ui->m_positionSlider->setTickPosition(QSlider::TicksBelow);
    ui->m_positionSlider->setTickInterval(10);
    ui->m_positionSlider->setMaximum(1000);

    ui->m_volumeSlider->setTickPosition(QSlider::TicksLeft);
    ui->m_volumeSlider->setTickInterval(2);
    ui->m_volumeSlider->setMaximum(10);
    ui->m_volumeSlider->setMaximumSize(64,32);

    connect(ui->m_positionSlider, SIGNAL(sliderMoved(int)), this, SLOT(setPosition(int)));
    connect(ui->m_player, SIGNAL(positionChanged()), this, SLOT(onPositionChanged()));
    connect(ui->m_player, SIGNAL(stateChanged()), this, SLOT(onStateChanged()));
    connect(ui->m_player, SIGNAL(metaChanged()), this, SLOT(onMetaChanged()));
    connect(ui->m_player, SIGNAL(volumeToggled()), this, SLOT(onVolumeToggled()));

    connect(ui->m_playButton, SIGNAL(clicked()), ui->m_player, SLOT(play()));
    connect(ui->m_pauseButton, SIGNAL(clicked()), ui->m_player, SLOT(pause()));
    connect(ui->m_stopButton, SIGNAL(clicked()), ui->m_player, SLOT(stop()));

    connect(ui->m_volumeSlider, SIGNAL(sliderMoved(int)), ui->m_player, SLOT(setVolume(int)));
    connect(ui->m_volumeButton, SIGNAL(clicked()), ui->m_player, SLOT(toggleVolume()));

    //this timer (re-)hides the controls after a few seconds when we are in fullscreen mode
    m_fullScreenTimer.setSingleShot(true);
    connect(&m_fullScreenTimer, SIGNAL(timeout()), this, SLOT(hideControls()));


    ui->m_playButton->setIcon(QIcon("../QtGstMediaPlayer/icons/play.png"));
    ui->m_playButton->setIconSize(QSize(32,32));

    ui->m_pauseButton->setIcon(QIcon("../QtGstMediaPlayer/icons/pause.png"));
    ui->m_pauseButton->setIconSize(QSize(32,32));

    ui->m_stopButton->setIcon(QIcon("../QtGstMediaPlayer/icons/stop.png"));
    ui->m_stopButton->setIconSize(QSize(32,32));

    ui->m_fullscreenButton->setIcon(QIcon("../QtGstMediaPlayer/icons/fullscreen.png"));

    ui->m_previousButton->setIcon(QIcon("../QtGstMediaPlayer/icons/previous.png"));
    ui->m_previousButton->setIconSize(QSize(32,32));

    ui->m_nextButton->setIcon(QIcon("../QtGstMediaPlayer/icons/next.png"));
    ui->m_nextButton->setIconSize(QSize(32,32));

    ui->m_volumeButton->setIcon(QIcon("../QtGstMediaPlayer/icons/audio_mid.png"));

    ui->m_increaseRateButton->setIcon(QIcon("../QtGstMediaPlayer/icons/increase.png"));


    ui->m_decreaseRateButton->setIcon(QIcon("../QtGstMediaPlayer/icons/decrease.png"));

    ui->m_addToPlayListButton->setIcon(QIcon("../QtGstMediaPlayer/icons/plus.png"));
//    ui->m_addToPlayListButton->setIconSize(QSize(32,32));

    ui->m_removeFromPlaylist->setIcon(QIcon("../QtGstMediaPlayer/icons/minus.png"));
//    ui->m_removeFromPlaylist->setIconSize(QSize(32,32));

    ui->m_moveUpInPlaylist->setIcon(QIcon("../QtGstMediaPlayer/icons/up.png"));
//    ui->m_moveUpInPlaylist->setIconSize(QSize(32,32));

    ui->m_moveDownInPlaylist->setIcon(QIcon("../QtGstMediaPlayer/icons/down.png"));
//    ui->m_moveDownInPlaylist->setIconSize(QSize(32,32));


    //TEMPORARY
    m_playListModel = new PlayListModel(this);
//    QStringList List;
//        List << "1.  Right Next Door To Hell"
//             << "2.  Dust 'N' Bones"
//             << "3.  Live And Let Die"
//             << "4.  Don't Cry"
//             << "5.  Perfect Crime"
//             << "6.  You Ain't The First"
//             << "7.  Bad Obsession"
//             << "8.  Back Off Bitch"
//             << "9.  Double Talkin' Jive"
//             << "10. November Rain"
//             << "11. The Garden"
//             << "12. Garden Of Eden"
//             << "13. Don't Damn Me"
//             << "14. Bad Apples"
//             << "15. Dead Horse"
//             << "16. Coma";
//    m_playListModel->setStringList(List);
    ui->m_playListView->setModel(m_playListModel);

    onStateChanged();

    setWindowTitle("QtGstMediaPlayer");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete fileSystemModel;
    delete fileInfoList;
}
void MainWindow::openFile(const QString & fileName)
{
    m_baseDir = QFileInfo(fileName).path();

    ui->m_player->stop();
    ui->m_player->setUri(fileName);
    ui->m_player->play();
}

void MainWindow::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select a file"), m_baseDir);

    if (!fileName.isEmpty()) {
        openFile(fileName);
        ui->m_filetypeLabel->setText(fileName.right(3).toUpper());
    }
}

void MainWindow::toggleFullScreen()
{
    if (isFullScreen()) {
        setMouseTracking(false);
        ui->centralWidget->setMouseTracking(false);
        ui->splitter->setMouseTracking(false);
        ui->widget_2->setMouseTracking(false);
        ui->m_player->setMouseTracking(false);
        m_fullScreenTimer.stop();

        ui->tabWidget->setVisible(true);
        ui->m_statusbar->setVisible(true);

        ui->gridLayout_2->setMargin(5);
        ui->gridLayout->setMargin(5);
        showControls();
        showNormal();
        ui->m_player->showNormal();
        ui->m_fullscreenButton->setIcon(QIcon("../QtGstMediaPlayer/icons/fullscreen.png"));
    } else {
        ui->tabWidget->setVisible(false);
        ui->m_statusbar->setVisible(false);

        setMouseTracking(true);
        ui->centralWidget->setMouseTracking(true);
        ui->splitter->setMouseTracking(true);
        ui->widget_2->setMouseTracking(true);
        ui->m_player->setMouseTracking(true);
        hideControls();

        ui->gridLayout_2->setContentsMargins(0,0,0,0);
        ui->gridLayout->setMargin(0);

        showFullScreen();
        ui->m_player->showFullScreen();
        ui->m_fullscreenButton->setIcon(QIcon("../QtGstMediaPlayer/icons/restore_fullscreen.png"));
    }
}

void MainWindow::showControls(bool show)
{
    ui->widget->setVisible(show);
    ui->widget_6->setVisible(show);
    if(show)
        ui->gridLayout_2->setContentsMargins(0,0,0,10);
    else
        ui->gridLayout_2->setContentsMargins(0,0,0,0);
}

void MainWindow::on_actionOpen_triggered()
{
    open();
}

void MainWindow::on_m_fullscreenButton_clicked()
{
    toggleFullScreen();
}

void MainWindow::onStateChanged()
{
    QGst::State newState = ui->m_player->state();
    ui->m_playButton->setEnabled(newState != QGst::StatePlaying);
    ui->m_playButton->setVisible(ui->m_playButton->isEnabled());
    ui->m_pauseButton->setEnabled(newState == QGst::StatePlaying);
    ui->m_pauseButton->setVisible(ui->m_pauseButton->isEnabled());
    ui->m_stopButton->setEnabled(newState != QGst::StateNull);
    ui->m_positionSlider->setEnabled(newState != QGst::StateNull);
    ui->m_volumeSlider->setEnabled(newState != QGst::StateNull);
    ui->m_volumeButton->setEnabled(newState != QGst::StateNull);
    ui->m_volumeSlider->setValue(ui->m_player->volume());

    //if we are in Null state, call onPositionChanged() to restore
    //the position of the slider and the text on the label
    if (newState == QGst::StateNull) {
        onPositionChanged();
    }
}

/* Called when the positionChanged() is received from the player */
void MainWindow::onPositionChanged()
{
    QTime length(0,0);
    QTime curpos(0,0);

    if (ui->m_player->state() != QGst::StateReady &&
        ui->m_player->state() != QGst::StateNull)
    {
        length = ui->m_player->length();
        curpos = ui->m_player->position();
    }

    ui->m_positionLabel->setText(curpos.toString("hh:mm:ss"));
    ui->m_fullLengthLabel->setText(length.toString("hh:mm:ss"));
    ui->m_lengthLabel->setText(length.toString("hh:mm:ss"));

    if (length != QTime(0,0)) {
        ui->m_positionSlider->setValue(curpos.msecsTo(QTime(0,0)) * 1000 / length.msecsTo(QTime(0,0)));
    } else {
        ui->m_positionSlider->setValue(0);
    }

    if (curpos != QTime(0,0)) {
        ui->m_positionLabel->setEnabled(true);
        ui->m_positionSlider->setEnabled(true);
    }
}

void MainWindow::onMetaChanged()
{
    QString lbl = ui->m_player->meta().value("brate");
    lbl.append(" kbps");
    if(lbl != NULL) {
        ui->m_bitrateLabel->setText(lbl);
    }
}

void MainWindow::onVolumeToggled()
{
    ui->m_volumeSlider->setValue(ui->m_player->volume());
}

/* Called when the user changes the slider's position */
void MainWindow::setPosition(int value)
{
    uint length = -ui->m_player->length().msecsTo(QTime(0,0));
    if (length != 0 && value > 0) {
        QTime pos(0,0);
        pos = pos.addMSecs(length * (value / 1000.0));
        ui->m_player->setPosition(pos);
    }
}


void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (isFullScreen()) {
        showControls();
        m_fullScreenTimer.start(3000); //re-hide controls after 3s
    }
}

void MainWindow::showContextMenu(const QPoint &point)
{
    QTreeView* view = ui->treeView;
    QModelIndex index = view->currentIndex();
    fileInfoList->clear();
    QFileInfo selected = fileSystemModel->fileInfo(index);

    getAllFilesUnderSelected(selected, *fileInfoList);

    QMenu *menu = new QMenu(view);
    menu->addAction(QString("Add to library"), this, SLOT(addMediaFilesToLibrary()));

    menu->exec(QCursor::pos());

}

void MainWindow::getAllFilesUnderSelected(QFileInfo selected, QFileInfoList & fList)
{
    QStringList filters;
    filters << "aac" << "wmv" << "avi" << "mpeg" << "mov" << "3gp" << "flv" << "mp3" << "mp4" ;

    if(selected.isFile())
        fList.append(selected);
    else {
        QDir root(selected.absoluteFilePath());
        QFileInfoList tmp = root.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst);
        for(int i = 0; i < tmp.size(); i++) {
            if(tmp.at(i).isDir())
                getAllFilesUnderSelected(tmp.at(i).absoluteFilePath(), fList);
            else {
                QString extension = tmp.at(i).suffix();
                if(filters.contains(extension))
                    fList.append(tmp.at(i));
            }
        }
    }
}

void MainWindow::addMediaFilesToLibrary()
{
    for(int i = 0; i < fileInfoList->size(); i++) {
//        qDebug() << fileInfoList->at(i).absoluteFilePath();
        m_playListModel->addElement(fileInfoList->at(i));
    }

}




