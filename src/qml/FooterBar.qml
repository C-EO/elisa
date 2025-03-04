/**
 * SPDX-FileCopyrightText: 2023 Arkadiusz Guzinski <kermit@ag.de1.cc>
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa


/*
  This is intended as a area to show status messages and outher helpful stuff.
  For now it only supports TrackImportNotification
 */
ToolBar {
    implicitHeight: importedTracksCountNotification.opacity > 0 ? importedTracksCountNotification.height + (Kirigami.Units.smallSpacing * 2) : 0

    position: ToolBar.Footer

    contentItem: RowLayout {
        layoutDirection: Qt.RightToLeft

        // track import notification
        TrackImportNotification {
            id: importedTracksCountNotification

            Binding {
                id: indexerBusyBinding
                target: importedTracksCountNotification
                property: 'indexingRunning'
                value: ElisaApplication.musicManager.indexerBusy
                when: ElisaApplication.musicManager !== undefined
            }

            Binding {
                target: importedTracksCountNotification
                property: 'importedTracksCount'
                value: ElisaApplication.musicManager.importedTracksCount
                when: ElisaApplication.musicManager !== undefined
            }
        }
    }

    Behavior on implicitHeight {
        NumberAnimation {
            duration: Kirigami.Units.longDuration
        }
    }
}
