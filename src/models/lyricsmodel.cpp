/*
   SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include "lyricsmodel.h"
#include <QDebug>
#include <algorithm>
class LyricsModel::LyricsModelPrivate
{
public:
    bool parse(const QString &lyric);
    int highlightedIndex{-1};
    int timeIndex{0};
    qint64 lastPosition{1};
    std::vector<std::pair<qint64, int>>
        timeToStringIndex; // timestamp, string index pair
    std::vector<QString> lyrics;

private:
    qint64 parseOneTimeStamp(QString::const_iterator &begin,
                             QString::const_iterator end);
    QString parseOneLine(QString::const_iterator &begin,
                         QString::const_iterator end);
};
qint64 LyricsModel::LyricsModelPrivate::parseOneTimeStamp(
    QString::const_iterator &begin,
    QString::const_iterator end)
{
    /* states
     *  [00:01.02]bla bla
     * 012 34 56
     *
     * */
    auto states{0};
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
                return minute * 60 * 1000 + second * 1000 +
                    hundred * 10; // we return milliseconds
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
QString
LyricsModel::LyricsModelPrivate::parseOneLine(QString::const_iterator &begin,
                                              QString::const_iterator end)
{
    auto size{0};
    auto it = begin;
    while (begin != end) {
        if (begin->toLatin1() != QLatin1Char('[')) {
            size++;
        } else
            break;
        begin++;
    }
    if (size) {
        return QString(--it, size); // FIXME: really weird workaround for QChar,
                                    // otherwise first char is lost
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

    std::sort(timeToStringIndex.begin(),
              timeToStringIndex.end(),
              [](const std::pair<qint64, int> &lhs,
                 const std::pair<qint64, int> &rhs) {
                  return lhs.first < rhs.first;
              });

    return !timeToStringIndex.empty();
}
LyricsModel::LyricsModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(std::make_unique<LyricsModelPrivate>())
{
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
    Q_EMIT layoutAboutToBeChanged();
    d->lastPosition = -1;
    d->timeIndex = 0;
    auto ret = d->parse(lyric);

    // has non-LRC formatted lyric
    if (!ret && !lyric.isEmpty()) {
        d->timeToStringIndex = {{-1, 0}};
        d->lyrics = {lyric};
    }
    Q_EMIT layoutChanged();
    Q_EMIT lyricChanged();
}
void LyricsModel::setPosition(qint64 position)
{
    if (d->timeToStringIndex.empty())
        return;

    // non-LRC formatted lyric, no highlight
    if (d->timeToStringIndex.front().first < 0) {
        if (d->highlightedIndex != -1) {
            d->highlightedIndex = -1;
            Q_EMIT highlightedIndexChanged();
        }
        return;
    }

    qDebug() << position;
    // if progressed less than 1s, do a linear search from last index
    if (d->lastPosition >= 0 && position >= d->lastPosition &&
        position - d->lastPosition < 1000) {
        d->lastPosition = position;
        auto index = d->timeIndex;
        while (index < (int)d->timeToStringIndex.size() - 1) {
            if (d->timeToStringIndex.at(index + 1).first > position) {
                d->timeIndex = index;
                d->highlightedIndex = d->timeToStringIndex.at(index).second;
                Q_EMIT highlightedIndexChanged();
                return;
            } else
                index++;
        }
        // last lyric
        d->timeIndex = d->timeToStringIndex.size() - 1;
        d->highlightedIndex = d->timeToStringIndex.back().second;
        Q_EMIT highlightedIndexChanged();
        return;
    }

    // do binary search
    d->lastPosition = position;
    auto result =
        std::lower_bound(d->timeToStringIndex.begin(),
                         d->timeToStringIndex.end(),
                         position,
                         [](const std::pair<qint64, int> &lhs, qint64 value) {
                             return lhs.first < value;
                         });
    if (result != d->timeToStringIndex.end()) {
        d->timeIndex = std::distance(d->timeToStringIndex.begin(), result);
        d->highlightedIndex = result->second;
        Q_EMIT highlightedIndexChanged();
    }
}
int LyricsModel::highlightedIndex() const
{
    return d->highlightedIndex;
}
