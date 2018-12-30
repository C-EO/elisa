/*
 * Copyright 2018 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.10
import QtQuick.Controls 2.3
import org.kde.elisa 1.0

MediaBrowser {
    id: localTracks

    focus: true

    anchors {
        fill: parent

        leftMargin: elisaTheme.layoutHorizontalMargin
        rightMargin: elisaTheme.layoutHorizontalMargin
    }

    DataModel {
        id: realModel
    }

    AllTracksProxyModel {
        id: proxyModel

        sortRole: Qt.DisplayRole
        sourceModel: realModel

        onTrackToEnqueue: elisa.mediaPlayList.enqueue(newEntries, databaseIdType, enqueueMode, triggerPlay)
    }

    firstPage: ListBrowserView {
        id: allTracksView
        focus: true

        contentModel: proxyModel

        delegate: MediaTrackDelegate {
            id: entry

            width: allTracksView.delegateWidth
            height: elisaTheme.trackDelegateHeight

            focus: true

            databaseId: model.databaseId
            title: model.title
            artist: model.artist
            album: (model.album !== undefined && model.album !== '' ? model.album : '')
            albumArtist: model.albumArtist
            duration: model.duration
            imageUrl: (model.imageUrl !== undefined && model.imageUrl !== '' ? model.imageUrl : '')
            trackNumber: model.trackNumber
            discNumber: model.discNumber
            rating: model.rating
            isFirstTrackOfDisc: false
            isSingleDiscAlbum: model.isSingleDiscAlbum

            onEnqueue: elisa.mediaPlayList.enqueue(databaseId, name, ElisaUtils.Track,
                                                   ElisaUtils.AppendPlayList,
                                                   ElisaUtils.DoNotTriggerPlay)

            onReplaceAndPlay: elisa.mediaPlayList.enqueue(databaseId, name, ElisaUtils.Track,
                                                          ElisaUtils.ReplacePlayList,
                                                          ElisaUtils.TriggerPlay)

            onClicked: contentDirectoryView.currentIndex = index
        }

        image: elisaTheme.tracksIcon
        mainTitle: i18nc("Title of the view of all tracks", "Tracks")
    }

    Connections {
        target: elisa

        onMusicManagerChanged: realModel.initialize(elisa.musicManager, ElisaUtils.Track)
    }

    Component.onCompleted: {
        if (elisa.musicManager) {
            realModel.initialize(elisa.musicManager, ElisaUtils.Track)
        }
    }
}