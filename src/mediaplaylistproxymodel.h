/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2019 (c) Alexander Stippich <a.stippich@gmx.net>
   SPDX-FileCopyrightText: 2020 (c) Carson Black <uhhadd@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef MEDIAPLAYLISTPROXYMODEL_H
#define MEDIAPLAYLISTPROXYMODEL_H

#include "elisaLib_export.h"
#include "elisautils.h"
#include "datatypes.h"

#include <QAbstractProxyModel>
#include <QMediaPlayer>
#include <QMimeType>
#include <QQmlEngine>
#include <QFileInfo>

#include <memory>

class M3uPlaylistParser
{
public:
    QList<QUrl> fromPlaylist(const QUrl &fileName, const QByteArray &fileContent);
    QString toPlaylist(const QUrl &fileName, const QList<QString> &listOfUrls);
};

class PlsPlaylistParser
{
public:
    QList<QUrl> fromPlaylist(const QUrl &fileName, const QByteArray &fileContent);
    QString toPlaylist(const QUrl &fileName, const QList<QString> &listOfUrls);
};

class ELISALIB_EXPORT PlaylistParser
{
public:
    QList<QUrl> fromPlaylist(const QUrl &fileName, const QByteArray &fileContent);
    QString toPlaylist(const QUrl &fileName, const QList<QString> &listOfUrls);

private:
    int filterImported(QList<QUrl>& result, const QUrl &playlistUrl);
};

class MediaPlayList;
class MediaPlayListProxyModelPrivate;

class ELISALIB_EXPORT MediaPlayListProxyModel : public QAbstractProxyModel
{

    Q_OBJECT

    QML_ELEMENT

public:
    enum Repeat {
        None,
        One,
        Playlist
    };
    Q_ENUM(Repeat)

    enum Shuffle {
        NoShuffle,
        Track,
        Album
    };
    Q_ENUM(Shuffle)

private:

    Q_PROPERTY(QVariantMap persistentState
               READ persistentState
               WRITE setPersistentState
               NOTIFY persistentStateChanged)

    Q_PROPERTY(QPersistentModelIndex previousTrack
               READ previousTrack
               NOTIFY previousTrackChanged)

    Q_PROPERTY(QPersistentModelIndex currentTrack
               READ currentTrack
               NOTIFY currentTrackChanged)

    Q_PROPERTY(QPersistentModelIndex nextTrack
               READ nextTrack
               NOTIFY nextTrackChanged)

    Q_PROPERTY(Repeat repeatMode
               READ repeatMode
               WRITE setRepeatMode
               NOTIFY repeatModeChanged)

    Q_PROPERTY(Shuffle shuffleMode
               READ shuffleMode
               WRITE setShuffleMode
               NOTIFY shuffleModeChanged)

    // in milliseconds
    Q_PROPERTY(int totalTracksDuration
               READ totalTracksDuration
               NOTIFY totalTracksDurationChanged)

    // in milliseconds
    Q_PROPERTY(int remainingTracksDuration
               READ remainingTracksDuration
               NOTIFY remainingTracksDurationChanged)

    Q_PROPERTY(int remainingTracks
               READ remainingTracks
               NOTIFY remainingTracksChanged)

    Q_PROPERTY(int currentTrackRow
               READ currentTrackRow
               NOTIFY currentTrackRowChanged)

    Q_PROPERTY(int radioCount
               READ radioCount
               NOTIFY radioCountChanged)

    //radios are also tracks
    Q_PROPERTY(int tracksCount
               READ tracksCount
               NOTIFY tracksCountChanged)

    Q_PROPERTY(bool partiallyLoaded
               READ partiallyLoaded
               NOTIFY partiallyLoadedChanged
               RESET resetPartiallyLoaded)

    Q_PROPERTY(bool canOpenLoadedPlaylist
               READ canOpenLoadedPlaylist
               NOTIFY canOpenLoadedPlaylistChanged)

public:

    explicit MediaPlayListProxyModel(QObject *parent = nullptr);

    ~MediaPlayListProxyModel() override;

    [[nodiscard]] QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;

    [[nodiscard]] QItemSelection mapSelectionFromSource(const QItemSelection &sourceSelection) const override;

    [[nodiscard]] QItemSelection mapSelectionToSource(const QItemSelection &proxySelection) const override;

    [[nodiscard]] QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

    [[nodiscard]] int mapRowFromSource(const int sourceRow) const;

    [[nodiscard]] int mapRowToSource(const int proxyRow) const;

    [[nodiscard]] int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    [[nodiscard]] int columnCount(const QModelIndex &parent) const override;

    [[nodiscard]] QModelIndex parent(const QModelIndex &child) const override;

    [[nodiscard]] bool hasChildren(const QModelIndex &parent) const override;

    void setPlayListModel(MediaPlayList* playListModel);

    [[nodiscard]] QPersistentModelIndex previousTrack() const;

    [[nodiscard]] QPersistentModelIndex currentTrack() const;

    [[nodiscard]] QPersistentModelIndex nextTrack() const;

