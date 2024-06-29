#include "MainWindow.h"
#include "Search.h"
#include <QFileDialog>
#include <QDir>
#include <QIcon>
#include <QResizeEvent>
#include <taglib/fileref.h>
#include <taglib/tag.h>
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/apetag.h>
#include <taglib/attachedpictureframe.h>
#include <QPixmap>
#include <QBuffer>
#include <QColor>
#include <QImage>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QStyle>
#include <QMap>
#include <QPair>
#include <QHash>

// Definisci la funzione qHash per QColor
uint qHash(const QColor &color, uint seed = 0) {
    return qHash(qMakePair(color.red(), qMakePair(color.green(), color.blue())), seed);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), settings(new QSettings("YourCompany", "MusicPlayer", this)), isShuffleActive(false), isRepeatActive(false) {
    musicPlayer = new MusicPlayer(this);
    setupUI();

    // Impostare il volume iniziale al 100%
    musicPlayer->setVolume(100);
    volumeSlider->setValue(100);

    // Connect signals and slots
    connect(playButton, &QPushButton::clicked, this, &MainWindow::play);
    connect(pauseButton, &QPushButton::clicked, this, &MainWindow::pause);
    connect(prevButton, &QPushButton::clicked, this, &MainWindow::prevSong);
    connect(nextButton, &QPushButton::clicked, this, &MainWindow::nextSong);
    connect(progressSlider, &QSlider::sliderMoved, this, &MainWindow::setPlayerPosition);
    connect(musicPlayer, &MusicPlayer::positionChanged, this, &MainWindow::updateSlider);
    connect(musicPlayer, &MusicPlayer::durationChanged, this, &MainWindow::updateSliderRange);
    connect(volumeSlider, &QSlider::valueChanged, [=](int value) {
        musicPlayer->setVolume(value);
    });
    connect(musicPlayer->getPlayer(), &QMediaPlayer::playbackStateChanged, this, &MainWindow::updatePlayPauseButtons);
    connect(searchBar, &QLineEdit::textChanged, this, &MainWindow::searchSongs);
    connect(optionsButton, &QPushButton::clicked, this, &MainWindow::openOptionsDialog);
    connect(songListWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::playSelectedSong);

    // Connessioni dei segnali per i tasti shuffle e repeat
    connect(shuffleButton, &QPushButton::clicked, this, &MainWindow::toggleShuffle);
    connect(repeatButton, &QPushButton::clicked, this, &MainWindow::toggleRepeat);


    // Custom behavior for slider click
    progressSlider->installEventFilter(this);
    volumeSlider->installEventFilter(this);

    // Load settings
    loadSettings();

    // Initially show play button and hide pause button
    playButton->show();
    pauseButton->hide();
}



