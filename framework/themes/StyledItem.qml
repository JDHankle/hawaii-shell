/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2013-2015 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *
 * $BEGIN_LICENSE:LGPL2.1+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

import QtQuick 2.0

/*!
    \qmltype StyledItem
    \qmlabstract
    \inqmlmodule Hawaii.Themes 1.0
*/

Item {
    id: root

    /*! \qmlproperty Component StyledItem::style
        The style Component for this item.
        \sa {Hawaii Themes QML Types}
    */
    property Component style

    /*! \internal */
    readonly property Item __styleInstance: styleLoader.status == Loader.Ready ? styleLoader.item : null

    implicitWidth: __styleInstance ? __styleInstance.implicitWidth : 0
    implicitHeight: __styleInstance ? __styleInstance.implicitHeight : 0

    Loader {
        id: styleLoader
        anchors.fill: parent
        sourceComponent: style
        onStatusChanged: {
            if (status === Loader.Error)
                console.error("Failed to load style for", root);
        }
    }
}
