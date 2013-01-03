import QtQuick 1.0
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
