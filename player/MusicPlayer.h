#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QList>

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

    void setShuffle(bool shuffle);
    void setRepeat(bool repeat);
    bool isShuffle() const;
    bool isRepeat() const;

signals:
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);

private:
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    bool shuffleMode;
    bool repeatMode;
    QList<QUrl> mediaList;
    int currentIndex;

    void playNext();
    void playPrevious();
    void playCurrentMedia();
};

#endif // MUSICPLAYER_H
