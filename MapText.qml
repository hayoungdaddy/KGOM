import QtQuick 2.5;
import QtLocation 5.6;
import QtPositioning 5.8;

MapQuickItem {
    id: mapText

    property var lat: 1
    property var lon: 1
    property var t: "ttt"

    coordinate: QtPositioning.coordinate(lat, lon);

    sourceItem: Text{
        font.bold: true
        x: 25
        y: -11
        color: 'red'
        font.pixelSize: 20
        opacity: 1.0
        horizontalAlignment: Text.AlignHCenter

        text: t
    }
}
