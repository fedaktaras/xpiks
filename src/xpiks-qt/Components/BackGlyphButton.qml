/*
 * This file is a part of Xpiks - cross platform application for
 * keywording and uploading images for microstocks
 * Copyright (C) 2014-2017 Taras Kushnir <kushnirTV@gmail.com>
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

import QtQuick 2.2
import QtQuick.Controls 1.1
import xpiks 1.0
import "../Constants"
import "../StyledControls"

GlyphButton {
    id: buttonBack
    width: 100
    glyphVisible: true
    leftShift: -5
    glyphMargin: 12
    glyph: Item {
        width: childrenRect.width
        height: 14

        TriangleElement {
            id: triangle
            isVertical: true
            isFlipped: true
            width: 7
            height: 14
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            color: buttonBack.textColor
        }

        Rectangle {
            anchors.left: triangle.right
            anchors.verticalCenter: parent.verticalCenter
            color: buttonBack.textColor
            width: 6
            height: 6
        }
    }
}
