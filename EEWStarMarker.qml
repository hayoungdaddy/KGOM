import QtQuick 2.5;
import QtLocation 5.6

MapQuickItem {
    id: eewStarMarker

    property var eewInfo: "Hello"
    property var sourceName: "../Icon/redStar.png"
    property var iconWidth: 40
    property var iconHeight: 40
    property var iconOpacity: 1.0

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
            font.pixelSize: 20
            color: 'red'
            opacity: 1.0
            horizontalAlignment: Text.AlignHCenter
            text: eewInfo
        }
    }
}