    [[nodiscard]] Repeat repeatMode() const;

    [[nodiscard]] Shuffle shuffleMode() const;

    [[nodiscard]] int totalTracksDuration() const;

    [[nodiscard]] int remainingTracksDuration() const;

    [[nodiscard]] int remainingTracks() const;

    [[nodiscard]] int currentTrackRow() const;

    [[nodiscard]] int radioCount() const;

    [[nodiscard]] int tracksCount() const;

    [[nodiscard]] QVariantMap persistentState() const;

    [[nodiscard]] bool partiallyLoaded() const;

    [[nodiscard]] bool canOpenLoadedPlaylist() const;

    int mSeekToBeginningDelay = 2000;


public Q_SLOTS:

    void enqueue(const QUrl &entryUrl,
                 ElisaUtils::PlayListEnqueueMode enqueueMode,
                 ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void enqueue(const DataTypes::MusicDataType &newEntry,
                 const QString &newEntryTitle,
                 ElisaUtils::PlayListEnqueueMode enqueueMode,
                 ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void enqueue(const DataTypes::EntryDataList &newEntries,
                 ElisaUtils::PlayListEnqueueMode enqueueMode,
                 ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void setRepeatMode(Repeat value);

    void setShuffleMode(Shuffle value);

    void trackInError(const QUrl &sourceInError, QMediaPlayer::Error playerError);

    void skipNextTrack(ElisaUtils::SkipReason reason = ElisaUtils::SkipReason::Automatic);

    void skipPreviousTrack(qint64 position);

    void switchTo(int row);

    void removeSelection(QList<int> selection);

    void removeRow(int row);

    void moveRow(int from, int to);

    void clearPlayList();

    void undoClearPlayList();

    bool savePlayList(const QUrl &fileName);

    void loadPlayList(const QUrl &fileName);

    void loadPlayList(const QUrl &fileName, ElisaUtils::PlayListEnqueueMode enqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void setPersistentState(const QVariantMap &persistentState);

    void openLoadedPlayList();

    void resetPartiallyLoaded();

    int indexForTrackUrl(const QUrl &url);

    void switchToTrackUrl(const QUrl &url, ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

Q_SIGNALS:

    void previousTrackChanged(const QPersistentModelIndex &previousTrack);

    void currentTrackChanged(const QPersistentModelIndex &currentTrack);

    void nextTrackChanged(const QPersistentModelIndex &nextTrack);

    void previousTrackDataChanged();

    void currentTrackDataChanged();

    void nextTrackDataChanged();

    void repeatModeChanged();

    void shuffleModeChanged();

    void totalTracksDurationChanged();

    void remainingTracksDurationChanged();

    void remainingTracksChanged();

    void ensurePlay();

    void requestPlay();

    void currentTrackRowChanged();

    void radioCountChanged();

    void tracksCountChanged();

    void playListFinished();

    void playListLoaded();

    void playListLoadFailed();

    void persistentStateChanged();

    void clearPlayListPlayer();

    void undoClearPlayListPlayer();

    void displayUndoNotification();

    void hideUndoNotification();

    void seek(qint64 position);

    void partiallyLoadedChanged();

    void canOpenLoadedPlaylistChanged();

private Q_SLOTS:

    void sourceRowsAboutToBeInserted(const QModelIndex &parent, int start, int end);

    void sourceRowsInserted(const QModelIndex &parent, int start, int end);

    void sourceRowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);

    void sourceRowsRemoved(const QModelIndex &parent, int start, int end);

    void sourceRowsAboutToBeMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destParent, int destRow);

    void sourceRowsMoved(const QModelIndex &parent, int start, int end, const QModelIndex &destParent, int destRow);

    void sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles);

    void sourceHeaderDataChanged(Qt::Orientation orientation, int first, int last);

    void sourceLayoutAboutToBeChanged();

    void sourceLayoutChanged();

    void sourceModelAboutToBeReset();

    void sourceModelReset();

private:

    void setSourceModel(QAbstractItemModel *sourceModel) override;

    void determineTracks();

    void notifyCurrentTrackRowChanged();

    void notifyCurrentTrackChanged();

    void determineAndNotifyPreviousAndNextTracks();

    QVariantList getRandomMappingForRestore() const;

    void restoreShuffleMode(Shuffle mode, QVariantList mapping);

    void loadLocalFile(DataTypes::EntryDataList &newTracks, QSet<QString> &processedFiles, const QFileInfo &fileInfo);

    void loadLocalPlayList(DataTypes::EntryDataList &newTracks, QSet<QString> &processedUFiles, const QUrl &fileName, const QByteArray &fileContent);

    void loadLocalDirectory(DataTypes::EntryDataList &newTracks, QSet<QString> &processedFiles, const QUrl &dirName);

    int filterLocalPlayList(QList<QUrl>& result, const QUrl &playlistUrl);

    std::unique_ptr<MediaPlayListProxyModelPrivate> d;
};

#endif // MEDIAPLAYLISTPROXYMODEL_H
