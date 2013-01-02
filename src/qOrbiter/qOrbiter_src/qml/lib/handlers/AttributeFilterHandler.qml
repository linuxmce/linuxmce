import QtQuick 1.0
/*!
 *\class AttributeFilterHandeler
 *\brief Handler for filtering through a datagrid
 *
 *\ingroup lib_handlers
 *
 * This handler is designed to work in conjunction with a pre-defined datagrid. It will filter or prepare a file details based on attribute
 *

 *
 */
MouseArea{
    anchors.fill: parent
    onClicked:{
       currentFilterModel.setSelectionStatus(name)
    }
}
