/*
 * Copyright 2016 Matthieu Gallien <matthieu_gallien@yahoo.fr>
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

import QtQuick 2.7
import QtQuick.Layouts 1.2
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.5 as Kirigami

Row {
    property int starRating
    property bool readOnly: true

    property double hoverBrightness: 0.5
    property double hoverContrast: 0.5
    property double hoverWidgetOpacity: 0

    property int hoveredRating: 0

    signal ratingEdited()

    spacing: 0
    opacity: starRating >= 2 ? 1 : hoverWidgetOpacity

    Repeater {
        model: 5

        Item {
            property int ratingThreshold: 2 + index * 2

            height: Kirigami.Units.iconSizes.small
            width: Kirigami.Units.iconSizes.small

            Image {
                width: Kirigami.Units.iconSizes.small
                height: Kirigami.Units.iconSizes.small
                anchors.centerIn: parent
                sourceSize.width: Kirigami.Units.iconSizes.small
                sourceSize.height: Kirigami.Units.iconSizes.small
                fillMode: Image.PreserveAspectFit

                layer.enabled: hoveredRating >= ratingThreshold

                layer.effect: BrightnessContrast {
                    brightness: hoverBrightness
                    contrast: hoverContrast
                }

                source: if (starRating >= ratingThreshold || hoveredRating >= ratingThreshold)
                            Qt.resolvedUrl(elisaTheme.ratingIcon)
                        else
                            Qt.resolvedUrl(elisaTheme.ratingUnratedIcon)
                opacity: if (starRating >= ratingThreshold || hoveredRating >= ratingThreshold)
                            1
                        else
                            0.7
            }

            MouseArea {
                anchors.fill: parent

                enabled: !readOnly

                acceptedButtons: Qt.LeftButton
                hoverEnabled: true

                onClicked: {
                    if (starRating !== ratingThreshold) {
                        starRating = ratingThreshold
                    } else {
                        starRating = 0
                    }
                    ratingEdited()
                }

                onEntered: hoveredRating = ratingThreshold
                onExited: hoveredRating = 0
            }
        }
    }
}
