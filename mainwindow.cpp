#include "mainwindow.h"
#include "player.h"
#include "ui_mainwindow.h"
#include <QBoxLayout>
#include <QFileDialog>
#include <QToolButton>
#include <QLabel>
#include <QSlider>
#include <QMouseEvent>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //create the player
    //m_player = new Player(this);
    ui->setupUi(this);
    m_baseDir = QLatin1String(".");
}

MainWindow::~MainWindow()
{
    delete ui;
    //delete m_player;
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
        ui->m_player->setMouseTracking(false);
//        m_fullScreenTimer.stop();
//        showControls();
        showNormal();
    } else {
        setMouseTracking(true);
        ui->m_player->setMouseTracking(true);
//        hideControls();
        showFullScreen();
    }
}

void MainWindow::on_actionOpen_triggered()
{
    open();
}

void MainWindow::on_pushButton_clicked()
{

}

void MainWindow::on_fullscreenButton_clicked()
{
    toggleFullScreen();
}
