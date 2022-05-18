/*
   SPDX-FileCopyrightText: 2018 (c) Alexander Stippich <a.stippich@gmx.net>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.3
import org.kde.kirigami 2.5 as Kirigami
import org.kde.elisa 1.0

ToolButton {
    display: AbstractButton.IconOnly

    ToolTip.visible: hovered && text.length > 0
    ToolTip.delay: Qt.styleHints.mousePressAndHoldInterval
    ToolTip.text: text

    Keys.onReturnPressed: action ? action.trigger() : clicked()
    Keys.onEnterPressed: action ? action.trigger() : clicked()

    Accessible.name: ToolTip.text
    Accessible.description: ToolTip.text
    Accessible.onPressAction: clicked()
}
