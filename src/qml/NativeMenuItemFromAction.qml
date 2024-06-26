/*
   SPDX-FileCopyrightText: 2020 (c) Carson Black <uhhadd@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import Qt.labs.platform 1.1
import org.kde.elisa

MenuItem {
    property string elisaAction: ""
    property var __action: ElisaApplication.action(elisaAction)

    text: __action.text
    shortcut: ElisaApplication.actionShortcut(__action)
    icon.name: ElisaApplication.iconName(__action.icon)
    onTriggered: __action.trigger()
    visible: __action.text !== ""
}
