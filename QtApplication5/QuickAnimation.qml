import QtQuick
import QtQuick.Controls

ApplicationWindow {
    visible: true; title: "QuickAnimation"; width: 400; height: 150
    maximumWidth: 400; maximumHeight: 150
    minimumWidth: 400; minimumHeight: 150

    Rectangle {id: re; x: 20; y: 20; width: 30; height: 30; color: "#d0d0d0"}

    SequentialAnimation {
        id: an
        ParallelAnimation {
            NumberAnimation {target: re; property: "x";
                from: 20; to: 330; duration: 1000}
            RotationAnimation {target: re; property: "rotation";
                from: 0; to: 45; duration: 1000}
            ColorAnimation  {target: re; property: "color";
                from: "#d0d0d0"; to: "#303030"; duration: 1000}
        }
        PauseAnimation {duration: 1000}
        NumberAnimation {target: re; property: "x";
            from: 330; to: 20; duration: 1000}
    }

    Button {x: 20; y: 70; width: 100; text: "Los"; onClicked: an.start()}
}
