/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2015 Taras Kushnir <kushnirTV@gmail.com>
 *
 * Xpiks is distributed under the GNU General Public License, version 3.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.3
import QtQuick.Dialogs 1.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2
import QtQuick.Layouts 1.1
import xpiks 1.0
import "Constants"
import "Constants/Colors.js" as Colors
import "Components"
import "StyledControls"
import "Dialogs"
import "Common.js" as Common

ApplicationWindow {
    id: applicationWindow
    visible: true
    width: 900
    height: 640
    minimumHeight: 640
    minimumWidth: 900
    title: qsTr("Xpiks")

    Component.onCompleted: {
        logsModel.startFlushing()
    }

    onClosing: {
        if (artItemsModel.modifiedArtworksCount > 0) {
            close.accepted = false
            configExitDialog.open()
        }
    }

    function mustUseConfirmation() {
        var mustUse = appSettings.boolValue(appSettings.useConfirmationDialogsKey, true)
        return mustUse
    }

    function openUploadDialog() {
        if (appSettings.boolValue(appSettings.mustUseMasterPasswordKey, false)) {
            var callbackObject = {
                onSuccess: function() { doOpenUploadDialog(true) },
                onFail: function() { doOpenUploadDialog(false) }
            }

            Common.launchComponent("Dialogs/EnterMasterPasswordDialog.qml",
                         applicationWindow,
                         {componentParent: applicationWindow, callbackObject: callbackObject})
        } else {
            doOpenUploadDialog(true)
        }
    }

    function doOpenUploadDialog(masterPasswordCorrectOrEmpty) {
        artworkUploader.resetModel()
        artItemsModel.setSelectedForUpload()
        uploadInfos.initializeAccounts(masterPasswordCorrectOrEmpty)

        Common.launchComponent("Dialogs/UploadArtworks.qml",
                     applicationWindow,
                     {componentParent: applicationWindow})
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            MenuItem {
                text: qsTr("&Settings")
                onTriggered: {
                    settingsModel.readAllValues()
                    Common.launchComponent("Dialogs/SettingsWindow.qml",
                                    applicationWindow, {},
                                    function(wnd) {wnd.show();});
                }
            }
            MenuItem {
                text: qsTr("&About")
                onTriggered: {
                    Common.launchComponent("Dialogs/AboutWindow.qml",
                                    applicationWindow, {},
                                    function(wnd) {wnd.show();});
                }
            }

            MenuItem {
                text: qsTr("Exit")
                onTriggered: Qt.quit();
            }
        }

        Menu {
            title: qsTr("Tools")

            MenuItem {
                text: qsTr("&Zip selected artworks")
                enabled: artItemsModel.selectedArtworksCount > 0
                onTriggered: {
                    console.log("Zip archives triggered")

                    artItemsModel.setSelectedForZipping()
                    Common.launchComponent("Dialogs/ZipArtworksDialog.qml",
                                    applicationWindow,
                                    {componentParent: applicationWindow});
                }
            }
        }
    }

    MessageDialog {
        id: configExitDialog

        title: "Confirmation"
        text: qsTr("You have some artworks modified. Really exit?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            Qt.quit()
        }
    }

    MessageDialog {
        id: confirmRemoveSelectedDialog
        property int itemsCount
        title: "Confirmation"
        text: qsTr("Are you sure you want to remove %1 item(s)?").arg(itemsCount)
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            doRemoveSelectedArtworks()
        }
    }

    function doRemoveSelectedArtworks() {
        artItemsModel.removeSelectedArtworks()
        artItemsModel.checkForWarnings()
    }

    function tryUploadArtworks() {
        if (artItemsModel.areSelectedArtworksSaved()) {
            openUploadDialog()
        } else {
            mustSaveWarning.open()
        }
    }

    MessageDialog {
        id: confirmRemoveDirectoryDialog
        property int directoryIndex
        title: "Confirmation"
        text: qsTr("Are you sure you want to remove this directory?")
        standardButtons: StandardButton.Yes | StandardButton.No
        onYes: {
            artItemsModel.removeArtworksDirectory(directoryIndex)
            artItemsModel.checkForWarnings()
        }
    }

    FileDialog {
        id: chooseArtworksDialog
        title: "Please choose artworks"
        selectExisting: true
        selectMultiple: true
        nameFilters: [ "Jpeg images (*.jpg), All files (*)" ]

        onAccepted: {
            console.log("You chose: " + chooseArtworksDialog.fileUrls)
            var filesAdded = artItemsModel.addLocalArtworks(chooseArtworksDialog.fileUrls)
            console.log(filesAdded + ' files via Open File(s)')
        }

        onRejected: {
            console.log("Artworks dialog canceled")
        }
    }

    FileDialog {
        id: chooseDirectoryDialog
        title: "Please choose artworks"
        selectExisting: true
        selectMultiple: false
        selectFolder: true

        onAccepted: {
            console.log("You chose: " + chooseDirectoryDialog.fileUrl)
            var filesAdded = artItemsModel.addLocalDirectory(chooseDirectoryDialog.fileUrl)
            console.log(filesAdded + ' files via Open Directory')
        }

        onRejected: {
            console.log("Directory dialog canceled")
        }
    }

    MessageDialog {
        id: mustSaveWarning
        title: "Warning"
        text: qsTr("Please, save selected items before upload")
    }

    MessageDialog {
        id: mustSelectDialog
        title: "Warning"
        text: qsTr("Please, select some artworks first")
    }

    MessageDialog {
        id: alreadySavedDialog
        title: "Information"
        text: qsTr("All selected items are already saved")
    }

    Connections {
        target: artItemsModel
        onArtworksAdded: {
           if (count > 0) {
               Common.launchComponent("Dialogs/ImportMetadata.qml", applicationWindow, {})
           }
        }
    }

    Rectangle {
        color: Colors.defaultDarkColor
        anchors.fill: parent

        DropArea {
            anchors.fill: parent
            onDropped: {
                if (drop.hasUrls) {
                    var filesCount = artItemsModel.dropFiles(drop.urls)
                    console.log(filesCount + ' files drag&dropped')
                }
            }
        }

        RowLayout {
            id: mainGrid
            anchors.fill: parent
            spacing: 0

            ColumnLayout {
                width: 250
                Layout.fillHeight: true

                spacing: 0

                Rectangle {
                    width: parent.width
                    height: 45
                    color: Colors.defaultDarkColor

                    RowLayout {
                        spacing: 10
                        anchors.fill: parent
                        anchors.margins: { top: 10; left: 10 }

                        StyledButton {
                            text: qsTr("Add directory")
                            width: 120
                            onClicked: chooseDirectoryDialog.open()
                        }

                        StyledButton {
                            text: qsTr("Add files")
                            width: 100
                            onClicked: chooseArtworksDialog.open()
                        }

                        Item {
                            Layout.fillWidth: true
                        }
                    }
                }

                Rectangle {
                    Layout.fillHeight: true
                    width: 250

                    color: Colors.defaultControlColor

                    ListView {
                        id: sourcesListView
                        model: artworkRepository
                        boundsBehavior: Flickable.StopAtBounds
                        anchors.fill: parent
                        anchors.margins: { left: 10; top: 5; right: 10 }

                        spacing: 10

                        displaced: Transition {
                            NumberAnimation { properties: "x,y"; duration: 230 }
                        }

                        addDisplaced: Transition {
                            NumberAnimation { properties: "x,y"; duration: 230 }
                        }

                        removeDisplaced: Transition {
                            NumberAnimation { properties: "x,y"; duration: 230 }
                        }

                        delegate: Rectangle {
                            id: sourceWrapper
                            property int delegateIndex: index
                            color: isselected ? Colors.itemsSourceSelected : Colors.itemsSourceBackground
                            width: parent.width
                            height: 31
                            Layout.minimumWidth: 237

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    artItemsModel.selectDirectory(sourceWrapper.delegateIndex)
                                }
                            }

                            RowLayout {
                                spacing: 10
                                anchors.fill: parent

                                Item {
                                    id: placeholder1
                                    width: 1
                                }

                                StyledText {
                                    id: directoryPath
                                    Layout.fillWidth: true
                                    anchors.verticalCenter: parent.verticalCenter
                                    height: 31
                                    color: Colors.itemsSourceForeground
                                    text: path + " (" + usedimagescount + ")"
                                    elide: Text.ElideMiddle
                                    font.bold: true
                                }

                                CloseIcon {
                                    width: 14
                                    height: 14
                                    anchors.verticalCenter: parent.verticalCenter
                                    isActive: false

                                    onItemClicked: {
                                        if (mustUseConfirmation()) {
                                            confirmRemoveDirectoryDialog.directoryIndex = sourceWrapper.delegateIndex
                                            confirmRemoveDirectoryDialog.open()
                                        } else {
                                            artItemsModel.removeArtworksDirectory(sourceWrapper.delegateIndex)
                                            artItemsModel.checkForWarnings()
                                        }
                                    }
                                }

                                Item {
                                    id: placeholder2
                                    width: 1
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                height: parent.height
                width: 2
                color: Colors.defaultDarkColor
            }

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 0

                Rectangle {
                    height: 45
                    color: Colors.defaultDarkColor
                    z: 10000
                    Layout.fillWidth: true

                    RowLayout {
                        spacing: 10
                        anchors.fill: parent
                        anchors.margins: { top: 10; left: 10 }
                        anchors.rightMargin: mainScrollView.flickableItem.contentHeight > mainScrollView.flickableItem.height ? 20 : 10

                        Item {
                            width: 1
                        }

                        StyledCheckbox {
                            id: selectAllCheckbox
                            enabled: artworkRepository.artworksSourcesCount > 0
                            text: artItemsModel.selectedArtworksCount == 0 ? qsTr("Select all") : qsTr("Select none")
                            checked: artItemsModel.selectedArtworksCount > 0

                            onClicked: {
                                if (checked) {
                                    artItemsModel.selectAllArtworks();
                                }
                                else {
                                    artItemsModel.unselectAllArtworks();
                                }
                            }

                            Connections {
                                target: artItemsModel
                                onSelectedArtworksCountChanged: {
                                    selectAllCheckbox.checked = artItemsModel.selectedArtworksCount > 0
                                }
                            }
                        }

                        Item {
                            Layout.fillWidth: true
                        }

                        StyledButton {
                            text: qsTr("Remove")
                            enabled: artworkRepository.artworksSourcesCount > 0
                            width: 80
                            onClicked: {
                                if (artItemsModel.selectedArtworksCount == 0) {
                                    mustSelectDialog.open()
                                }
                                else {
                                    var itemsCount = artItemsModel.selectedArtworksCount
                                    if (itemsCount > 0) {
                                        if (mustUseConfirmation()) {
                                            confirmRemoveSelectedDialog.itemsCount = itemsCount
                                            confirmRemoveSelectedDialog.open()
                                        } else {
                                            doRemoveSelectedArtworks()
                                        }
                                    }
                                }
                            }
                        }

                        StyledButton {
                            text: qsTr("Edit")
                            width: mainScrollView.flickableItem.contentHeight > mainScrollView.flickableItem.height ? 72 : 82
                            enabled: artworkRepository.artworksSourcesCount > 0
                            onClicked: {
                                if (artItemsModel.selectedArtworksCount == 0) {
                                    mustSelectDialog.open()
                                }
                                else {
                                    if (artItemsModel.selectedArtworksCount > 0) {
                                        combinedArtworks.resetModelData();
                                        artItemsModel.combineSelectedArtworks();
                                        Common.launchComponent("Dialogs/CombinedArtworksDialog.qml", applicationWindow, {});
                                    }
                                }
                            }
                        }

                        StyledButton {
                            text: qsTr("Save")
                            width: 80
                            enabled: artworkRepository.artworksSourcesCount > 0
                            onClicked: {
                                if (artItemsModel.selectedArtworksCount == 0) {
                                    mustSelectDialog.open()
                                }
                                else {
                                    if (artItemsModel.selectedArtworksCount > 0 &&
                                            artItemsModel.modifiedArtworksCount > 0) {
                                        iptcProvider.resetModel()
                                        artItemsModel.patchSelectedArtworks()
                                        Common.launchComponent("Dialogs/ExportMetadata.qml", applicationWindow, {})
                                    } else {
                                        if (artItemsModel.modifiedArtworksCount == 0) {
                                            alreadySavedDialog.open()
                                        }
                                    }
                                }
                            }
                        }

                        StyledButton {
                            text: qsTr("Upload")
                            width: 90
                            enabled: artworkRepository.artworksSourcesCount > 0
                            onClicked: {
                                if (artItemsModel.selectedArtworksCount == 0) {
                                    artItemsModel.selectAllArtworks();
                                }

                                tryUploadArtworks();
                            }
                        }

                        Item {
                            width: 10
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Colors.defaultControlColor

                    Item {
                        anchors.topMargin: 4
                        anchors.fill: parent

                        Rectangle {
                            id: undoRedoRect
                            color: Colors.defaultDarkColor
                            width: parent.width
                            height: 0
                            opacity: 0

                            states: [
                                State {
                                    name: "canundo"
                                    when: undoRedoManager.canUndo
                                    PropertyChanges {
                                        target: undoRedoRect
                                        height: 50
                                    }
                                }
                            ]

                            transitions: [
                                Transition {
                                    from: ""; to: "canundo"
                                    ParallelAnimation {
                                        NumberAnimation {
                                            target: undoRedoRect
                                            properties: "height";
                                            from: 0
                                            to: 50
                                            easing.type: "InOutQuad";
                                            duration: 250
                                        }

                                        NumberAnimation {
                                            target: undoRedoRect
                                            properties: "opacity";
                                            from: 0
                                            to: 1
                                            easing.type: "InOutQuad";
                                            duration: 250
                                        }
                                    }
                                },
                                Transition {
                                    from: "canundo"; to: ""
                                    ParallelAnimation {
                                        NumberAnimation {
                                            target: undoRedoRect
                                            properties: "height";
                                            from: 50
                                            to: 0
                                            easing.type: "InOutQuad";
                                            duration: 250
                                        }

                                        NumberAnimation {
                                            target: undoRedoRect
                                            properties: "opacity";
                                            from: 1
                                            to: 0
                                            easing.type: "InOutQuad";
                                            duration: 250
                                        }
                                    }
                                }

                            ]

                            RowLayout {
                                anchors.fill: parent
                                spacing: 10

                                Rectangle {
                                    color: Colors.artworkSavedColor
                                    width: 6
                                    Layout.fillHeight: true
                                }

                                Item {
                                    Layout.fillWidth: true
                                }

                                StyledText {
                                    id: undoDescription
                                    text: undoRedoManager.undoDescription
                                }

                                Item {
                                    width: 10
                                }

                                StyledText {
                                    text: qsTr("Undo")
                                    color: undoMA.pressed ? Colors.defaultLightColor : Colors.artworkActiveColor

                                    MouseArea {
                                        id: undoMA
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            undoRedoManager.undoLastAction()
                                        }
                                    }
                                }

                                StyledText {
                                    text: qsTr("Dismiss (%1)").arg(10 - (autoDismissTimer.iterations % 11))
                                    color: dismissUndoMA.pressed ? Colors.defaultLightColor : Colors.defaultInputBackground

                                    MouseArea {
                                        id: dismissUndoMA
                                        anchors.fill: parent
                                        cursorShape: Qt.PointingHandCursor
                                        onClicked: {
                                            undoRedoManager.discardLastAction()
                                        }
                                    }
                                }

                                Item {
                                    Layout.fillWidth: true
                                }

                                Timer {
                                    id: autoDismissTimer
                                    property int iterations: 0
                                    interval: 1000
                                    repeat: true
                                    running: undoRedoManager.canUndo
                                    onTriggered: {
                                        iterations += 1

                                        if (iterations % 11 === 10) {
                                            undoRedoManager.discardLastAction()
                                            iterations = 0
                                        }
                                    }
                                }

                                Connections {
                                    target: undoRedoManager
                                    onItemRecorded: {
                                        autoDismissTimer.iterations = 0
                                    }
                                }
                            }
                        }

                        StyledScrollView {
                            id: mainScrollView
                            width: parent.width
                            anchors.topMargin: 4
                            anchors.top: undoRedoRect.bottom
                            anchors.bottom: parent.bottom
                            //Layout.fillWidth: true
                            //Layout.fillHeight: true
                            // does not work for now in Qt 5.4.1 in combination with ListView
                            //verticalScrollBarPolicy: Qt.ScrollBarAlwaysOn

                            ListView {
                                id: imagesListView
                                model: artItemsModel
                                boundsBehavior: Flickable.StopAtBounds
                                spacing: 4

                                add: Transition {
                                    NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 230 }
                                }

                                remove: Transition {
                                    NumberAnimation { property: "opacity"; to: 0; duration: 230 }
                                }

                                displaced: Transition {
                                    NumberAnimation { properties: "x,y"; duration: 230 }
                                }

                                addDisplaced: Transition {
                                    NumberAnimation { properties: "x,y"; duration: 230 }
                                }

                                removeDisplaced: Transition {
                                    NumberAnimation { properties: "x,y"; duration: 230 }
                                }

                                delegate: Rectangle {
                                    id: rowWrapper
                                    property bool isHighlighted: (isselected || descriptionTextInput.activeFocus || flv.isFocused)
                                    color: isHighlighted ? Colors.selectedArtworkColor : Colors.artworkImageBackground
                                    property int delegateIndex: index
                                    property variant artworkModel: model

                                    width: parent.width
                                    height: 200

                                    RowLayout {
                                        anchors.fill: parent
                                        anchors.rightMargin: 10
                                        spacing: 5

                                        Rectangle {
                                            id: isModifiedRectangle
                                            color: ismodified ? Colors.artworkModifiedColor : Colors.artworkSavedColor
                                            width: 6
                                            Layout.fillHeight: true
                                        }

                                        Item {
                                            width: 5
                                        }

                                        StyledCheckbox {
                                            id: itemCheckedCheckbox
                                            //checked: isselected
                                            onClicked: editisselected = checked
                                            Component.onCompleted: itemCheckedCheckbox.checked = isselected
                                            Connections {
                                                target: artItemsModel
                                                onSelectedArtworksCountChanged: {
                                                    itemCheckedCheckbox.checked = rowWrapper.artworkModel.isselected || false
                                                }
                                            }
                                        }

                                        Rectangle {
                                            width: 180
                                            height: parent.height
                                            color: "transparent"

                                            ColumnLayout {
                                                anchors.fill: parent
                                                anchors.margins: { left: 15; right: 15 }
                                                spacing: 5

                                                Item {
                                                    height: 15
                                                }

                                                Rectangle {
                                                    width: 150
                                                    height: 130
                                                    anchors.horizontalCenter: parent.horizontalCenter
                                                    color: "transparent"
                                                    Image {
                                                        anchors.fill: parent
                                                        source: "image://global/" + filename
                                                        sourceSize.width: 150
                                                        sourceSize.height: 150
                                                        fillMode: Image.PreserveAspectCrop
                                                        asynchronous: true
                                                    }

                                                    MouseArea {
                                                        anchors.fill: parent
                                                        onClicked: editisselected = !isselected
                                                        onDoubleClicked: {
                                                            combinedArtworks.resetModelData();
                                                            artItemsModel.combineArtwork(rowWrapper.delegateIndex);
                                                            Common.launchComponent("Dialogs/CombinedArtworksDialog.qml", applicationWindow, {});
                                                        }
                                                    }
                                                }

                                                StyledText {
                                                    Layout.fillWidth: true
                                                    elide: Text.ElideMiddle
                                                    color: Colors.defaultInputBackground
                                                    horizontalAlignment: Text.AlignHCenter
                                                    text: filename.split(/[\\/]/).pop()
                                                }

                                                Item {
                                                    Layout.fillHeight: true
                                                }
                                            }
                                        }

                                        Rectangle {
                                            id: columnRectangle
                                            height: parent.height
                                            Layout.fillWidth: true
                                            color: rowWrapper.isHighlighted  ? Colors.selectedMetadataColor : Colors.artworkBackground

                                            ColumnLayout {
                                                id: columnLayout
                                                spacing: 3
                                                anchors.fill: parent
                                                anchors.margins: { left: 20; right: 20 }

                                                StyledText {
                                                    text: qsTr("Description:")
                                                    anchors.left: parent.left
                                                }

                                                Rectangle {
                                                    id: rect
                                                    Layout.fillWidth: true
                                                    height: 30
                                                    color: rowWrapper.isHighlighted ? Colors.defaultInputBackground : Colors.defaultControlColor
                                                    border.color: Colors.artworkActiveColor
                                                    border.width: descriptionTextInput.activeFocus ? 1 : 0

                                                    StyledTextInput {
                                                        id: descriptionTextInput
                                                        height: 30
                                                        anchors.left: parent.left
                                                        anchors.right: parent.right
                                                        anchors.leftMargin: 5
                                                        anchors.rightMargin: 5

                                                        anchors.verticalCenter: parent.verticalCenter
                                                        maximumLength: 250
                                                        text: description
                                                        color: rowWrapper.isHighlighted ? Colors.defaultLightColor : Colors.artworkActiveColor
                                                        onTextChanged: model.editdescription = text
                                                        Keys.onTabPressed: {
                                                            flv.activateEdit()
                                                        }

                                                        validator: RegExpValidator {
                                                            regExp: /[a-zA-Z0-9 !@#$%^&*()+="'|-,]*/
                                                        }

                                                        Keys.onPressed: {
                                                            if(event.matches(StandardKey.Paste)) {
                                                                var clipboardText = clipboard.getText();
                                                                // same regexp as in validator
                                                                var sanitizedText = clipboardText.replace(/[^a-zA-Z0-9 !@#$%^&*()+="'|-,]*/g, '');
                                                                descriptionTextInput.paste(sanitizedText)
                                                                event.accepted = true
                                                            }
                                                        }
                                                    }
                                                }

                                                Item {
                                                    height: 1
                                                }

                                                RowLayout {
                                                    anchors.left: parent.left
                                                    spacing: 5

                                                    StyledText {
                                                        id: keywordsLabel
                                                        text: qsTr("Keywords:")
                                                    }

                                                    StyledText {
                                                        text: qsTr("(comma-separated)")
                                                        visible: rowWrapper.isHighlighted
                                                        color: Colors.defaultInputBackground
                                                    }
                                                }

                                                Rectangle {
                                                    id: keywordsWrapper
                                                    Layout.fillWidth: true
                                                    height: 80
                                                    anchors.rightMargin: 20
                                                    border.color: Colors.artworkActiveColor
                                                    border.width: flv.isFocused ? 1 : 0
                                                    color: rowWrapper.isHighlighted ? Colors.defaultInputBackground : Colors.defaultControlColor


                                                    function removeKeyword(index) {
                                                        artItemsModel.removeKeywordAt(rowWrapper.delegateIndex, index)
                                                    }

                                                    function removeLastKeyword() {
                                                        artItemsModel.removeLastKeyword(rowWrapper.delegateIndex)
                                                    }

                                                    function appendKeyword(keyword) {
                                                        artItemsModel.appendKeyword(rowWrapper.delegateIndex, keyword)
                                                    }

                                                    function pasteKeywords(keywords) {
                                                        artItemsModel.pasteKeywords(rowWrapper.delegateIndex, keywords)
                                                    }

                                                    function saveKeywords() {
                                                        artItemsModel.backupItem(rowWrapper.delegateIndex)
                                                    }

                                                    MouseArea {
                                                        anchors.fill: parent
                                                        propagateComposedEvents: true
                                                        onClicked: {
                                                            flv.activateEdit()
                                                            mouse.accepted = false
                                                        }
                                                    }

                                                    StyledScrollView {
                                                        id: scroller
                                                        height: parent.height
                                                        width: parent.width + 15
                                                        highlightOnFocus: true

                                                        EditableTags {
                                                            id: flv
                                                            model: artItemsModel.getArtworkItself(rowWrapper.delegateIndex)
                                                            anchors.margins: { left: 5; top: 5; right: 0; bottom: 5 }

                                                            delegate: Rectangle {
                                                                id: itemWrapper
                                                                property int delegateIndex: index
                                                                color: rowWrapper.isHighlighted ? Colors.defaultLightColor : Colors.artworkActiveColor

                                                                width: childrenRect.width
                                                                height: childrenRect.height

                                                                RowLayout {
                                                                    spacing: 1

                                                                    Rectangle {
                                                                        id: tagTextRect
                                                                        width: childrenRect.width + 5
                                                                        height: 20
                                                                        color: "transparent"

                                                                        StyledText {
                                                                            anchors.left: parent.left
                                                                            anchors.leftMargin: 5
                                                                            anchors.top: parent.top
                                                                            anchors.bottom: parent.bottom
                                                                            verticalAlignment: Text.AlignVCenter
                                                                            text: keyword
                                                                            color: rowWrapper.isHighlighted ? Colors.defaultControlColor : Colors.defaultLightColor
                                                                        }
                                                                    }

                                                                    CloseIcon {
                                                                        width: 14
                                                                        height: 14
                                                                        isActive: rowWrapper.isHighlighted
                                                                        anchors.verticalCenter: tagTextRect.verticalCenter
                                                                        onItemClicked: keywordsWrapper.removeKeyword(itemWrapper.delegateIndex)
                                                                    }

                                                                    Item {
                                                                        width: 1
                                                                    }
                                                                }
                                                            }

                                                            onTagAdded: {
                                                                keywordsWrapper.appendKeyword(text)
                                                            }

                                                            onTagsPasted: {
                                                                keywordsWrapper.pasteKeywords(tagsList)
                                                            }

                                                            onRemoveLast: {
                                                                keywordsWrapper.removeLastKeyword()
                                                            }

                                                            onFocusLost: keywordsWrapper.saveKeywords()
                                                        }
                                                    }
                                                }

                                                RowLayout {
                                                    spacing: 15

                                                    StyledText {
                                                        text: keywordscount
                                                        color: rowWrapper.isHighlighted ? Colors.defaultControlColor : Colors.selectedArtworkColor
                                                    }

                                                    Item {
                                                        Layout.fillWidth: true
                                                    }

                                                    StyledText {
                                                        text: qsTr("Suggest keywords")
                                                        color: suggestKeywordsMA.pressed ? Colors.defaultLightColor : Colors.artworkActiveColor

                                                        MouseArea {
                                                            id: suggestKeywordsMA
                                                            anchors.fill: parent
                                                            cursorShape: Qt.PointingHandCursor
                                                            onClicked: {
                                                                var callbackObject = {
                                                                    promoteKeywords: function(keywords) {
                                                                        artItemsModel.pasteKeywords(rowWrapper.delegateIndex, keywords)
                                                                    }
                                                                }

                                                                Common.launchComponent("Dialogs/KeywordsSuggestion.qml",
                                                                                       applicationWindow,
                                                                                       {callbackObject: callbackObject});
                                                            }
                                                        }
                                                    }

                                                    StyledText {
                                                        text: qsTr("More Edits")
                                                        color: moreEditsMA.pressed ? Colors.defaultLightColor : Colors.artworkActiveColor

                                                        MouseArea {
                                                            id: moreEditsMA
                                                            anchors.fill: parent
                                                            cursorShape: Qt.PointingHandCursor
                                                            onClicked: {
                                                                combinedArtworks.resetModelData();
                                                                artItemsModel.combineArtwork(rowWrapper.delegateIndex);
                                                                Common.launchComponent("Dialogs/CombinedArtworksDialog.qml", applicationWindow, {});
                                                            }
                                                        }
                                                    }

                                                    StyledText {
                                                        text: qsTr("Copy keywords")
                                                        color: copyKeywordsMA.pressed ? Colors.defaultLightColor : Colors.artworkActiveColor

                                                        MouseArea {
                                                            id: copyKeywordsMA
                                                            anchors.fill: parent
                                                            cursorShape: Qt.PointingHandCursor
                                                            onClicked: clipboard.setText(keywordsstring)
                                                        }
                                                    }
                                                }

                                                Item {
                                                    height: 1
                                                }
                                            }
                                        }
                                    }
                                }

                                Connections {
                                    target: artItemsModel
                                    onArtworksChanged: {
                                        console.log("Force layout for artworks list view")
                                        imagesListView.forceLayout()
                                        imagesListView.update()
                                    }
                                }
                            }
                        }
                    }
                }

                ClipboardHelper {
                    id: clipboard
                }
            }
        }
    }

    statusBar: StatusBar {
        RowLayout {
            width: parent.width
            height: 20
            implicitHeight: 20
            spacing: 5

            Item {
                width: 20
            }

            StyledText {
                text: qsTr("Show logs")
                color: logsMA.pressed ? Colors.defaultInputBackground : Colors.selectedMetadataColor

                MouseArea {
                    id: logsMA
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        Common.launchComponent("Dialogs/LogsDialog.qml",
                                        applicationWindow,
                                        { logText: logsModel.getAllLogsText() });
                    }
                }
            }

            StyledText {
                text: "|"
                color: Colors.selectedMetadataColor
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                text: qsTr("Check warnings")
                color: warningsMA.pressed ? Colors.defaultInputBackground : warningsManager.warningsCount > 0 ? Colors.artworkModifiedColor : Colors.selectedMetadataColor

                MouseArea {
                    id: warningsMA
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        artItemsModel.checkForWarnings()
                        Common.launchComponent("Dialogs/WarningsDialog.qml", applicationWindow, {});
                    }
                }
            }

            Item {
                Layout.fillWidth: true
            }

            StyledText {
                text: artItemsModel.selectedArtworksCount > 0 ? qsTr("%1 selected item(s)").arg(artItemsModel.selectedArtworksCount) : qsTr("No selected items")
                color: Colors.selectedMetadataColor
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                text: "|"
                color: Colors.selectedMetadataColor
                verticalAlignment: Text.AlignVCenter
            }

            StyledText {
                text: artItemsModel.modifiedArtworksCount > 0 ? qsTr("%1 modified item(s)").arg(artItemsModel.modifiedArtworksCount) : qsTr("No modified items")
                verticalAlignment: Text.AlignVCenter
                color: artItemsModel.modifiedArtworksCount > 0 ? Colors.artworkModifiedColor : Colors.selectedMetadataColor
            }

            Item {
                width: 20
            }
        }

        style: StatusBarStyle {
            background: Rectangle {
                implicitHeight: 20
                implicitWidth: 200
                color: Colors.defaultDarkColor
            }
        }
    }
}