MainWindow::~MainWindow() {
    delete musicPlayer;
    delete songListWidget;
    delete volumeSlider;
    delete progressSlider;
    delete musicFolderPath;
    delete searchBar;
    delete optionsButton;
    delete playButton;
    delete pauseButton;
    delete prevButton;
    delete nextButton;
    delete shuffleButton;  // Aggiungi questa linea
    delete repeatButton;   // Aggiungi questa linea
    delete optionsDialog;
    delete settings;
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    mainLayout = new QHBoxLayout(centralWidget);

    // Player widget
    playerWidget = new QWidget(this);
    playerLayout = new QVBoxLayout(playerWidget);

    // Aggiungi la title label
    titleLabel = new QLabel("MVZK", this);
    titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    playerLayout->addWidget(titleLabel, 0, Qt::AlignCenter);

    // Cover art label
    coverArtLabel = new QLabel(this);
    coverArtLabel->setScaledContents(true);
    coverArtLabel->setFixedSize(300, 300);
    playerLayout->addWidget(coverArtLabel, 0, Qt::AlignCenter);

    // Aggiungi un QSpacerItem per spingere le etichette in basso
    playerLayout->addItem(new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding));

    // Song title and artist labels
    songTitleLabel = new MarqueeLabel(this);
    songArtistLabel = new MarqueeLabel(this);
    songTitleLabel->setStyleSheet("font-size: 18px; color: white;");
    songArtistLabel->setStyleSheet("font-size: 14px; color: white;");
    playerLayout->addWidget(songTitleLabel);
    playerLayout->addWidget(songArtistLabel);

    // Slider per il progresso della traccia
    progressSlider = new QSlider(Qt::Horizontal, this);
    progressSlider->setStyleSheet("QSlider::groove:horizontal { background: #333; height: 4px; }"
    "QSlider::handle:horizontal { background: #aaa; width: 14px; margin: -5px 0; border-radius: 7px; }"
    "QSlider::sub-page:horizontal { background: #5a5; }");
    playerLayout->addWidget(progressSlider);

    // Elapsed and total time labels
    QHBoxLayout *timeLayout = new QHBoxLayout();
    elapsedTimeLabel = new QLabel("00:00", this);
    totalTimeLabel = new QLabel("00:00", this);
    elapsedTimeLabel->setStyleSheet("color: white;");
    totalTimeLabel->setStyleSheet("color: white;");
    timeLayout->addWidget(elapsedTimeLabel);
    timeLayout->addStretch();
    timeLayout->addWidget(totalTimeLabel);

    // Aggiungi il layout del tempo dopo lo slider
    playerLayout->addLayout(timeLayout);

    QHBoxLayout *playbackLayout = new QHBoxLayout();

    shuffleButton = new QPushButton(QIcon::fromTheme("media-playlist-shuffle"), "", this);
    prevButton = new QPushButton(QIcon::fromTheme("media-skip-backward"), "", this);
    playButton = new QPushButton(QIcon::fromTheme("media-playback-start"), "", this);
    pauseButton = new QPushButton(QIcon::fromTheme("media-playback-pause"), "", this);
    nextButton = new QPushButton(QIcon::fromTheme("media-skip-forward"), "", this);
    repeatButton = new QPushButton(QIcon::fromTheme("media-playlist-repeat"), "", this);

    shuffleButton->setStyleSheet("background-color: #444; color: white; border: none; padding: 10px; border-radius: 10px;");
    prevButton->setStyleSheet("background-color: #444; color: white; border: none; padding: 10px; border-radius: 10px;");
    playButton->setStyleSheet("background-color: #444; color: white; border: none; padding: 10px; border-radius: 10px;");
    pauseButton->setStyleSheet("background-color: #444; color: white; border: none; padding: 10px; border-radius: 10px;");
    nextButton->setStyleSheet("background-color: #444; color: white; border: none; padding: 10px; border-radius: 10px;");
    repeatButton->setStyleSheet("background-color: #444; color: white; border: none; padding: 10px; border-radius: 10px;");

    playbackLayout->addWidget(shuffleButton);
    playbackLayout->addWidget(prevButton);
    playbackLayout->addWidget(playButton);
    playbackLayout->addWidget(pauseButton);
    playbackLayout->addWidget(nextButton);
    playbackLayout->addWidget(repeatButton);
    playerLayout->addLayout(playbackLayout);

    volumeSlider = new QSlider(Qt::Horizontal, this);
    volumeSlider->setStyleSheet("QSlider::groove:horizontal { background: #333; height: 4px; }"
    "QSlider::handle:horizontal { background: #aaa; width: 14px; margin: -5px 0; border-radius: 7px; }"
    "QSlider::sub-page:horizontal { background: #5a5; }");
    playerLayout->addWidget(volumeSlider);

    // Blocca la larghezza del player
    playerWidget->setFixedWidth(320);

    // Playlist widget
    playlistWidget = new QWidget(this);
    playlistLayout = new QVBoxLayout(playlistWidget);

    // Top bar
    QHBoxLayout *topBar = new QHBoxLayout();
    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("Search");
    searchBar->setStyleSheet("background-color: #fff; color: #333; padding: 10px; margin: 5px; border-radius: 10px; border: 1px solid #ccc;");
    optionsButton = new QPushButton("Options", this);
    topBar->addWidget(searchBar);
    topBar->addStretch();
    topBar->addWidget(optionsButton);
    playlistLayout->addLayout(topBar);

    // Playlist area
    songListWidget = new QListWidget(this);
    songListWidget->setStyleSheet("background-color: #333; color: white; border: none; padding: 10px;");
    playlistLayout->addWidget(songListWidget);

    mainLayout->addWidget(playerWidget);
    mainLayout->addWidget(playlistWidget);

    setCentralWidget(centralWidget);

    // Installare il filtro degli eventi sugli slider
    progressSlider->installEventFilter(this);
    volumeSlider->installEventFilter(this);

    // Connect signals and slots
    connect(playButton, &QPushButton::clicked, this, &MainWindow::play);
    connect(pauseButton, &QPushButton::clicked, this, &MainWindow::pause);
    connect(prevButton, &QPushButton::clicked, this, &MainWindow::prevSong);
    connect(nextButton, &QPushButton::clicked, this, &MainWindow::nextSong);
    connect(progressSlider, &QSlider::sliderMoved, this, &MainWindow::setPlayerPosition);
    connect(volumeSlider, &QSlider::valueChanged, [=](int value) {
        musicPlayer->setVolume(value);
    });
    connect(musicPlayer->getPlayer(), &QMediaPlayer::playbackStateChanged, this, &MainWindow::updatePlayPauseButtons);
    connect(searchBar, &QLineEdit::textChanged, this, &MainWindow::searchSongs);
    connect(optionsButton, &QPushButton::clicked, this, &MainWindow::openOptionsDialog);
    connect(songListWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::playSelectedSong);

    // Connect shuffle and repeat buttons
    connect(shuffleButton, &QPushButton::clicked, this, &MainWindow::toggleShuffle);
    connect(repeatButton, &QPushButton::clicked, this, &MainWindow::toggleRepeat);

    // Custom behavior for slider click
    progressSlider->installEventFilter(this);
    volumeSlider->installEventFilter(this);

    // Load settings
    loadSettings();

    // Initially show play button and hide pause button
    playButton->show();
    pauseButton->hide();
}




