import QtQuick 2.5;
import QtLocation 5.8;
import QtPositioning 5.8;

MapQuickItem {
    id: mapText

    property real lat: 1.0
    property real lon: 1.0
    property string t: "ttt"

    coordinate: QtPositioning.coordinate(lat, lon);

    sourceItem: Text{
        font.bold: true
        x: 25
        y: -11
        color: 'white'
        font.pixelSize: 22
        opacity: 1.0
        horizontalAlignment: Text.AlignHCenter

        text: t
    }
}
