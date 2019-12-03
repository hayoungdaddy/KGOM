import QtQuick 2.6;
import QtLocation 5.8

MapQuickItem {
    id: myPositionMarker

    property string eewInfo: "MyPosition"
    property string sourceName: "../Icon/myPositionMarker.png"

    anchorPoint.x: image.width * 0.8
    anchorPoint.y: image.height * 0.8

    sourceItem: Image {
        id: image
        source: "../Icon/myPositionMarker.png"
        width: 45
        height: 50
        opacity: 1.0

        Text{
            id: myText
            y: image.height
            width: image.width
            font.bold: true
            font.pixelSize: 15
            opacity: 1.0
            horizontalAlignment: Text.AlignHCenter
            //text: eewInfo
        }
    }
}