void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    updateLayout();
}

void MainWindow::updateLayout() {
    if (width() > height()) {
        // Landscape mode
        mainLayout->setDirection(QBoxLayout::LeftToRight);
    } else {
        // Portrait mode
        mainLayout->setDirection(QBoxLayout::TopToBottom);
    }
}

void MainWindow::play() {
    if (musicPlayer->getPlayer()->position() == musicPlayer->getPlayer()->duration()) {
        musicPlayer->setPosition(0);
    }
    musicPlayer->play();
    playButton->hide();
    pauseButton->show();
}

void MainWindow::pause() {
    musicPlayer->pause();
    playButton->show();
    pauseButton->hide();
}

void MainWindow::toggleShuffle() {
    isShuffleActive = !isShuffleActive;
    musicPlayer->setShuffle(isShuffleActive);
    shuffleButton->setStyleSheet(isShuffleActive ? QString("background-color: %1; color: white; border: none; padding: 10px; border-radius: 10px;").arg(dominantColor.name())
    : "background-color: #444; color: white; border: none; padding: 10px; border-radius: 10px;");
}

void MainWindow::toggleRepeat() {
    isRepeatActive = !isRepeatActive;
    musicPlayer->setRepeat(isRepeatActive);
    repeatButton->setStyleSheet(isRepeatActive ? QString("background-color: %1; color: white; border: none; padding: 10px; border-radius: 10px;").arg(dominantColor.name())
    : "background-color: #444; color: white; border: none; padding: 10px; border-radius: 10px;");
}

void MainWindow::prevSong() {
    int currentIndex = songListWidget->currentRow();
    if (currentIndex > 0) {
        songListWidget->setCurrentRow(currentIndex - 1);
        playSelectedSong(songListWidget->currentItem());
    }
}

void MainWindow::nextSong() {
    int currentIndex = songListWidget->currentRow();
    if (currentIndex < songListWidget->count() - 1) {
        songListWidget->setCurrentRow(currentIndex + 1);
        playSelectedSong(songListWidget->currentItem());
    }
}

void MainWindow::updatePlayPauseButtons(QMediaPlayer::PlaybackState state) {
    if (state == QMediaPlayer::PlayingState) {
        playButton->hide();
        pauseButton->show();
    } else {
        playButton->show();
        pauseButton->hide();
    }
}

void MainWindow::openFolder() {
    QString folderPath = QFileDialog::getExistingDirectory(this, "Select Music Folder");
    if (!folderPath.isEmpty()) {
        settings->setValue("musicFolder", folderPath);
        QDir directory(folderPath);
        QStringList musicFiles = directory.entryList(QStringList() << "*.mp3" << "*.wav" << "*.flac", QDir::Files);
        playlist.clear();
        songListWidget->clear();
        allSongs.clear();
        foreach (QString fileName, musicFiles) {
            QString filePath = directory.absoluteFilePath(fileName);
            playlist.append(QUrl::fromLocalFile(filePath));
            allSongs.append(fileName);

            // Leggi i tag ID3 usando TagLib
            TagLib::FileRef f(filePath.toUtf8().constData());
            QString displayName = fileName;
            if (!f.isNull() && f.tag()) {
                TagLib::Tag *tag = f.tag();
                QString title = tag->title().toCString(true);
                QString artist = tag->artist().toCString(true);
                if (!title.isEmpty()) {
                    displayName = title;
                }
                if (!artist.isEmpty()) {
                    displayName += " - " + artist;
                }
            }

            QListWidgetItem *item = new QListWidgetItem(displayName, songListWidget);
            item->setData(Qt::UserRole, filePath);
        }
    }
}

