#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QWidget>
#include <QStyle>
#include <QFileSystemModel>
#include "playlistmodel.h"

class Player;
class PlayListModel;

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

    void on_m_fullscreenButton_clicked();
    void hideControls() {showControls(false);}

    void onStateChanged();
    void onPositionChanged();

    void setPosition(int position);
    void onMetaChanged();
    void onVolumeToggled();
    void showContextMenu(const QPoint &);
    void addMediaFilesToLibrary();

private:
    Ui::MainWindow *ui;
    QString m_baseDir;
    QFileSystemModel *fileSystemModel;
    QFileInfoList *fileInfoList;
    QTimer m_fullScreenTimer;
        PlayListModel *m_playListModel;

    void open();
    void openFile(const QString & fileName);
    void toggleFullScreen();
    void showControls(bool show = true);
    void getAllFilesUnderSelected(QFileInfo selected, QFileInfoList & fList);
protected:
    void mouseMoveEvent(QMouseEvent *event);
};

#endif // MAINWINDOW_H
