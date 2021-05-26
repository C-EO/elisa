/*
   SPDX-FileCopyrightText: 2021 Han Young <hanyoung@protonmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */
#include "lyricsmodel.h"
#include <QDebug>
#include <algorithm>
#include <unordered_map>
#include <KLocalizedString>
class LyricsModel::LyricsModelPrivate
{
public:
    bool parse(const QString &lyric);
    int highlightedIndex{-1};
    int timeIndex{0};
    qint64 lastPosition{1};

    // pairs of lyric timestamp and corresponding content index in lyrics vector
    std::vector<std::pair<qint64, int>> timeToStringIndex;
    std::vector<QString> lyrics;

private:
    qint64 parseOneTimeStamp(QString::const_iterator &begin, QString::const_iterator end);
    QString parseOneLine(QString::const_iterator &begin, QString::const_iterator end);
    QString parseTags(QString::const_iterator &begin, QString::const_iterator end);
};
/*###########parseOneTimeStamp###########
 * Function to parse timestamp of one LRC line
 * if successful, return timestamp in milliseconds
 * otherwise return -1
 * */
qint64 LyricsModel::LyricsModelPrivate::parseOneTimeStamp(
    QString::const_iterator &begin,
    QString::const_iterator end)
{
    /* Example of LRC format and corresponding states
     *
     * States:
     *
     *  [00:01.02]bla bla
     * ^^^ ^^ ^^ ^^
     * ||| || || ||
     * ||| || || |End
     * ||| || || RightBracket
     * ||| || |Hundredths
     * ||| || Period
     * ||| |Seconds
     * ||| Colon
     * ||Minutes
     * |LeftBracket
     * Start
     * */
    enum States {Start, LeftBracket, Minutes, Colon, Seconds, Period, Hundredths, RightBracket, End};
    auto states {States::Start};
    auto minute {0}, second {0}, hundred {0};

    while (begin != end) {
        switch (begin->toLatin1()) {
        case '.':
            if (states == Seconds)
                states = Period;
            break;
        case '[':
            if (states == Start)
                states = LeftBracket;
            break;
        case ']':
            begin++;
            if (states == Hundredths) {
                return minute * 60 * 1000 + second * 1000 +
                    hundred * 10; // we return milliseconds
            }
            else {
                return -1;
            }
        case ':':
            if (states == Minutes)
                states = Colon;
            break;
        default:
            if (begin->isDigit()) {
                switch (states) {
                    case LeftBracket:
                        states = Minutes;
                    case Minutes:
                        minute *= 10;
                        minute += begin->toLatin1() - '0';
                        break;
                    case Colon:
                        states = Seconds;
                    case Seconds:
                        second *= 10;
                        second += begin->toLatin1() - '0';
                        break;
                    case Period:
                        states = Hundredths;
                    case Hundredths:
                        // we only parse to hundredth second
                        if (hundred >= 100) {
                            break;
                        }
                        hundred *= 10;
                        hundred += begin->toLatin1() - '0';
                        break;
                    default:
                        // lyric format is corrupt
                        break;
                }
            } else {
                begin++;
                return -1;
            }
            break;
        }
        begin++;
    }

    // end of lyric and no correct value found
    return -1;
}
QString
LyricsModel::LyricsModelPrivate::parseOneLine(QString::const_iterator &begin,
                                              QString::const_iterator end)
{
    auto size{0};
    auto it = begin;
    while (begin != end) {
        if (begin->toLatin1() != '[') {
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
QString LyricsModel::LyricsModelPrivate::parseTags(QString::const_iterator &begin, QString::const_iterator end)
{
    static auto skipTillChar = [](QString::const_iterator begin, QString::const_iterator end, char endChar) {
        while (begin != end && begin->toLatin1() != endChar) {
            begin++;
        }
        return begin;
    };
    static std::unordered_map<QString, QString> map = {{QStringLiteral("ar"), i18n("Artist: ")},
                                                       {QStringLiteral("al"), i18n("Album: ")},
                                                       {QStringLiteral("ti"), i18n("Title: ")},
                                                       {QStringLiteral("au"), i18n("Creator: ")},
                                                       {QStringLiteral("length"), i18n("Length: ")},
                                                       {QStringLiteral("by"), i18n("Created by: ")},
                                                       {QStringLiteral("re"), i18n("Editor: ")},
                                                       {QStringLiteral("ve"), i18n("Version: ")}};
    QString tags;

    while (begin != end) {
        // skip till tags
        begin = skipTillChar(begin, end, '[');
        if (begin != end)
            begin++;
        else
            break;

        auto tagIdEnd = skipTillChar(begin, end, ':');
        auto tagId = QString(begin, std::distance(begin, tagIdEnd));
        if(tagIdEnd != end && map.count(tagId)) {
            tagIdEnd++;
            auto tagContentEnd = skipTillChar(tagIdEnd, end, ']');
            tags += map[tagId] + QString(tagIdEnd, std::distance(tagIdEnd, tagContentEnd))
                    + QStringLiteral("\n");
            begin = tagContentEnd;
        } else {
            return tags;
        }
    }
    return tags;
}
bool LyricsModel::LyricsModelPrivate::parse(const QString &lyric)
{
    timeToStringIndex.clear();
    lyrics.clear();

    if (lyric.isEmpty())
        return false;

    QString::const_iterator begin = lyric.begin(), end = lyric.end();
    auto tag = parseTags(begin, end);
    int index = 0;
    std::vector<qint64> timeStamps;

    while (begin != lyric.end()) {
        auto timeStamp = parseOneTimeStamp(begin, end);
        while (timeStamp >= 0) {
            timeStamps.push_back(timeStamp);
            timeStamp = parseOneTimeStamp(begin, end);
        }
        auto string = parseOneLine(begin, end);
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

    // insert tags to first lyric front
    if (!timeToStringIndex.empty() && !tag.isEmpty()) {
        lyrics.at(timeToStringIndex.begin()->second).push_front(tag);
    }
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
    beginResetModel();
    d->lastPosition = -1;
    d->timeIndex = 0;
    auto ret = d->parse(lyric);

    // has non-LRC formatted lyric
    if (!ret && !lyric.isEmpty()) {
        d->timeToStringIndex = {{-1, 0}};
        d->lyrics = {lyric};
        d->highlightedIndex = -1;
    }
    endResetModel();
    Q_EMIT highlightedIndexChanged();
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
