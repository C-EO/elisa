/*
   SPDX-FileCopyrightText: 2020 Carson Black <uhhadd@gmail.com>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import org.kde.elisa
import QtQuick.Controls 2.3

MenuItem {
    required property int mode
    autoExclusive: true
    checkable: true
    checked: ElisaApplication.mediaPlayListProxyModel.repeatMode === mode
    onTriggered: ElisaApplication.mediaPlayListProxyModel.repeatMode = mode
}
