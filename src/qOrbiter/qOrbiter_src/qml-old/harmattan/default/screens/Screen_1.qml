import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import "../components";

ScenariosPage {
    id: screen_1;
    headerColor: "#6300bb";
    pageTitle: "Home"
    itemsAreScreens: true;
    model: ListModel {
        id: pagesModel;
        ListElement {
            title: "Lighting";
            page: "Screen_2.qml";
        }
        ListElement {
            title: "Media";
            page: "Screen_3.qml";
        }
        ListElement {
            title: "Climate";
            page: "Screen_4.qml";
        }
        ListElement {
            title: "Telecom";
            page: "Screen_5.qml";
        }
        ListElement {
            title: "Security";
            page: "Screen_6.qml";
        }
    }
}
