/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

#ifndef FILEBROWSERPROXYMODEL_H
#define FILEBROWSERPROXYMODEL_H

#include "elisaLib_export.h"

#include "filescanner.h"
#include "elisautils.h"

#include <KDirSortFilterProxyModel>
#include <KJob>
#include <KIO/ListJob>
#include <KIO/UDSEntry>

#include <QMimeDatabase>
#include <QQmlEngine>
#include <QRegularExpression>

#include <queue>
#include <memory>

class MediaPlayListProxyModel;

class ELISALIB_EXPORT FileBrowserProxyModel : public KDirSortFilterProxyModel
{
    Q_OBJECT

    QML_ELEMENT

    Q_PROPERTY(QString filterText
               READ filterText
               WRITE setFilterText
               NOTIFY filterTextChanged)

    Q_PROPERTY(int filterRating
               READ filterRating
               WRITE setFilterRating
               NOTIFY filterRatingChanged)

    Q_PROPERTY(bool sortedAscending
               READ sortedAscending
               NOTIFY sortedAscendingChanged)

    Q_PROPERTY(MediaPlayListProxyModel* playList READ playList WRITE setPlayList NOTIFY playListChanged)

public:

    explicit FileBrowserProxyModel(QObject *parent = nullptr);

    ~FileBrowserProxyModel() override;

    [[nodiscard]] QString filterText() const;

    [[nodiscard]] int filterRating() const;

    [[nodiscard]] bool sortedAscending() const;

    [[nodiscard]] MediaPlayListProxyModel* playList() const;

public Q_SLOTS:

    void enqueueAll(ElisaUtils::PlayListEnqueueMode enqueueMode, ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void enqueue(const DataTypes::MusicDataType &newEntry,
                 const QString &newEntryTitle,
                 ElisaUtils::PlayListEnqueueMode enqueueMode,
                 ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void setFilterText(const QString &filterText);

    void setFilterRating(int filterRating);

    void setPlayList(MediaPlayListProxyModel* playList);

    void sortModel(Qt::SortOrder order);

Q_SIGNALS:

    void entriesToEnqueue(const DataTypes::EntryDataList &newEntries,
                          ElisaUtils::PlayListEnqueueMode enqueueMode,
                          ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void filterTextChanged(const QString &filterText);

    void filterRatingChanged();

    void sortedAscendingChanged();

    void playListChanged();

private Q_SLOTS:

    void listRecursiveResult(KJob *job);

    void listRecursiveNewEntries(KIO::Job *job, const KIO::UDSEntryList &list);

private:

    void genericEnqueueToPlayList(const QModelIndex &rootIndex,
                                  ElisaUtils::PlayListEnqueueMode enqueueMode,
                                  ElisaUtils::PlayListEnqueueTriggerPlay triggerPlay);

    void disconnectPlayList();

    void connectPlayList();

    void recursiveEnqueue();

    QString mFilterText;

    QRegularExpression mFilterExpression;

    MediaPlayListProxyModel* mPlayList = nullptr;

    int mFilterRating = 0;

    QMimeDatabase mMimeDatabase;

    bool mEnqueueInProgress = false;

    std::queue<std::tuple<QUrl, bool>> mPendingEntries;

    DataTypes::EntryDataList mAllData;

    ElisaUtils::PlayListEnqueueMode mEnqueueMode;

    ElisaUtils::PlayListEnqueueTriggerPlay mTriggerPlay;

    QUrl mCurentUrl;

    KIO::Job *mCurrentJob = nullptr;

};

#endif // FILEBROWSERPROXYMODEL_H
