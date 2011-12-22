import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import "../components";

ScenariosPage {
    id: screen_2;
    headerColor: "#F1B011";
    pageTitle: "Lights"
    model: currentRoomLights;
    itemsAreScreens: false;
}
