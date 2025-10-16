import QtQuick

import TableModel

Window {

    SystemPalette { id: appPalette; colorGroup: SystemPalette.Active }

    width: 640
    height: 480
    visible: true
    title: qsTr("USBGuard GUI Dashboard")

    // Set the Window's background color to the system's window color.
    // This will correctly switch between white (light mode) and dark grey (dark mode).
    color: appPalette.window

    TableView {
        anchors.fill: parent
        columnSpacing: 1
        rowSpacing: 1
        clip: true

        model: TableModel {}

        delegate: Rectangle {
            implicitWidth: 100
            implicitHeight: 50
            Text {
                text: display
            }
        }
    }
}
