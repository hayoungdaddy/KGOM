import QtQuick 2.1
import QtQuick.Controls 1.4
import QtLocation 5.6
import QtPositioning 5.5

Rectangle {
    id: rectangle
    width: 550
    height: 810
    visible: true

    property var localStaMarker: []
    property var kissStaMarker: []
    property var allKissStaMarker: []
    property var numKissSta: 0

    Plugin {
        id: mapPlugin
        name: "osm"
        PluginParameter { name: "osm.mapping.cache.directory"; value: "/home/sysop/KGOnSite/params/maps" }
        PluginParameter { name: "osm.mapping.offline.directory"; value: "/home/sysop/KGOnSite/params/maps" }
    }

    Map {
        anchors.fill: parent
        id: map
        plugin: mapPlugin
        center: QtPositioning.coordinate(36.1, 127.75)
        zoomLevel: 7
    }

    function createLocalStaMarker()
    {
        var i;
        for(i=0;i<6;i++)
            localStaMarker[i] = Qt.createQmlObject('LocalStaMarker { } ', map);
        return true;
    }

    function addLocalStaMarker(lat, longi, staName, which)
    {
        localStaMarker[which].coordinate = QtPositioning.coordinate(lat, longi);
        localStaMarker[which].staName = staName;
        map.addMapItem(localStaMarker[which]);
        return true;
    }

    function delLocalStaMarker(which)
    {
        map.removeMapItem(localStaMarker[which]);
        return true;
    }

    function createAllKissStaMarker(lat, longi, staName)
    {
        allKissStaMarker[numKissSta] = Qt.createQmlObject('KissStaMarker { } ', map);
        allKissStaMarker[numKissSta].coordinate = QtPositioning.coordinate(lat, longi);
        allKissStaMarker[numKissSta].staName = staName;
        allKissStaMarker[numKissSta].sourceName = "../Icon/triangle-red.png"
        allKissStaMarker[numKissSta].iconWidth = 25
        allKissStaMarker[numKissSta].iconHeight = 25
        allKissStaMarker[numKissSta].iconOpacity = 0.7
        numKissSta++;
        return true;
    }

    function showAllKissStaMarker()
    {
        var i;
        for(i=0;i<numKissSta;i++)
            map.addMapItem(allKissStaMarker[i]);
        return true;
    }

    function hideAllKissStaMarker()
    {
        var i;
        for(i=0;i<numKissSta;i++)
            map.removeMapItem(allKissStaMarker[i]);
        return true;
    }

    function createKissStaMarker()
    {
        var i;
        for(i=0;i<6;i++)
        {
            kissStaMarker[i] = Qt.createQmlObject('KissStaMarker { } ', map);
            kissStaMarker[i].sourceItem.opacity = 1.0;
        }
        return true;
    }

    function addKissStaMarker(lat, longi, staName, which)
    {
        kissStaMarker[which].coordinate = QtPositioning.coordinate(lat, longi);
        kissStaMarker[which].staName = staName;
        map.addMapItem(kissStaMarker[which]);
        return true;
    }

    function delKissStaMarker(which)
    {
        map.removeMapItem(kissStaMarker[which]);
        return true;
    }

    function clearMap()
    {
        map.clearMapItems();
        return true;
    }

}
