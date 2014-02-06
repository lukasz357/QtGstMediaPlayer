#ifndef PLAYER_H
#define PLAYER_H
#include <QTimer>
#include <QTime>
#include <QMap>
#include <QGst/Pipeline>
#include <QGst/Ui/VideoWidget>

class Player : public QGst::Ui::VideoWidget
{
    Q_OBJECT
public:
    Player(QWidget *parent = 0);
    ~Player();

    void setUri(const QString & uri);

    QTime position() const;
    void setPosition(const QTime & pos);
    int volume() const;

    QTime length() const;
    QGst::State state() const;

public Q_SLOTS:
    void play();
    void pause();
    void stop();
    void setVolume(int volume);
    void toggleVolume();

    QMap<QString, QString> meta();

Q_SIGNALS:
    void positionChanged();
    void stateChanged();
    void gotBitrate(quint32);
    void metaChanged();
    void volumeToggled();

private:
    void onBusMessage(const QGst::MessagePtr & message);
    void handlePipelineStateChange(const QGst::StateChangedMessagePtr & scm);
    void handleTag(const QGst::TagMessagePtr &scm);

    QGst::PipelinePtr m_pipeline;
    QTimer m_positionTimer;
    QMap<QString, QString> m_Meta;
    int m_previousVolumeValue;
};

#endif // PLAYER_H
