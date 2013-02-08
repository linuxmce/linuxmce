import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import "../components";

ScenariosPage {
    id: screen_3;
    headerColor: "#1C51AE";
    pageTitle: "Media"
    model: currentRoomMedia;
    itemsAreScreens: false;
}
