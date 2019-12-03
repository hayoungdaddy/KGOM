import QtQuick 2.5;
import QtLocation 5.8

MapQuickItem {
    id: localStaMarker

    property string staName: "Hello"

    anchorPoint.x: image.width * 0.5
    anchorPoint.y: image.height * 0.5

    sourceItem: Image {
        id: image
        source: "../Icon/localStaMarker.png"
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

