import QtQuick 1.0
/*!
 *\class ScreenBackHandler
 *\brief Handler Component for going back to the previous screen.
 *
 *\ingroup lib_handlers
 *Sends the user to the previous screen if there is one. no params.
 *\sa goBackToQScreen
 */
MouseArea{
    anchors.fill: parent
    onClicked: manager.goBacktoQScreen()
}
