import QtQuick 2.2

Item{
    id:applicationStyle
    objectName: "style"

    Connections{
        target:settings
        onFontSizeModChanged:{
            console.log("Font mod changed:")
        }
    }
    function scaleX(w){
        return w/100*manager.appWidth
    }

    function scaleY(y){
        return y/100*manager.appHeight
    }

    function dumpObj(obj, message){
        console.log((message? message+"\n\t" : "No Message \n\t")+ JSON.stringify(obj, null, "\t"))
    }

  readonly property double dpRatio:screenInfo.primaryScreen.pixelRatio

    //picture ratios - will be moved to cpp class
    property double dvdPosterRatio:955/1080
    property double hdPosterRatio:755/1080
    property double cdCoverRatioFront:1080/1080
    property double cdCoverRatioBack:1080/1264
    property double vcdRatio:1080/1080
    property double vhsRatio:1280/620

    /* Transitions */
    readonly property int transition_animationTime:350
    readonly property int transition_accentTime:350
    readonly property int transitionFadeTime:350

    /* Application Colors */
    readonly property color appcolor_background:"#0588ff"
    readonly property color appcolor_background_light:"#80c4ff"
    readonly property color appcolor_background_medium:"#43acff"
    readonly property color appcolor_background_list:"#222222"
    readonly property color appcolor_foregroundColor:"darkgreen"
    readonly property color appcolor_navigationBackgroundColor:"blue"

    /*Application Buttons */
    readonly property string appbutton_confirm_color:appcolor_background
    readonly property color appbutton_cancel_color:"#f26c4f"
    readonly property color appbutton_disabled_color:"#464646"
    readonly property color appbutton_color:appcolor_background
    property int appButtonHeight:manager.isProfile ? scaleY(12) : scaleY(16)
    property int appButtonWidth: manager.isProfile ? scaleX(26) : scaleX(15)
    readonly property int appButtonNumberPadHeight:manager.isProfile ? scaleY(12) : scaleY(18)
    readonly property int appButtonNumberPadWidth: manager.isProfile ? scaleX(18) : scaleX(12)
    readonly property int appButtonNumberPadSmallHeight:manager.isProfile ? scaleY(6) : scaleY(9)
    readonly property int appButtonNumberPadSmallWidth:manager.isProfile ? scaleX(9) : scaleX(6)
    readonly property int appButtonLargeHeight:manager.isProfile ? scaleY(15) : scaleY(20)
    readonly property int appButtonLargeWidth:manager.isProfile ? scaleX(28) : scaleX(25)
    readonly property int buttonSpacing:5

    /* Application Fonts */
    readonly property color apptext_color_active:"white"
    readonly property color apptext_color_inactive:"grey"
    readonly property color apptext_color_list_active:"blue"
     property int appFontSize_list:12
     property int appFontSize_header:16
     property int appFontSize_description:11
     property int appFontSize_title:14


    property int fontSize_small:8
    property int fontSize_medium:appFontSize_description
    property int fontSize_large:appFontSize_title

    property int fontSize_listItem:9
    property int fontSize_listTitle:14
    property int fontSize_listAccent:10

    /* Application Navigation Bars */
    readonly property string appbutton_navigationButtonHeight:manager.isProfile ? scaleY(13) : scaleY(15)
    readonly property string appNavigation_panelHeight:manager.isProfile ? scaleY(7) : scaleY(9)

    /* Application Menu Panels */
    readonly property int appMenuPanel_height:scaleY(90)
    readonly property int appMenuPanel_widthSmall:scaleX(25)
    readonly property int appMenuPanel_widthMedium:scaleX(45)
    readonly property int appMenuPanel_widthLarge:scaleX(85)

    /* Application List Views */
    readonly property int listViewWidth_large:scaleX(55)
    readonly property int listViewWidth_medium:manager.isProfile ? scaleX(35): scaleX(20)
    readonly property int listViewWidth_small:scaleX(15)
    readonly property int listViewItemHeight:manager.isProfile ? scaleY(12) : scaleY(15)
    readonly property int listViewMargin:manager.isProfile ? scaleX(5) : scaleX(2.5)

    /* Dialogs */
    readonly property int dialog_large_height:scaleY(75)
    readonly property int dialog_large_width:scaleX(85)

    //opacity and accents
    readonly property double appHeader_opacity:.55
    readonly property double appPanel_opacity:.45
    readonly property double appList_opacity:.89

    //remote Screens
    readonly property double remote_posterHeight:manager.isProfile ? scaleY(34) : scaleY(55)
    readonly property double remote_posterWidth:remote_posterHeight*hdPosterRatio

    readonly property double remote_albumHeight:manager.b_orientation ? scaleY(25) : scaleY(50)
    readonly property double remote_albumWidth:remote_albumHeight

}
