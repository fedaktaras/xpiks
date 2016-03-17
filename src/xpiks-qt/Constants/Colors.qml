pragma Singleton
/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2016 Taras Kushnir <kushnirTV@gmail.com>
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
import QtQuick 2.0

QtObject {
    property color defaultDarkColor: "#1e1e1e";
    property color defaultControlColor: "#292929";

    property color defaultLightColor: "#E0E0E0";
    property color defaultLightGrayColor: "#D0D0D0";

    property color itemsSourceBackground: "#232323";
    property color itemsSourceSelected: "#333333";
    property color itemsSourceForeground: "#e6e6e6";

    property color buttonHoverBackground: "#12b9bc";
    property color buttonPressedBackground: "#41b1b7";
    property color buttonDisabledForeground: "#283c3f";
    property color buttonPressedForeground: "#283c3f";

    property color artworkBackground: "#031619";
    property color artworkImageBackground: "#071215";
    property color artworkModifiedColor: "#f49c12";
    property color artworkSavedColor: "#435151";
    property color artworkActiveColor: "#12b9bc"

    property color selectedArtworkColor: "#545456";
    property color selectedMetadataColor: "#5d5d5d";
    property color checkboxCheckedColor: "#ffffff";
    property color defaultInputBackground: "#999999";

    property color destructiveColor: "#d10b0b";
    property color greenColor: "#2daf02";
    property color whiteColor: "#ffffff";

    function initTheme(theme) {
        defaultDarkColor = theme.defaultDarkColor
        defaultControlColor = theme.defaultControlColor

        defaultLightColor = theme.defaultLightColor
        defaultLightGrayColor = theme.defaultLightGrayColor

        itemsSourceBackground = theme.itemsSourceBackground
        itemsSourceSelected = theme.itemsSourceSelected
        itemsSourceForeground = theme.itemsSourceForeground

        buttonHoverBackground = theme.buttonHoverBackground
        buttonPressedBackground = theme.buttonPressedBackground
        buttonDisabledForeground = theme.buttonDisabledForeground
        buttonPressedForeground = theme.buttonPressedForeground

        artworkBackground = theme.artworkBackground
        artworkImageBackground = theme.artworkImageBackground
        artworkModifiedColor = theme.artworkModifiedColor
        artworkSavedColor = theme.artworkSavedColor
        artworkActiveColor = theme.artworkActiveColor

        selectedArtworkColor = theme.selectedArtworkColor
        selectedMetadataColor = theme.selectedMetadataColor
        checkboxCheckedColor = theme.checkboxCheckedColor
        defaultInputBackground = theme.defaultInputBackground

        destructiveColor = theme.destructiveColor
        greenColor = theme.greenColor
        whiteColor = theme.whiteColor
    }
}