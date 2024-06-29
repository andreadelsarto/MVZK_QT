#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QListWidgetItem>

class MusicPlayer : public QObject {
    Q_OBJECT

public:
    MusicPlayer(QObject *parent = nullptr);
    ~MusicPlayer();

    void setMedia(const QUrl &url);
    void play();
    void pause();
    void stop();
    void setVolume(int volume);
    void setPosition(int position);
    QMediaPlayer* getPlayer() const;

signals:
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);

private:
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
};

#endif // MUSICPLAYER_H
