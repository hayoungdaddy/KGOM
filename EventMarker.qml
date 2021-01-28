import QtQuick 2.5;
import QtLocation 5.8

MapQuickItem {
    id: eventMarker

    property real lati : 0
    property real longi : 0
    property string colors: 'red'
    property string bcolors: 'white'
    property int id: 0
    property real textOpacity: 0
    property string txt: ""

    sourceItem: Rectangle {
        id: circle
        width: 20
        height: 20
        color: colors
        border.width: 3
        border.color: bcolors
        smooth: true
        radius: 10

        Text{
            id: ttt
            y: circle.height
            width: circle.width
            font.bold: true
            font.pixelSize: 15
            opacity: textOpacity
            horizontalAlignment: Text.AlignHCenter
            text: txt
        }
    }
    coordinate {
        latitude: lati
        longitude: longi
    }
    opacity: 1.0
    anchorPoint: Qt.point(sourceItem.width/2, sourceItem.height/2)

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
            circle.border.color='red';
            textOpacity=1;
        }
        onExited: {
            circle.border.color=bcolors;
            textOpacity=0;
        }
        onPressed: {
            rectangle.sendIDSignal(id, lati, longi);
        }
    }
}
