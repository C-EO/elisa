/*
   SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */
#ifndef LYRICSMODEL_H
#define LYRICSMODEL_H
#include "elisaLib_export.h"
#include <QAbstractListModel>
#include <memory>
#include <vector>
class ELISALIB_EXPORT LyricsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString lyric WRITE setLyric NOTIFY lyricChanged)
    Q_PROPERTY(int highlightedIndex READ highlightedIndex NOTIFY
                   highlightedIndexChanged)
    Q_PROPERTY(qint64 position WRITE setPosition NOTIFY positionChanged)
public:
    LyricsModel(QObject *parent = nullptr);
    ~LyricsModel() override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index,
                  int role = Qt::DisplayRole) const override;
    void setLyric(const QString &lyric);
    void setPosition(qint64 position);
    int highlightedIndex() const;
Q_SIGNALS:
    void lyricChanged();
    void highlightedIndexChanged();
    void positionChanged();

private:
    class LyricsModelPrivate;
    std::unique_ptr<LyricsModelPrivate> d;
};

#endif // LYRICSMODEL_H
