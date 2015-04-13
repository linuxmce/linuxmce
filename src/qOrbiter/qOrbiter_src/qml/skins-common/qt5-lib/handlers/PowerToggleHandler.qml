import QtQuick 2.2
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
    onClicked: {
        manager.toggleDisplay(mode)
    }
}
