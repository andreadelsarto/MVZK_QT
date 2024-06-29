#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QSlider>
#include <QLineEdit>
#include <QDialog>
#include <QSettings>
#include <QPushButton>
#include <QMediaPlayer>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsBlurEffect>
#include "MusicPlayer.h"
#include "../widgets/MarqueeLabel.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void openFolder();
    void play();
    void pause();
    void prevSong();
    void nextSong();
    void playSelectedSong(QListWidgetItem *item);
    void openOptionsDialog();
    void updateSlider(qint64 position);
    void setPlayerPosition(int position);
    void loadSettings();
    void saveSettings();
    void updatePlayPauseButtons(QMediaPlayer::PlaybackState state);
    void searchSongs(const QString &searchText);
    void updateSliderRange(qint64 duration);
    void jumpToSliderPosition(QMouseEvent *event, QSlider *slider);
    void toggleShuffle();
    void toggleRepeat();

private:
    void setupUI();
    void updateLayout();
    void displayCoverArt(const QString &filePath);
    void resetSliderStyle();  // Aggiungi questa dichiarazione
    QString formatTime(qint64 milliseconds);

    MusicPlayer *musicPlayer;
    QList<QUrl> playlist;
    QListWidget *songListWidget;
    QSlider *volumeSlider;
    QSlider *progressSlider;
    QLineEdit *musicFolderPath;
    QLineEdit *searchBar;
    QPushButton *optionsButton;
    QPushButton *shuffleButton;
    QPushButton *repeatButton;
    QPushButton *playButton;
    QPushButton *pauseButton;
    QPushButton *prevButton;
    QPushButton *nextButton;
    QDialog *optionsDialog;
    QSettings *settings;
    QStringList allSongs;

    QLabel *titleLabel;  // Aggiungi questa dichiarazione
    MarqueeLabel *songTitleLabel;
    MarqueeLabel *songArtistLabel;
    QLabel *elapsedTimeLabel;
    QLabel *totalTimeLabel;
    QLabel *coverArtLabel;
    QVBoxLayout *playerLayout;
    QVBoxLayout *playlistLayout;
    QWidget *playerWidget;
    QWidget *playlistWidget;
    QHBoxLayout *mainLayout;

    bool isShuffleActive;  // Aggiungi questa variabile
    bool isRepeatActive;

    QColor dominantColor;
};

#endif // MAINWINDOW_H
