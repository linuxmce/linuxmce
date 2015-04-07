import QtQuick 1.1

import "../components"
import "../js/ComponentLoader.js" as JsLib


//to test new pageloading function //RayBe

Item {
    //height: mainComponentScreenLoader.height
    //width: mainComponentScreenLoader.width
    //anchors.fill: mainComponentScreenLoader <-- werkt
    anchors.fill: parent
    //anchors.margins: 10
    //height: style.orbiterH *.7
    //width: style.orbiterW *.7
    Rectangle {
        //height: parent.height
        //width: parent.width
        anchors.fill: parent
        //anchors.margins: 10
        //height: style.orbiterH *.7
        //width: style.orbiterW *.7
        color:  "red"
        opacity: .8
    }
}

