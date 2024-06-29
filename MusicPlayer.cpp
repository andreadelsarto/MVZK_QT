#include "MusicPlayer.h"

MusicPlayer::MusicPlayer(QObject *parent) : QObject(parent) {
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);

    connect(player, &QMediaPlayer::positionChanged, this, &MusicPlayer::positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &MusicPlayer::durationChanged);
}

MusicPlayer::~MusicPlayer() {
    delete player;
    delete audioOutput;
}

void MusicPlayer::setMedia(const QUrl &url) {
    player->setSource(url);
}

void MusicPlayer::play() {
    player->play();
}

void MusicPlayer::pause() {
    player->pause();
}

void MusicPlayer::stop() {
    player->stop();
}

void MusicPlayer::setVolume(int volume) {
    audioOutput->setVolume(volume / 100.0);
}

void MusicPlayer::setPosition(int position) {
    player->setPosition(position);
}

QMediaPlayer* MusicPlayer::getPlayer() const {
    return player;
}
