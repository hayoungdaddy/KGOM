import QtQuick 2.1
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.5

Rectangle {
    id: rectangle
    width: 0
    height: 0
    visible: true

    property var localStaMarker: []
    property var kissStaMarker: []
    property var eewStarMarker: "Hello"
    property var myPositionMarker: "Hello"
    property var dirName: "Hello"
    property var mapName: "Hello"
    property var aniCircleP: []
    property var aniCircleS: []
    property var recOpacity: 0
    property var leftingTime: 99
    property var inten: 1
    property var vib: '미세'
    property var forRight: 0

    Plugin {
        id: mapPlugin
        name: mapName

        PluginParameter { name: "osm.mapping.cache.directory"; value: dirName }
        PluginParameter { name: "osm.mapping.offline.directory"; value: dirName }
    }

    Map {
        anchors.fill: parent
        id: map
        center: QtPositioning.coordinate(36.3, 127.7)
        zoomLevel: 7

        Item {
            width: 300; height: 100
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 30
            opacity: recOpacity

            Row {
                spacing: 20
                Rectangle { id: rec1; width: 80; height: 80; color: "white"; border.color: "gray"; border.width: 3; radius: 10; }
                Rectangle { id: rec2; width: 180; height: 80; color: "white"; border.color: "gray"; border.width: 3; radius: 10; }
            }
            Row {
                spacing: 20
                Label { width: 80; height: 80;
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter; y:5
                        text: "남은시간"
                    }
                }
                Label { width: 180; height: 80;
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter; y:5
                        text: "예측진도        예측진동"
                    }
                }
            }
            Rectangle { x: 188; y: 11; width: 3; height: 60; color: "gray"; border.color: "gray"; border.width: 3; }

        }
        Item {
            width: 185; height: 100
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 30
            opacity: recOpacity

            Row {
                spacing: 20
                Rectangle { y: 27; width: 65; height: 45; color: "black"; border.color: "black"; border.width: 3; radius: 10;
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 20
                        color: 'white'
                        text: inten
                    }
                }
                Rectangle { y: 27; width: 65; height: 45; color: "black"; border.color: "black"; border.width: 3; radius: 10;
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 20
                        color: 'white'
                        text: vib
                    }
                }
            }
        }
        Item {
            width: 270; height: 80
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 30
            opacity: recOpacity
            Rectangle { x: forRight; y: 35; width: 30; height: 30; color: "white"; border.color: "white"; border.width: 3;
                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize: 21
                    color: 'red'
                    text: leftingTime + "<font color='black' size='1'>초</font>"
                }
            }
        }
    }

    function mapReset(dname, mname)
    {
        dirName = dname;
        //mapName = "mapboxgl";
        mapName = mname;
        map.plugin = mapPlugin;
    }

    function clearMap()
    {
        map.clearMapItems();
    }

    function createEEWStarMarker()
    {
        eewStarMarker = Qt.createQmlObject('EEWStarMarker { } ', map);
        return true;
    }

    function createMyPositionMarker()
    {
        myPositionMarker = Qt.createQmlObject('MyPositionMarker { } ', map);
        return true;
    }

    function createLocalStaMarker()
    {
        var i;
        for(i=0;i<6;i++)
            localStaMarker[i] = Qt.createQmlObject('LocalStaMarker { } ', map);
        return true;
    }

    function createKissStaMarker()
    {
        var i;
        for(i=0;i<6;i++)
            kissStaMarker[i] = Qt.createQmlObject('KissStaMarker { } ', map);
        return true;
    }

    function addCircle(lat, longi, rad)
    {
        var circle = Qt.createQmlObject('import QtLocation 5.6; MapCircle { }', map, "dynamic");
        circle.center = QtPositioning.coordinate(lat, longi);
        circle.radius = rad;
        circle.opacity = 0.1;
        circle.color = 'red';
        circle.border.width = 1;

        map.addMapItem(circle);

        if(rad <= 50000)
            map.zoomLevel = 11;
        else if(rad > 50000 && rad <= 15000)
            map.zoomLevel = 9;
        else if(rad > 35000)
            map.zoomLevel = 7;

        return true;
    }

    function addCircleForAnimation(lat, longi, radP, radS, left, inte, vi)
    {
        map.removeMapItem(aniCircleP);
        map.removeMapItem(aniCircleS);

        aniCircleP = Qt.createQmlObject('import QtLocation 5.6; MapCircle { }', map, "dynamic");
        aniCircleP.center = QtPositioning.coordinate(lat, longi);
        aniCircleP.radius = radP;
        aniCircleP.opacity = 1;
        aniCircleP.border.color = 'blue';
        aniCircleP.border.width = 3;

        aniCircleS = Qt.createQmlObject('import QtLocation 5.6; MapCircle { }', map, "dynamic");
        aniCircleS.center = QtPositioning.coordinate(lat, longi);
        aniCircleS.radius = radS;
        aniCircleS.opacity = 1;
        aniCircleS.border.color = 'red';
        aniCircleS.border.width = 3;

        map.addMapItem(aniCircleP);
        map.addMapItem(aniCircleS); 

        recOpacity = 1;

        if(left < 10 && left >= 0)
            forRight = 10;
        else if(left < 0)
            forRight = 5;
        else
            forRight = 0;


        leftingTime = left;
        inten = inte;
        vib = vi;

        return true;
    }

    function removeItemForAnimation()
    {
        map.removeMapItem(aniCircleP);
        map.removeMapItem(aniCircleS);
        map.removeMapItem(myPositionMarker);
        recOpacity = 0;
    }

    function addStaMarker(net, lat, longi, staName, which, zLevel)
    {
        map.center = QtPositioning.coordinate(lat, longi);
        //map.zoomLevel = zLevel;

        if(net == 1) // kiss station
        {
            kissStaMarker[which].coordinate = QtPositioning.coordinate(lat, longi);
            kissStaMarker[which].staName = staName;
            map.addMapItem(kissStaMarker[which]);
        }
        else if(net == 0) // local station
        {
            localStaMarker[which].coordinate = QtPositioning.coordinate(lat, longi);
            localStaMarker[which].staName = staName;
            map.addMapItem(localStaMarker[which]);
        }

        return true;
    }

    function addEEWStarMarker(lat, longi, mag)
    {
        map.center = QtPositioning.coordinate(lat, longi);
        eewStarMarker.coordinate = QtPositioning.coordinate(lat, longi);
        eewStarMarker.eewInfo = "MAG.:" + mag;

        map.addMapItem(eewStarMarker);
        return true;
    }

    function addMyPositionMarker(lat, longi)
    {
        myPositionMarker.coordinate = QtPositioning.coordinate(lat, longi);
        map.addMapItem(myPositionMarker);
        return true;
    }

    function addPolyline(lat1, lon1, lat2, lon2)
    {
        var polyline = Qt.createQmlObject('import QtLocation 5.6; MapPolyline { }', map, "dynamic");
        polyline.line.width = 3;
        polyline.line.color = 'white';
        polyline.addCoordinate(QtPositioning.coordinate(lat1, lon1));
        polyline.addCoordinate(QtPositioning.coordinate(lat1, lon2));

        map.addMapItem(polyline);

        return true;
    }

    function addText(lat, longi, ttt)
    {
        var myText = Qt.createQmlObject('MapText { }', map);
        myText.lat = lat;
        myText.lon = longi;
        myText.t = ttt;

        map.addMapItem(myText);

        return true;
    }
}
