/*
   SPDX-FileCopyrightText: 2016 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-3.0-or-later
 */

import QtQml 2.2
import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.3
import QtGraphicalEffects 1.0
import org.kde.kirigami 2.8 as Kirigami
import org.kde.elisa 1.0

Item {
    id: navigationBar

    property string mainTitle
    property string secondaryTitle
    property url image
    property bool allowArtistNavigation: false
    property bool showEnqueueButton: true
    property bool showCreateRadioButton

    property string labelText
    property bool showRating: !Kirigami.Settings.isMobile
    property alias filterText: filterTextInput.text
    property alias filterRating: ratingFilter.starRating
    property bool enableGoBack: true
    property bool expandedFilterView
    property bool enableSorting: true
    property alias sortRole: sortMenu.sortRole
    property alias sortRoles: sortMenu.sortRoles
    property alias sortRoleNames: sortMenu.sortRoleNames
    property alias sortOrder: sortMenu.sortOrder
    property alias sortOrderNames: sortMenu.sortOrderNames

    property ViewManager viewManager

    signal enqueue();
    signal replaceAndPlay();
    signal createRadio();
    signal goBack();

    property bool isWidescreen: mainWindow.width >= elisaTheme.viewSelectorSmallSizeThreshold

    implicitHeight: layout.height

    SortMenu {
        id: sortMenu
        onSortOrderChanged: if (viewManager) viewManager.sortOrderChanged(sortOrder)
        onSortRoleChanged: if (viewManager) viewManager.sortRoleChanged(sortRole)
    }

    // shared actions between mobile and desktop
    Component {
        id: sortMenuComponent
        FlatButtonWithToolTip {
            id: sortMenuButton
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            display: AbstractButton.TextOnly

            checkable: true
            checked: sortMenu.visible

            onClicked: {
                if (sortMenu.visible) {
                    sortMenu.dismiss()
                } else {
                    sortMenu.sortOrder = navigationBar.sortOrder
                    sortMenu.popup(sortMenuButton, sortMenuButton.x, sortMenuButton.y + sortMenuButton.height)
                }
            }

            // Custom content item for now to replicate the look of a button
            // that opens a dropdown menu
            // TODO: Port more fully to Kirigami so we get this for free
            contentItem: RowLayout {
                spacing: Kirigami.Units.smallSpacing

                Kirigami.Icon {
                    Layout.leftMargin: Kirigami.Units.largeSpacing
                    implicitWidth: Kirigami.Units.iconSizes.smallMedium
                    implicitHeight: Kirigami.Units.iconSizes.smallMedium
                    source: 'view-sort'
                    isMask: true
                }

                Label {
                    text: if (sortMenu.sortRoleName !== "") {
                        i18nc("Sort By Menu Title with no sort selected", "Sort: %1", sortMenu.sortRoleName)
                    } else {
                        i18nc("Sort By Menu Title with no sort selected", "Sort")
                    }
                }

                Kirigami.Icon {
                    Layout.leftMargin: Kirigami.Units.smallSpacing
                    Layout.rightMargin: Kirigami.Units.largeSpacing
                    implicitWidth: Kirigami.Units.iconSizes.small
                    implicitHeight: Kirigami.Units.iconSizes.small
                    source: 'arrow-down'
                    isMask: true
                }
            }
        }
    }

    Component {
        id: createRadioButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            objectName: 'createRadioButton'
            text: i18nc("Create a new radio", "Create a radio")
            icon.name: "list-add"
            display: Kirigami.Settings.isMobile && navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
            onClicked: createRadio()
        }
    }
    Component {
        id: enqueueButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            objectName: 'enqueueButton'
            text: i18nc("Add current list to playlist", "Add to Playlist")
            icon.name: "list-add"
            display: Kirigami.Settings.isMobile && navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
            onClicked: enqueue()
        }
    }
    Component {
        id: replaceAndPlayButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            objectName: 'replaceAndPlayButton'
            text: i18n("Play now, replacing current playlist")
            icon.name: "media-playback-start"
            display: Kirigami.Settings.isMobile && navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
            onClicked: replaceAndPlay()
        }
    }
    Component {
        id: showArtistButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            objectName: 'showArtistButton'
            text: i18nc("Button to navigate to the artist of the album", "Display Artist")
            icon.name: "view-media-artist"
            display: Kirigami.Settings.isMobile && navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
            onClicked: if (secondaryTitle) { showArtist(secondaryTitle) }
        }
    }
    Component {
        id: showPlaylistButton
        FlatButtonWithToolTip {
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false
            id: showPlaylistButton
            text: i18nc("show the playlist", "Show Playlist")
            icon.name: "view-media-playlist"
            display: navigationBar.isWidescreen ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
            onClicked: {
                if (navigationBar.isWidescreen) {
                    contentView.showPlaylist = !contentView.showPlaylist;
                } else {
                    playlistDrawer.open();
                }
            }
        }
    }

    // header layout
    ColumnLayout {
        id: layout
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        spacing: 0

        HeaderFooterToolbar {
            id: mainHeader
            toolbarType: Kirigami.Settings.isMobile || filterRow.visible ? HeaderFooterToolbar.ToolbarType.Other
                                                                        : HeaderFooterToolbar.ToolbarType.Header
            Layout.fillWidth: true

            // on mobile, the header is translucent
            color: Kirigami.Settings.isMobile ? Qt.rgba(myPalette.window.r, myPalette.window.g, myPalette.window.b, 0.3) : myPalette.window
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false

            contentItems: [
                FlatButtonWithToolTip {
                    id: showSidebarButton
                    objectName: 'showSidebarButton'
                    visible: Kirigami.Settings.isMobile
                    text: i18nc("open the sidebar", "Open sidebar")
                    icon.name: "open-menu-symbolic"
                    onClicked: mainWindow.globalDrawer.open()
                },
                FlatButtonWithToolTip {
                    id: goPreviousButton
                    objectName: 'goPreviousButton'
                    visible: enableGoBack
                    text: i18nc("navigate back in the views stack", "Back")
                    icon.name: (Qt.application.layoutDirection == Qt.RightToLeft) ? "go-next" : "go-previous"
                    onClicked: goBack()
                },
                Kirigami.Icon {
                    id: mainIcon
                    visible: image.toString().length > 0
                             && !Kirigami.Settings.isMobile // On mobile, we want more header space
                             && navigationBar.enableGoBack // For top-level pages, the icon is redundant
                    source: image

                    Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
                    Layout.preferredWidth: Kirigami.Units.iconSizes.medium
                    Layout.preferredHeight: Kirigami.Units.iconSizes.medium
                },
                ColumnLayout {
                    id: authorAndAlbumLayout
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.leftMargin: mainIcon.visible ? Kirigami.Units.smallSpacing : Kirigami.Units.largeSpacing

                    spacing: 0

                    LabelWithToolTip {
                        id: albumLabel
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.fillWidth: true

                        text: mainTitle
                        level: authorLabel.visible ? 4 : 1
                    }
                    LabelWithToolTip {
                        id: authorLabel
                        Layout.alignment: Qt.AlignLeft | Qt.AlignVCenter
                        Layout.fillWidth: true

                        visible: secondaryTitle !== ""
                        opacity: 0.6

                        text: secondaryTitle
                        elide: Text.ElideRight

                    }
                },
                Loader {
                    sourceComponent: sortMenuComponent
                    active: !Kirigami.Settings.isMobile && enableSorting && !showCreateRadioButton
                    Layout.maximumHeight: parent.height
                    Layout.preferredWidth: item ? item.implicitContentWidth : 0
                },
                Loader {
                    sourceComponent: createRadioButton
                    active: !Kirigami.Settings.isMobile && showCreateRadioButton
                    Layout.maximumHeight: parent.height
                },
                Loader {
                    sourceComponent: enqueueButton
                    active: !Kirigami.Settings.isMobile && !showCreateRadioButton
                    Layout.maximumHeight: parent.height
                },
                Loader {
                    sourceComponent: replaceAndPlayButton
                    active: !Kirigami.Settings.isMobile && !showCreateRadioButton
                    Layout.maximumHeight: parent.height
                },
                Loader {
                    sourceComponent: showArtistButton
                    active: !Kirigami.Settings.isMobile && allowArtistNavigation && !showCreateRadioButton
                    Layout.maximumHeight: parent.height
                },
                FlatButtonWithToolTip {
                    Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
                    Kirigami.Theme.inherit: false
                    objectName: 'showFilterButton'
                    visible: !showCreateRadioButton
                    text: !navigationBar.expandedFilterView ? i18nc("Show filters in the navigation bar", "Search and Filter") : i18nc("Hide filters in the navigation bar", "Hide Search and Filter")
                    icon.name: 'search'
                    checkable: true
                    checked: expandedFilterView
                    onClicked: persistentSettings.expandedFilterView = !persistentSettings.expandedFilterView;
                }
            ]
        }

        // on mobile, move header buttons into a second row (there's limited horizontal space for track names and etc.)
        Loader {
            active: Kirigami.Settings.isMobile
            visible: active

            Layout.fillWidth: true
            Layout.preferredHeight: Kirigami.Units.gridUnit * 2

            sourceComponent: HeaderFooterToolbar {
                id: mobileActions
                toolbarType: Kirigami.Settings.isMobile || filterRow.visible ? HeaderFooterToolbar.ToolbarType.Other : HeaderFooterToolbar.ToolbarType.Header

                // on mobile, the header is translucent
                color: Kirigami.Settings.isMobile ? Qt.rgba(myPalette.window.r, myPalette.window.g, myPalette.window.b, 0.3) : myPalette.window
                Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
                Kirigami.Theme.inherit: false

                contentItems: [
                    Loader {
                        sourceComponent: sortMenuComponent
                        active: enableSorting && !showCreateRadioButton
                        Layout.maximumHeight: parent.height
                        Layout.preferredWidth: item ? item.implicitContentWidth : 0
                    },
                    Item {
                        Layout.fillWidth: true
                    },
                    Loader {
                        sourceComponent: createRadioButton
                        active: showCreateRadioButton
                        Layout.maximumHeight: parent.height
                    },
                    Loader {
                        sourceComponent: enqueueButton
                        active: !showCreateRadioButton
                        Layout.maximumHeight: parent.height
                    },
                    Loader {
                        sourceComponent: replaceAndPlayButton
                        active: !showCreateRadioButton
                        Layout.maximumHeight: parent.height
                    },
                    Loader {
                        sourceComponent: showArtistButton
                        active: allowArtistNavigation && !showCreateRadioButton
                        Layout.maximumHeight: parent.height
                    },
                    Loader {
                        sourceComponent: showPlaylistButton
                        Layout.maximumHeight: parent.height
                    }
                ]
            }
        }

        // filter bar
        HeaderFooterToolbar {
            id: filterRow
            toolbarType: Kirigami.Settings.isMobile ? HeaderFooterToolbar.ToolbarType.Other : HeaderFooterToolbar.ToolbarType.Header

            color: Kirigami.Settings.isMobile ? Qt.rgba(myPalette.window.r, myPalette.window.g, myPalette.window.b, 0.3) : myPalette.window
            Kirigami.Theme.colorSet: Kirigami.Settings.isMobile ? Kirigami.Theme.Complementary : Kirigami.Theme.Window
            Kirigami.Theme.inherit: false

            Layout.fillWidth: true

            visible: opacity > 0.0

            opacity: expandedFilterView ? 1 : 0
            Behavior on opacity {
                NumberAnimation {
                    easing.type: Easing.Linear
                    duration: Kirigami.Units.longDuration
                }
            }
            onVisibleChanged: {
                if (visible) {
                    filterTextInput.forceActiveFocus();
                }
            }

            contentItems: [
                Kirigami.SearchField {
                    id: filterTextInput
                    objectName: 'filterTextInput'

                    Layout.fillWidth: true
                    focusSequence: ""

                    selectByMouse: true

                    Accessible.role: Accessible.EditableText

                    placeholderText: i18n("Search for album name, artist, etc.")

                    Keys.onEscapePressed: persistentSettings.expandedFilterView = false;
                },
                Item {
                    width: Kirigami.Units.largeSpacing
                    visible: showRating && !ElisaApplication.useFavoriteStyleRatings
                },
                LabelWithToolTip {
                    text: i18n("Filter by rating: ")

                    visible: showRating && !ElisaApplication.useFavoriteStyleRatings
                },
                RatingStar {
                    id: ratingFilter
                    objectName: 'ratingFilter'

                    visible: showRating && !ElisaApplication.useFavoriteStyleRatings
                    hoverWidgetOpacity: 1

                    readOnly: false
                },
                Button {
                    visible: showRating && ElisaApplication.useFavoriteStyleRatings

                    text: i18n("Only Show Favorites")
                    icon.name: "rating"
                    checkable: true
                    checked: ratingFilter.starRating == 10
                    onToggled: {
                        if (checked) {
                            ratingFilter.starRating = 10;
                        } else {
                            ratingFilter.starRating = 0;
                        }
                    }
                }
            ]
        }
    }

    // darken background
    Rectangle {
        anchors.fill: background
        visible: Kirigami.Settings.isMobile

        z: -1
        color: "black"
        opacity: 0.8
    }

    // mobile blurred background image
    Loader {
        id: background
        active: Kirigami.Settings.isMobile
        visible: active

        anchors.fill: layout
        z: -1

        sourceComponent: ImageWithFallback {
            source: ElisaApplication.manageHeaderBar.image
            fallback: elisaTheme.defaultBackgroundImage
            asynchronous: true

            fillMode: Image.PreserveAspectCrop

            // make the FastBlur effect more strong
            sourceSize.height: 10

            layer.enabled: true
            layer.effect: HueSaturation {
                cached: true

                lightness: -0.5
                saturation: 0.9

                layer.enabled: true
                layer.effect: FastBlur {
                    cached: true
                    radius: 64
                    transparentBorder: false
                }
            }
        }
    }
}
