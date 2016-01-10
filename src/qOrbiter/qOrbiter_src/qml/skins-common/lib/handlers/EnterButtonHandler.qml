import QtQuick 1.1
/*!
 *\class EnterButtonHandler
 *\brief Handler Component for Enter Button.
 *
 *\ingroup lib_handlers
 *
 */
MouseArea
{
    anchors.fill: parent
    hoverEnabled: true
    onClicked: manager.moveDirection(5)
}
