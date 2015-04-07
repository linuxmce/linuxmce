import QtQuick 1.1

Row {
    id: tabContainer
    property ListModel actions
    property int itemWidth: 200
    property int selectedIndex: 0

    Repeater {
        property alias itemWidth: tabContainer.itemWidth
	property alias selectedIndex: tabContainer.selectedIndex
        model: parent.actions
        delegate: TabDelegate {   }
    }
}