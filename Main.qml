import QtQuick
import QtQuick.Controls

import UsbGuardDevicesModel

Window {

    SystemPalette { id: appPalette; colorGroup: SystemPalette.Active }

    width: 640
    height: 480
    visible: true
    title: qsTr("USBGuard GUI Dashboard")

    // Set the Window's background color to the system's window color.
    // This will correctly switch between white (light mode) and dark grey (dark mode).
    color: appPalette.window

    Column {

        Row {
            height: 50

            Button {
                text: "Click Me!"
                onClicked: {
                    console.log("Button clicked!");
                    // Perform desired action here
                }
            }

            Button {
                text: "Another Button"
                onClicked: {
                    console.log("Another button clicked!");
                }
            }
        }

        visible: true
        spacing: 10
        anchors.fill: parent

        TableView {
            anchors.fill: parent
            columnSpacing: 1
            rowSpacing: 1
            clip: true

            model: UsbGuardDevicesModel {}

            delegate: Rectangle {
                implicitWidth: 150
                implicitHeight: 50

                // Display the table header in a different color than the table content
                color: (header == true) ? appPalette.accent : appPalette.base

                //TableView.onPooled: console.log(tabledata + " pooled")
                //TableView.onReused: console.log(tabledata + " reused")

                Text {
                    text: tabledata
                    anchors.centerIn: parent
                    font.pointSize: 12
                    // Display the text in the table header in a different color
                    color: (header == true) ? appPalette.dark : appPalette.text
                }
            }
        }
    }
}
