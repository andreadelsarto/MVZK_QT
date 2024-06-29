#include "Search.h"

void searchSongs(const QString &searchText, const QStringList &allSongs, const QList<QUrl> &playlist, QListWidget *songListWidget) {
    songListWidget->clear();
    for (const QString &song : allSongs) {
        if (song.contains(searchText, Qt::CaseInsensitive)) {
            QListWidgetItem *item = new QListWidgetItem(song, songListWidget);
            QString filePath = playlist[allSongs.indexOf(song)].toLocalFile();
            item->setData(Qt::UserRole, filePath);
        }
    }
}
