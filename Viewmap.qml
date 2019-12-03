import QtQuick 2.1
import QtQuick.Controls 1.4
import QtLocation 5.8
import QtPositioning 5.8

Rectangle {
    id: rectangle
    width: 0
    height: 0
    visible: true

    property var eventMarker: []
    property var localStaMarker: []
    property var kissStaMarker: []
    property var staCircle: []
    property var eewStarMarker: "Hello"
    property var myPositionMarker: "Hello"
    property var currentEventMarker: "Hello"
    property var aniCircleP: []
    property var aniCircleS: []
    property real recOpacity: 0
    property int leftingTime: 99
    property int inten: 1
    property string vib: '미세'
    property string intenColor: 'black'
    property string vibColor: 'black'
    property string intenTextColor: 'white'
    property string vibTextColor: 'white'
    property variant map

    signal sendIDSignal(string ids, string lats, string lons)
    //signal sendStationIndexSignal(string msg)

    Plugin {
        id: streetPlugin
        name: "osm"

        //PluginParameter { name: "osm.mapping.providersrepository.disabled"; value: false }
        //PluginParameter { name: "osm.mapping.providersrepository.address"; value: "http://localhost/osm_tile/" }
        //PluginParameter { name: "osm.mapping.custom.host"; value: "http://localhost/osm_tile/" }
        PluginParameter { name: "osm.mapping.highdpi_tiles"; value: true }
        //PluginParameter { name: "osm.mapping.cache.directory"; value: "/.KGOM/map_data" }
        PluginParameter { name: "osm.mapping.offline.directory"; value: "/.KGOM/map_data" }
    }

    Map {
        anchors.fill: parent
        id: map
        plugin: streetPlugin
        center: QtPositioning.coordinate(36.3, 127.7)
        zoomLevel: 8
        maximumZoomLevel: 22
        minimumZoomLevel: 8

        Component.onCompleted: {
            for( var i_type in supportedMapTypes ) {
                if( supportedMapTypes[i_type].name.localeCompare( "Custom URL Map" ) === 0 ) {
                    activeMapType = supportedMapTypes[i_type]
                }
            }
            //activeMapType = supportedMapTypes[supportedMapTypes.length -1]
        }

        /*
        onZoomLevelChanged:{
            console.log(zoomLevel)
        }
        */

        onCenterChanged: {
            if(map.center.latitude > 44.5 || map.center.latitude < 30.5 || map.center.longitude > 137.5 || map.center.longitude < 119.5)
                map.center = QtPositioning.coordinate(36.3, 127.7)
        }

        Item {
            width: 350; height: 200
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 30
            opacity: recOpacity

            Rectangle { id: rec2; width: 350; height: 200; opacity: recOpacity - 0.3;
                    color: "white"; border.color: "black"; border.width: 2; }
            Label { width: 350; height: 100;
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter; y:15
                    font.pixelSize: 30
                    text: "예측진도        예측진동"
                }
            }
            Rectangle { x: 175; y: 11; width: 2; height: 180; color: "black"; border.color: "black"; border.width: 2; }
        }

        Item {
            width: 300; height: 150
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 53
            opacity: recOpacity

            Row {
                spacing: 50
                Rectangle { y: 40; width: 125; height: 100; color: intenColor; border.color: "black"; border.width: 2; radius: 10;
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 50
                        font.bold: true;
                        color: intenTextColor
                        text: inten
                    }
                }
                Rectangle { y: 40; width: 125; height: 100; color: vibColor; border.color: "black"; border.width: 2; radius: 10;
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 50
                        color: vibTextColor
                        text: vib
                    }
                }
            }
        }

        Item {
            width: 250; height: 200
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.margins: 30
            opacity: recOpacity
            Rectangle { y: 250; width: 250; height: 200; border.width: 2; opacity: recOpacity-0.3;
                color: "white"; border.color: "black";}
            Label { width: 250; height: 200;
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter;
                    y:255
                    font.pixelSize: 30
                    text: 'S파 예상 도달 시간'
                }
            }
            Label { width: 250; height: 200;
                Text {
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 150
                    y: 275
                    color: 'red'
                    text: leftingTime
                }
            }
        }
    }

    function clearMap()
    {
        map.clearMapItems();
    }

    function createCurrentEventMarker()
    {
        currentEventMarker = Qt.createQmlObject('EventMarker { }', map);
        currentEventMarker.colors = '#33FFFF';
        currentEventMarker.bcolors = '#33FFFF';
        map.addMapItem(currentEventMarker);

        return true;
    }

    function createEEWStarMarker()
    {
        eewStarMarker = Qt.createQmlObject('EEWStarMarker { }', map);
        return true;
    }

    function createMyPositionMarker()
    {
        myPositionMarker = Qt.createQmlObject('MyPositionMarker { }', map);
        return true;
    }

    function createLocalStaMarker()
    {
        var i;
        for(i=0;i<6;i++)
            localStaMarker[i] = Qt.createQmlObject('LocalStaMarker { }', map);

        return true;
    }

    function createKissStaMarker()
    {
        var i;
        for(i=0;i<6;i++)
            kissStaMarker[i] = Qt.createQmlObject('KissStaMarker { }', map);
        return true;
    }

    function createStaCircle(index, lat, longi, width, color, netsta, opacity)
    {
        staCircle[index] = Qt.createQmlObject('StaCircle { }', map);
        staCircle[index].index = index;
        staCircle[index].lati = lat;
        staCircle[index].longi = longi;
        staCircle[index].widthi = width;
        staCircle[index].colors = color;
        staCircle[index].txt = netsta;
        staCircle[index].circleOpacity = opacity;

        map.addMapItem(staCircle[index]);
    }

    function changeSizeAndColorForStaCircle(index, width, color, opacity)
    {
        staCircle[index].widthi = width;
        staCircle[index].colors = color;
        staCircle[index].circleOpacity = opacity;
    }

    function addEventCircle(lat, longi, colors, id)
    {
        eventMarker[id] = Qt.createQmlObject('EventMarker { }', map);
        eventMarker[id].lati = lat;
        eventMarker[id].longi = longi;
        eventMarker[id].colors = colors;
        eventMarker[id].bcolors = 'white';
        eventMarker[id].id = id;
        eventMarker[id].txt = "ID:" + id + " Lat.:" + lat + " Lon.:" + longi;

        map.addMapItem(eventMarker[id]);

        return true;
    }

    function moveCurrentEventMarker(id, lat, longi)
    {
        map.removeMapItem(currentEventMarker);
        currentEventMarker.colors = '#33FFFF';
        currentEventMarker.bcolors = '#33FFFF';
        currentEventMarker.lati = lat;
        currentEventMarker.longi = longi;
        currentEventMarker.id = id;
        map.addMapItem(currentEventMarker);

        return true;
    }

    function adjustMap(lat, longi, dist)
    {
        map.center = QtPositioning.coordinate(lat, longi);

        if(dist <= 100)
            map.zoomLevel = 10;
        else if(dist <= 200)
            map.zoomLevel = 9;
        else if(dist <= 300)
            map.zoomLevel = 8;
        else
            map.zoomLevel = 8;
    }

    function addCircle(lat, longi, rad)
    {
        var circle = Qt.createQmlObject('import QtLocation 5.8; MapCircle { }', map, "dynamic");
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
            map.zoomLevel = 8;

        return true;
    }

    function addCircleForAnimation(lat, longi, radP, radS, left, inte, vi, itcolor, icolor)
    {
        map.removeMapItem(aniCircleP);
        map.removeMapItem(aniCircleS);

        aniCircleP = Qt.createQmlObject('import QtLocation 5.8; MapCircle { }', map, "dynamic");
        aniCircleP.center = QtPositioning.coordinate(lat, longi);
        aniCircleP.radius = radP;
        aniCircleP.opacity = 1;
        aniCircleP.border.color = 'blue';
        aniCircleP.border.width = 3;

        aniCircleS = Qt.createQmlObject('import QtLocation 5.8; MapCircle { }', map, "dynamic");
        aniCircleS.center = QtPositioning.coordinate(lat, longi);
        aniCircleS.radius = radS;
        aniCircleS.opacity = 1;
        aniCircleS.border.color = 'red';
        aniCircleS.border.width = 3;

        map.addMapItem(aniCircleP);
        map.addMapItem(aniCircleS); 

        recOpacity = 1;

        leftingTime = left;
        inten = inte;
        vib = vi;
        intenTextColor = itcolor;
        intenColor = icolor;
        vibTextColor = itcolor;
        vibColor = icolor;

        return true;
    }

    function removeItemForAnimation()
    {
        map.removeMapItem(aniCircleP);
        map.removeMapItem(aniCircleS);
        recOpacity = 0;
        //recOpacity = 1;
    }

    function addStaMarker(net, lat, longi, staName)
    {
        var staMarker;
        if(net === 1)
            staMarker = Qt.createQmlObject('KissStaMarker { }', map);
        else if(net ===0)
            staMarker = Qt.createQmlObject('LocalStaMarker { }', map);
        staMarker.coordinate = QtPositioning.coordinate(lat, longi);
        staMarker.staName = staName;
        map.addMapItem(staMarker);
    }

    function addStaMarker2(net, lat, longi, staName, which)
    {
        map.center = QtPositioning.coordinate(lat, longi);

        if(net === 1) // kiss station
        {
            kissStaMarker[which].coordinate = QtPositioning.coordinate(lat, longi);
            kissStaMarker[which].staName = staName;
            map.addMapItem(kissStaMarker[which]);
        }
        else if(net === 0) // local station
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
        eewStarMarker.eewInfo = "M " + mag;

        map.addMapItem(eewStarMarker);
        return true;
    }

    function addMyPositionMarker(lat, longi)
    {
        myPositionMarker.coordinate = QtPositioning.coordinate(lat, longi);
        map.addMapItem(myPositionMarker);
        return true;
    }

    function moveMyPositionMarker(lat, longi)
    {
        myPositionMarker.coordinate = QtPositioning.coordinate(lat, longi);
        return true;
    }

    function addPolyline(lat1, lon1, lat2, lon2)
    {
        var polyline = Qt.createQmlObject('import QtLocation 5.8; MapPolyline { }', map, "dynamic");
        polyline.line.width = 3;
        polyline.line.color = 'white';
        polyline.addCoordinate(QtPositioning.coordinate(lat1, lon1));
        polyline.addCoordinate(QtPositioning.coordinate(lat1, lon2));

        map.addMapItem(polyline);

        return true;
    }

    function addText(lat, longi, content)
    {
        var myText = Qt.createQmlObject('MapText { }', map);
        myText.lat = lat;
        myText.lon = longi;
        myText.t = content;

        map.addMapItem(myText);

        return true;
    }

    function addPolygon(lats, lons, colors, numLine)
    {
        var polygon = Qt.createQmlObject('import QtLocation 5.8; MapPolygon { }', map, "dynamic");
        polygon.border.width = 2;
        polygon.border.color = 'black';
        polygon.opacity = 0.5;

        for(var i=0;i<numLine;i++)
        {
            polygon.color = colors[i];
            polygon.addCoordinate( QtPositioning.coordinate(lats[i], lons[i]) );
        }

        map.addMapItem(polygon);

        return true;
    }
}
