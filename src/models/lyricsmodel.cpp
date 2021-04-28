/*
   SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include "lyricsmodel.h"
#include <algorithm>
#include <QDebug>
class LyricsModel::LyricsModelPrivate
{
public:
    bool parse(const QString &lyric);
    int highlightedIndex {0};
    qint64 lastPosition {0};
    std::vector<std::pair<qint64, int>> timeToStringIndex; // timestamp, string index pair
    std::vector<QString> lyrics;
private:
    qint64 parseOneTimeStamp(QString::const_iterator &begin, QString::const_iterator end);
    QString parseOneLine(QString::const_iterator &begin, QString::const_iterator end);
};
qint64 LyricsModel::LyricsModelPrivate::parseOneTimeStamp(QString::const_iterator &begin, QString::const_iterator end)
{
    /* states
     *  [00:01.02]bla bla
     * 012 34 56
     *
     * */
    auto states {0};
    int minute = 0, second = 0, hundred = 0;
    while (begin != end) {
        switch (begin->toLatin1()) {
            case '.':
                if (states == 4)
                    states = 5;
                break;
            case '[':
                if (states == 0)
                    states = 1;
                break;
            case ']':
                begin++;
                if (states == 6)
                    return minute * 60 * 1000 + second * 1000 + hundred * 10; // we return milliseconds
                else
                    return -1;
            case ':':
                if (states == 2)
                    states = 3;
                break;
            default:
                if (isdigit(begin->toLatin1())) {
                    if (states == 1 || states == 3 || states == 5)
                        states++;

                    auto ret = begin->toLatin1() - '0';
                    if (states == 2) {
                        minute *= 10;
                        minute += ret;
                    } else if (states == 4) {
                        second *= 10;
                        second += ret;
                    } else if (states == 6) {
                        hundred *= 10;
                        hundred += ret;
                    }
                } else if (states == 0) {
                    begin++;
                    return -1;
                }
                break;
        }
        begin++;
    }
    return -1;
}
QString LyricsModel::LyricsModelPrivate::parseOneLine(QString::const_iterator &begin, QString::const_iterator end)
{
    auto size {0};
    auto it = begin;
    while (begin != end) {
        if (begin->toLatin1() != QLatin1Char('[')) {
            size++;
        }
        else
            break;
        begin++;
    }
    if (size) {
        return QString(it, size);
    } else
        return {};
}
bool LyricsModel::LyricsModelPrivate::parse(const QString &lyric)
{
    timeToStringIndex.clear();
    lyrics.clear();

    if (lyric.isEmpty())
        return false;

    QString::const_iterator begin = lyric.begin();

    int index = 0;
    std::vector<qint64> timeStamps;

    while (begin != lyric.end()) {
        auto timeStamp = parseOneTimeStamp(begin, lyric.end());
        while (timeStamp >= 0) {
            timeStamps.push_back(timeStamp);
            timeStamp = parseOneTimeStamp(begin, lyric.end());
        }
        auto string = parseOneLine(begin, lyric.end());
        if (!string.isEmpty() && !timeStamps.empty()) {
            for (auto time : timeStamps) {
                timeToStringIndex.push_back({time, index});
            }
            index++;
            lyrics.emplace_back(std::move(string));
        }
        timeStamps.clear();
    }

    std::sort(timeToStringIndex.begin(), timeToStringIndex.end(), [](const std::pair<qint64, int>& lhs, const std::pair<qint64, int>& rhs)
    {
        return lhs.first < rhs.first;
    });

    for (auto pair : timeToStringIndex)
        qDebug() << pair.first << " " << lyrics[pair.second];

    return !timeToStringIndex.empty();
}
LyricsModel::LyricsModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(std::make_unique<LyricsModelPrivate>())
{
    connect(this, &LyricsModel::highlightedIndexChanged, [this]
    {if (highlightedIndex() < d->lyrics.size())
        qDebug() << "index: " << d->lyrics.at(highlightedIndex());});
}

LyricsModel::~LyricsModel() = default;

int LyricsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return d->lyrics.size();
}
QVariant LyricsModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role)

    if (index.row() < 0 || index.row() >= (int)d->lyrics.size())
        return {};

    return d->lyrics.at(index.row());
}
void LyricsModel::setLyric(const QString &lyric)
{
    d->lastPosition = 0;
    d->parse(lyric);
    Q_EMIT lyricChanged();
    Q_EMIT highlightedIndexChanged();
}
void LyricsModel::setPosition(qint64 position)
{
    qDebug() << "c++: " << position;
    // if progressed less than 1s, do a linear search from last index
    if (position >= d->lastPosition && position - d->lastPosition < 1000) {
        d->lastPosition = position;
        auto index = d->highlightedIndex;
        while (index < (int)d->timeToStringIndex.size() - 1) {
            if (d->timeToStringIndex.at(index).first <= position &&
                    d->timeToStringIndex.at(index + 1).first > position) {
                d->highlightedIndex = d->timeToStringIndex.at(index).second;
                Q_EMIT highlightedIndexChanged();
                return;
            } else
                index++;
        }

        // last lyric
        d->highlightedIndex = d->lyrics.size() - 1;
        Q_EMIT highlightedIndexChanged();
        return;
    }

    // do binary search
    d->lastPosition = position;
    auto result = std::lower_bound(d->timeToStringIndex.begin(), d->timeToStringIndex.end(), position,
         [](const std::pair<qint64, int>& lhs, qint64 value)
         {
            return lhs.first < value;
         });
    if (result != d->timeToStringIndex.end()) {

        d->highlightedIndex = result->second;
        Q_EMIT highlightedIndexChanged();
    }
}
int LyricsModel::highlightedIndex() const
{
    return d->highlightedIndex;
}
