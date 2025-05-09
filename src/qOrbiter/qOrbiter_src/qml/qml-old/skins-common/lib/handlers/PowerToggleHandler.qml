import QtQuick 1.1
/*!
 *\class PowerToggleHandler
 *\brief Handler Component for Toggling Display.
 *
 *\ingroup lib_handlers
 *
 */
MouseArea{
    property int mode /*! 0=Off / 1=On  */
    anchors.fill: parent
    signal activated();
    onReleased: {
        activated()
        manager.toggleDisplay(mode)
        console.log("clicked")
    }
}
