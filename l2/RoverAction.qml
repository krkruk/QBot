import QtQuick 2.4

RoverActionForm {
    enableCameraButton.onClicked: {
        console.log("Button clicked")
        appEngine.enableCamera()
    }


    Connections {
        target: appEngine
        ignoreUnknownSignals: true
        onCameraEnabledChanged: {
            console.log("Camera state changed" + cameraEnabled)
        }
    }
}
