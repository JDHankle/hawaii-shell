/****************************************************************************
 * This file is part of Hawaii.
 *
 * Copyright (C) 2014-2016 Pier Luigi Fiorini
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

import QtQuick 2.2
import QtQuick.Layouts 1.0
import Qt.labs.controls 1.0 as LabsControls
import Qt.labs.calendar 1.0 as LabsCalendar
import Hawaii.Themes 1.0 as Themes
import Fluid.Ui 1.0 as FluidUi

Item {
    property alias month: calendar.month
    property alias year: calendar.year

    height: FluidUi.Units.gu(20)

    Component {
        id: hourDelegate

        Item {
            height: FluidUi.Units.gu(2)

            LabsControls.Label {
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: FluidUi.Units.smallSpacing
                font: Themes.Theme.smallestFont
                text: modelData + ":00"
            }

            Rectangle {
                anchors.bottom: parent.bottom
                color: Themes.Theme.palette.panel.textColor
                opacity: 0.4
                height: 1
            }
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: FluidUi.Units.smallSpacing
        spacing: FluidUi.Units.largeSpacing

        GridLayout {
            columns: 2

            LabsCalendar.DayOfWeekRow {
                id: dayOfWeek
                locale: calendar.locale
                delegate: Text {
                    text: model.shortName
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font: dayOfWeek.font
                    color: Themes.Theme.palette.panel.textColor
                }

                Layout.column: 1
                Layout.fillWidth: true
            }

            LabsCalendar.WeekNumberColumn {
                id: weekNumber
                month: calendar.month
                year: calendar.year
                locale: calendar.locale
                font.bold: true
                delegate: Text {
                    text: model.weekNumber
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font: weekNumber.font
                    color: Themes.Theme.palette.panel.textColor
                }

                Layout.fillHeight: true
            }

            LabsCalendar.MonthGrid {
                id: calendar
                delegate: Text {
                    text: model.day
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font: calendar.font
                    color: Themes.Theme.palette.panel.textColor
                    opacity: model.month === calendar.month ? 1 : 0
                }

                Layout.fillWidth: true
                Layout.fillHeight: true
            }

            Layout.fillHeight: true
        }

        ListView {
            model: 25
            delegate: hourDelegate

            //LabsControls.ScrollBar.horizontal: LabsControls.ScrollBar {}
            //LabsControls.ScrollBar.vertical: LabsControls.ScrollBar {}

            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
