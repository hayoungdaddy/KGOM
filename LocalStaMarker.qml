import QtQuick 2.5;
import QtLocation 5.6

MapQuickItem {
    id: localStaMarker

    property var staName: "Hello"

    anchorPoint.x: image.width * 0.5
    anchorPoint.y: image.height

    sourceItem: Image {
        id: image
        source: "../Icon/triangle-blue.png"
        width: 25
        height: 25
        opacity: 1.0

        Text{
            id: myText
            y: image.height
            width: image.width
            font.bold: true
            font.pixelSize: 15
            opacity: 1.0
            horizontalAlignment: Text.AlignHCenter
            text: staName
        }
    }
}

