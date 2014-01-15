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
    m_baseDir = QLatin1String(".");

    dirmodel = new QFileSystemModel(this);
    dirmodel->setRootPath(m_baseDir);
    ui->treeView->setModel(dirmodel);
    ui->gridLayout_2->setMargin(5);
    ui->gridLayout->setMargin(5);

    //this timer (re-)hides the controls after a few seconds when we are in fullscreen mode
    m_fullScreenTimer.setSingleShot(true);
    connect(&m_fullScreenTimer, SIGNAL(timeout()), this, SLOT(hideControls()));
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
    if(show)
        ui->gridLayout_2->setContentsMargins(0,0,0,10);
    else
        ui->gridLayout_2->setContentsMargins(0,0,0,0);
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

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    if (isFullScreen()) {
        showControls();
        m_fullScreenTimer.start(3000); //re-hide controls after 3s
    }
}
