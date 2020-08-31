/*
   SPDX-FileCopyrightText: 2017 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import org.kde.kirigami 2.0 as Kirigami

Item {
    property string defaultAlbumImage: 'image://icon/media-optical-audio'
    property string defaultArtistImage: 'image://icon/view-media-artist'
    property string defaultBackgroundImage: 'qrc:///background.png'
    property string nowPlayingIcon: 'image://icon/view-media-lyrics'
    property string artistIcon: 'image://icon/view-media-artist'
    property string albumIcon: 'image://icon/view-media-album-cover'
    property string albumCoverIcon: 'image://icon/media-optical-audio'
    property string tracksIcon: 'image://icon/view-media-track'
    property string genresIcon: 'image://icon/view-media-genre'
    property string clearIcon: 'image://icon/edit-clear'
    property string recentlyPlayedTracksIcon: 'image://icon/media-playlist-play'
    property string frequentlyPlayedTracksIcon: 'image://icon/view-media-playcount'
    property string pausedIndicatorIcon: 'image://icon/media-playback-paused'
    property string playingIndicatorIcon: 'image://icon/media-playback-playing'
    property string ratingIcon: 'image://icon/rating'
    property string ratingUnratedIcon: 'image://icon/rating-unrated'
    property string errorIcon: 'image://icon/error'
    property string folderIcon: 'image://icon/document-open-folder'

    property int hairline: Math.floor(Kirigami.Units.devicePixelRatio)

    property int playListAlbumArtSize: 60

    property int coverImageSize: 180
    property int contextCoverImageSize: 100
    property int smallImageSize: 32

    property int tooltipRadius: 3
    property int shadowOffset: 2

    property int delegateToolButtonSize: 34

    property int mediaPlayerControlHeight: 42
    property real mediaPlayerControlOpacity: 0.6
    property int volumeSliderWidth: 100

    property int dragDropPlaceholderHeight: 28

    property int gridDelegateSize: 170

    property int viewSelectorSmallSizeThreshold: 800
}
