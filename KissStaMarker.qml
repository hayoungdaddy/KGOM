import QtQuick 2.5;
import QtLocation 5.8

MapQuickItem {
    id: kissStaMarker

    property string staName: "Hello"
    property string sourceName: "../Icon/kissStaMarker.png"
    property int iconWidth: 25
    property int iconHeight: 25
    property real iconOpacity: 1.0

    anchorPoint.x: image.width * 0.5
    anchorPoint.y: image.height * 0.5

    sourceItem: Image {
        id: image
        source: sourceName
        width: iconWidth
        height: iconHeight
        opacity: iconOpacity

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

