import QtQuick

Window {

    SystemPalette { id: appPalette; colorGroup: SystemPalette.Active }

    width: 640
    height: 480
    visible: true
    title: qsTr("USBGuard GUI Dashboard")

    // Set the Window's background color to the system's window color.
    // This will correctly switch between white (light mode) and dark grey (dark mode).
    color: appPalette.window

    // The main content area
    Column {
        anchors.fill: parent
        spacing: 15
        padding: 20

        Text {
            horizontalAlignment: Qt.AlignCenter
            font.pixelSize: 24
            font.bold: true
            text: qsTr("USB Guard Service Status")

            color: appPalette.windowText
        }

        Rectangle {
            width: 600
            height: 480
            color: "#E0F7FA"
            radius: 8
            border.color: "#00BCD4"
            border.width: 1

            Text {
                font.pixelSize: 18
                text: qsTr("Service: Running\nPolicy: Loaded (0 devices blocked)")
                color: "#006064"
                horizontalAlignment: Text.AlignHCenter
            }
        }

    }
}
