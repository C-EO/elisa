/*
 * Copyright 2017 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef ELISAUTILS_H
#define ELISAUTILS_H

#include "elisaLib_export.h"

#include <QMetaObject>

namespace ElisaUtils {

Q_NAMESPACE

enum PlayListEnqueueMode {
    AppendPlayList,
    ReplacePlayList,
};

Q_ENUM_NS(PlayListEnqueueMode)

enum PlayListEnqueueTriggerPlay {
    DoNotTriggerPlay,
    TriggerPlay,
};

Q_ENUM_NS(PlayListEnqueueTriggerPlay)

enum ColumnsRoles {
    TitleRole = Qt::UserRole + 1,
    SecondaryTextRole,
    ImageUrlRole,
    ShadowForImageRole,
    ChildModelRole,
    DurationRole,
    MilliSecondsDurationRole,
    ArtistRole,
    AllArtistsRole,
    HighestTrackRating,
    AlbumRole,
    AlbumArtistRole,
    TrackNumberRole,
    DiscNumberRole,
    RatingRole,
    GenreRole,
    LyricistRole,
    ComposerRole,
    CommentRole,
    YearRole,
    ChannelsRole,
    BitRateRole,
    SampleRateRole,
    ImageRole,
    ResourceRole,
    IdRole,
    DatabaseIdRole,
    IsSingleDiscAlbumRole,
    ContainerDataRole,
    IsPartialDataRole,
};

Q_ENUM_NS(ColumnsRoles)

}

#endif // ELISAUTILS_H