void MainWindow::playSelectedSong(QListWidgetItem *item) {
    QString filePath = item->data(Qt::UserRole).toString();
    musicPlayer->setMedia(QUrl::fromLocalFile(filePath));
    musicPlayer->play();

    // Leggi i tag ID3 usando TagLib
    TagLib::FileRef f(filePath.toUtf8().constData());
    if (!f.isNull() && f.tag()) {
        TagLib::Tag *tag = f.tag();
        QString title = tag->title().toCString(true);
        QString artist = tag->artist().toCString(true);
        if (!title.isEmpty()) {
            songTitleLabel->setText(title);
        } else {
            songTitleLabel->setText(item->text());
        }
        if (!artist.isEmpty()) {
            songArtistLabel->setText(artist);
        } else {
            songArtistLabel->setText("");
        }
        displayCoverArt(filePath);
    } else {
        songTitleLabel->setText(item->text());
        songArtistLabel->setText("");
    }
}

void MainWindow::updateSlider(qint64 position) {
    progressSlider->setValue(static_cast<int>(position));
    elapsedTimeLabel->setText(formatTime(position));
}

void MainWindow::updateSliderRange(qint64 duration) {
    progressSlider->setRange(0, static_cast<int>(duration));
    totalTimeLabel->setText(formatTime(duration));
}

void MainWindow::setPlayerPosition(int position) {
    musicPlayer->setPosition(position);
}

void MainWindow::openOptionsDialog() {
    optionsDialog = new QDialog(this);
    optionsDialog->setWindowTitle("Options");

    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsDialog);
    QLabel *musicFolderLabel = new QLabel("Music Folder:", optionsDialog);
    musicFolderPath = new QLineEdit(optionsDialog);
    musicFolderPath->setText(settings->value("musicFolder", "").toString());
    QPushButton *browseButton = new QPushButton("Browse", optionsDialog);

    connect(browseButton, &QPushButton::clicked, [this]() {
        QString folderPath = QFileDialog::getExistingDirectory(this, "Select Music Folder");
        if (!folderPath.isEmpty()) {
            musicFolderPath->setText(folderPath);
        }
    });

    QPushButton *okButton = new QPushButton("OK", optionsDialog);
    connect(okButton, &QPushButton::clicked, [this]() {
        settings->setValue("musicFolder", musicFolderPath->text());
        openFolder();
        optionsDialog->accept();
    });

    optionsLayout->addWidget(musicFolderLabel);
    optionsLayout->addWidget(musicFolderPath);
    optionsLayout->addWidget(browseButton);
    optionsLayout->addWidget(okButton);

    optionsDialog->setLayout(optionsLayout);
    optionsDialog->exec();
}

void MainWindow::loadSettings() {
    QString folderPath = settings->value("musicFolder", "").toString();
    if (!folderPath.isEmpty()) {
        QDir directory(folderPath);
        QStringList musicFiles = directory.entryList(QStringList() << "*.mp3" << "*.wav" << "*.flac", QDir::Files);
        playlist.clear();
        songListWidget->clear();
        allSongs.clear();
        foreach (QString fileName, musicFiles) {
            QString filePath = directory.absoluteFilePath(fileName);
            playlist.append(QUrl::fromLocalFile(filePath));
            allSongs.append(fileName);

            // Leggi i tag ID3 usando TagLib
            TagLib::FileRef f(filePath.toUtf8().constData());
            QString displayName = fileName;
            if (!f.isNull() && f.tag()) {
                TagLib::Tag *tag = f.tag();
                QString title = tag->title().toCString(true);
                QString artist = tag->artist().toCString(true);
                if (!title.isEmpty()) {
                    displayName = title;
                }
                if (!artist.isEmpty()) {
                    displayName += " - " + artist;
                }
            }

            QListWidgetItem *item = new QListWidgetItem(displayName, songListWidget);
            item->setData(Qt::UserRole, filePath);
        }
    }
}

void MainWindow::saveSettings() {
    settings->setValue("musicFolder", musicFolderPath->text());
}

void MainWindow::searchSongs(const QString &searchText) {
    ::searchSongs(searchText, allSongs, playlist, songListWidget);
}

