import QtQuick 2.2
/*!
 *\class AttributeFilterHandler
 *\brief Handler for filtering through a datagrid via various attributes.
 *
 *\ingroup lib_handlers
 *
 * This handler is designed to work in conjunction with a pre-defined datagrid.
 * It sets the property in the selected model and emits the signal to initate filtering
 * based on that attribute.
 *
 *
 */
MouseArea{
    anchors.fill: parent
    onClicked:{
       currentFilterModel.setSelectionStatus(name)
    }
}
