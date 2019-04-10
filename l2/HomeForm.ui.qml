import QtQuick 2.12
import QtQuick.Controls 2.5

Page {
    id: page
    width: 600
    height: 400
    property alias enableCameraButton: enableCameraButton

    title: qsTr("Home")

    Column {
        id: row
        spacing: 5
        anchors.fill: parent

        Label {
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("You are on the home page.")
        }

        Button {
            id: enableCameraButton
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Enable camera")
            Connections {
                target: enableCameraButton
                onClicked: appEngine.enableCamera()
            }
        }
    }
}