void MainWindow::displayCoverArt(const QString &filePath) {
    TagLib::MPEG::File mpegFile(filePath.toUtf8().constData());
    TagLib::ID3v2::Tag *id3v2tag = mpegFile.ID3v2Tag();
    if (id3v2tag) {
        const TagLib::ID3v2::FrameList frames = id3v2tag->frameListMap()["APIC"];
        if (!frames.isEmpty()) {
            TagLib::ID3v2::AttachedPictureFrame *frame = static_cast<TagLib::ID3v2::AttachedPictureFrame *>(frames.front());
            QPixmap pixmap;
            if (pixmap.loadFromData(reinterpret_cast<const uchar *>(frame->picture().data()), frame->picture().size())) {
                coverArtLabel->setPixmap(pixmap.scaled(coverArtLabel->size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

                // Ottenere i colori dai 5 punti
                QImage image = pixmap.toImage();
                int padding = 10;
                QList<QPoint> points = {
                    QPoint(image.width() / 2, image.height() / 2), // Centro
                    QPoint(padding, padding), // Alto a sinistra
                    QPoint(image.width() - padding, padding), // Alto a destra
                    QPoint(padding, image.height() - padding), // Basso a sinistra
                    QPoint(image.width() - padding, image.height() - padding) // Basso a destra
                };

                QHash<QColor, int> colorCount;
                for (const QPoint &point : points) {
                    QColor color = image.pixelColor(point);
                    if (color != QColor(Qt::white) && color.lightness() < 200) { // Escludi i colori vicini al bianco
                        colorCount[color]++;
                    }
                }

                // Trovare il colore più frequente
                QColor dominantColor = QColor(Qt::white);
                int maxCount = 0;
                for (auto it = colorCount.constBegin(); it != colorCount.constEnd(); ++it) {
                    if (it.value() > maxCount) {
                        dominantColor = it.key();
                        maxCount = it.value();
                    }
                }

                // Cambiare il colore dello slider del player e del cerchio
                QString sliderStyle = QString("QSlider::groove:horizontal { background: #333; height: 4px; }"
                "QSlider::handle:horizontal { background: %1; width: 14px; margin: -5px 0; border-radius: 7px; }"
                "QSlider::sub-page:horizontal { background: %1; }").arg(dominantColor.name());
                progressSlider->setStyleSheet(sliderStyle);
                volumeSlider->setStyleSheet(sliderStyle);

                // Cambiare il colore dei pulsanti play/pause
                QString buttonStyle = QString("background-color: %1; color: white; border: none; padding: 10px; border-radius: 10px;").arg(dominantColor.name());
                playButton->setStyleSheet(buttonStyle);
                pauseButton->setStyleSheet(buttonStyle);

            } else {
                coverArtLabel->clear();
                resetSliderStyle();
            }
        } else {
            coverArtLabel->clear();
            resetSliderStyle();
        }
    } else {
        coverArtLabel->clear();
        resetSliderStyle();
    }
}


void MainWindow::resetSliderStyle() {
    // Resetta lo stile degli slider e dei pulsanti quando non c'è una copertina valida
    QString defaultSliderStyle = "QSlider::groove:horizontal { background: #333; height: 4px; }"
    "QSlider::handle:horizontal { background: #aaa; width: 14px; margin: -5px 0; border-radius: 7px; }"
    "QSlider::sub-page:horizontal { background: #5a5; }";
    progressSlider->setStyleSheet(defaultSliderStyle);
    volumeSlider->setStyleSheet(defaultSliderStyle);

    QString defaultButtonStyle = "background-color: #444; color: white; border: none; padding: 10px; border-radius: 10px;";
    playButton->setStyleSheet(defaultButtonStyle);
    pauseButton->setStyleSheet(defaultButtonStyle);
}



void MainWindow::jumpToSliderPosition(QMouseEvent *event, QSlider *slider) {
    int newValue = QStyle::sliderValueFromPosition(slider->minimum(), slider->maximum(), event->pos().x(), slider->width());
    slider->setValue(newValue);
    setPlayerPosition(newValue);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (obj == progressSlider) {
            jumpToSliderPosition(mouseEvent, progressSlider);
            return true;  // Evento gestito
        } else if (obj == volumeSlider) {
            // Gestisci il clic sullo slider del volume in modo indipendente
            int newValue = QStyle::sliderValueFromPosition(volumeSlider->minimum(), volumeSlider->maximum(), mouseEvent->pos().x(), volumeSlider->width());
            volumeSlider->setValue(newValue);
            musicPlayer->setVolume(newValue);
            return true;  // Evento gestito
        }
    }
    return QMainWindow::eventFilter(obj, event);
}


QString MainWindow::formatTime(qint64 milliseconds) {
    int seconds = (milliseconds / 1000) % 60;
    int minutes = (milliseconds / 60000) % 60;
    int hours = (milliseconds / 3600000);
    return hours > 0
    ? QString("%1:%2:%3").arg(hours, 2, 10, QChar('0')).arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'))
    : QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}
