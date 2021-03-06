/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2015-2016 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0
import QtQuick.Window 2.2
import GreenIsland 1.0 as GreenIsland

GreenIsland.WaylandOutput {
    id: output
    manufacturer: nativeScreen.manufacturer
    model: nativeScreen.model
    position: nativeScreen.position
    physicalSize: nativeScreen.physicalSize
    subpixel: nativeScreen.subpixel
    transform: nativeScreen.transform
    scaleFactor: nativeScreen.scaleFactor
    sizeFollowsWindow: true
    window: Window {
        id: window
        x: nativeScreen.position.x
        y: nativeScreen.position.y
        width: nativeScreen.size.width
        height: nativeScreen.size.height
        flags: Qt.FramelessWindowHint

        GreenIsland.LocalPointerTracker {
            id: localPointerTracker
            anchors.fill: parent
            globalTracker: globalPointerTracker

            ErrorScreen {
                anchors.fill: parent
            }

            GreenIsland.PointerItem {
                id: cursor
                inputDevice: output.compositor.defaultInputDevice
                x: localPointerTracker.mouseX - hotspotX
                y: localPointerTracker.mouseY - hotspotY
                visible: globalPointerTracker.output === output
            }
        }
    }
}
