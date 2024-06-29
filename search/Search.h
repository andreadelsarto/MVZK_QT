#ifndef SEARCH_H
#define SEARCH_H

#include <QString>
#include <QListWidget>
#include <QStringList>
#include <QUrl>

void searchSongs(const QString &searchText, const QStringList &allSongs, const QList<QUrl> &playlist, QListWidget *songListWidget);

#endif // SEARCH_H
