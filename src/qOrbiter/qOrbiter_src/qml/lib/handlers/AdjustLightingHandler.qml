import QtQuick 1.0
/*!
 *\class AdjustLightingHandler
 *\brief Handler Component for adjusting lighting.
 *
 *\ingroup lib_handlers
 *
 * Simple component designed to provide a mouse area with predefined functionality to raise / lower lights. See params for values
 *
 *
 */
MouseArea{
    property string level:"10" /*! String value. \note use "10" or "-10" */
    anchors.fill: parent
    onClicked: manager.adjustLights(level)
}
