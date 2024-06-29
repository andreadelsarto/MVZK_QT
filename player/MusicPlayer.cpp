#include "MusicPlayer.h"
#include <algorithm>
#include <random>

MusicPlayer::MusicPlayer(QObject *parent) : QObject(parent), shuffleMode(false), repeatMode(false), currentIndex(0) {
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);

    connect(player, &QMediaPlayer::positionChanged, this, &MusicPlayer::positionChanged);
    connect(player, &QMediaPlayer::durationChanged, this, &MusicPlayer::durationChanged);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            playNext();
        }
    });
}

MusicPlayer::~MusicPlayer() {
    delete player;
    delete audioOutput;
}

void MusicPlayer::setMedia(const QUrl &url) {
    mediaList.clear();
    mediaList.append(url);
    currentIndex = 0;
    playCurrentMedia();
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

void MusicPlayer::setShuffle(bool shuffle) {
    shuffleMode = shuffle;
    if (shuffle) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(mediaList.begin(), mediaList.end(), g);
    }
}

void MusicPlayer::setRepeat(bool repeat) {
    repeatMode = repeat;
}

bool MusicPlayer::isShuffle() const {
    return shuffleMode;
}

bool MusicPlayer::isRepeat() const {
    return repeatMode;
}

void MusicPlayer::playNext() {
    if (shuffleMode) {
        currentIndex = std::rand() % mediaList.size();
    } else {
        currentIndex = (currentIndex + 1) % mediaList.size();
    }

    if (repeatMode && currentIndex == 0) {
        playCurrentMedia();
    } else if (!repeatMode && currentIndex == 0) {
        player->stop();
    } else {
        playCurrentMedia();
    }
}

void MusicPlayer::playPrevious() {
    if (shuffleMode) {
        currentIndex = std::rand() % mediaList.size();
    } else {
        currentIndex = (currentIndex - 1 + mediaList.size()) % mediaList.size();
    }
    playCurrentMedia();
}

void MusicPlayer::playCurrentMedia() {
    if (!mediaList.isEmpty()) {
        player->setSource(mediaList[currentIndex]);
        player->play();
    }
}
