#include "mainwindow.h"
#include "player.h"
#include "ui_mainwindow.h"
#include <QBoxLayout>
#include <QFileDialog>
#include <QToolButton>
#include <QLabel>
#include <QSlider>
#include <QMouseEvent>
#include <QTimer>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->m_pauseButton->setVisible(false);
    m_baseDir = QLatin1String(".");

    dirmodel = new QFileSystemModel(this);
    dirmodel->setRootPath(m_baseDir);
    ui->treeView->setModel(dirmodel);
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

    connect(ui->m_playButton, SIGNAL(clicked()), ui->m_player, SLOT(play()));
    connect(ui->m_pauseButton, SIGNAL(clicked()), ui->m_player, SLOT(pause()));
    connect(ui->m_stopButton, SIGNAL(clicked()), ui->m_player, SLOT(stop()));

    //this timer (re-)hides the controls after a few seconds when we are in fullscreen mode
    m_fullScreenTimer.setSingleShot(true);
    connect(&m_fullScreenTimer, SIGNAL(timeout()), this, SLOT(hideControls()));

    onStateChanged();
}

MainWindow::~MainWindow()
{
    delete ui;
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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open a Movie"), m_baseDir);

    if (!fileName.isEmpty()) {
        openFile(fileName);
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
        ui->gridLayout_2->setMargin(5);
        ui->gridLayout->setMargin(5);
        showControls();
        showNormal();
        ui->m_player->showNormal();
    } else {
        ui->tabWidget->setVisible(false);
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
    }
}

void MainWindow::showControls(bool show)
{
    ui->widget->setVisible(show);
    ui->m_positionSlider->setVisible(show);
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
//    m_volumeLabel->setEnabled(newState != QGst::StateNull);
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

//    m_positionLabel->setText(curpos.toString("hh:mm:ss.zzz")
//                                        + "/" +
//                             length.toString("hh:mm:ss.zzz"));

    if (length != QTime(0,0)) {
        ui->m_positionSlider->setValue(curpos.msecsTo(QTime(0,0)) * 1000 / length.msecsTo(QTime(0,0)));
    } else {
        ui->m_positionSlider->setValue(0);
    }

    if (curpos != QTime(0,0)) {
//        m_positionLabel->setEnabled(true);
        ui->m_positionSlider->setEnabled(true);
    }
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
