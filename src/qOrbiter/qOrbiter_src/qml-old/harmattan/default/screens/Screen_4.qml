import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0
import "../components";

ScenariosPage {
    id: screen_4;
    headerColor: "#007cc8";
    pageTitle: "Climate"
    model: currentRoomClimate;
    itemsAreScreens: false;
}
