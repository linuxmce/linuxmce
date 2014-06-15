import QtQuick 1.1

Item {

    id:main_default_schema
    //New style //
    /* Overall */
    property double bgLightOpacity:.35
    property double bgHeavyOpacity:.65
    property double bgContentOpacity:.50

    /* Headers, containers and lists */
    property color contentBgColor:"darkgrey"
    property int headerFontSize:14
    property color headerBgColor:"green"
    property int listItemHeight:scaleY(8)
    property color listItemBgColor:"green"
    property color listItemActiveBgColor: "white"
    property color listItemPressedBgColor: "limegreen"
    property color listItemTextInactiveColor:"white"
    property color listItemTextActiveColor:"black"
    property Gradient bgHeaderGradient: Gradient{
        GradientStop{ position:0.0;color:"transparent"}
        GradientStop{ position:0.65;color:"black"}
    }
    property Gradient bgContentGradient: Gradient{

        GradientStop{ position:0.0;color:"transparent"}
        GradientStop{ position:0.75;color:"black"}
    }

    /* Buttons and their junk */
    property int buttonHeight:scaleY(4)
    property int  buttonWidth: scaleX(12)
    property int buttonLabelSize:scaleY(2)
    property color buttonColor:"darkgreen"
    property color buttonPressedColor:"white"
    property color buttonBorderColor:"white"
    property int buttonBorderWidth:1
    property Gradient buttonGradient: Gradient{
        GradientStop{ position:0.0;color:"transparent"}
        GradientStop{ position:0.65;color:"black"}
    }


    /* Animation Speeds */
    property int quickAnimation:350
    property int mediumAnimation:500
    property int longAnimation:750
    property int animationEasing:Easing.OutBack


    //end scale function
    //-color definitions

    property color accentcolor: "green"
    property color highlight1: "silver"
    property color highlight2: "orange"
    property color alertcolor: "red"
    property color warncolor:"yellow"
    property color darkColor:"black"
    property double shadeOpacity:.65

    property color darkhighlight: "darkgrey"
    property color lighthighlight: "green"
    //--end color definitions
}
