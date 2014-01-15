#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QWidget>
#include <QStyle>
#include <QFileSystemModel>

class Player;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_actionOpen_triggered();

    void on_pushButton_clicked();

    void on_fullscreenButton_clicked();
    void hideControls() {showControls(false);}

private:
    Ui::MainWindow *ui;
    QString m_baseDir;
    QFileSystemModel *dirmodel;
    QTimer m_fullScreenTimer;

    void open();
    void openFile(const QString & fileName);
    void toggleFullScreen();
    void showControls(bool show = true);
protected:
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H
