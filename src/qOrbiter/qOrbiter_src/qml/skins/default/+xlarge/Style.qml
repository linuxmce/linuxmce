import QtQuick 2.2

QtObject{
    id:applicationStyle
    objectName: "style"
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
    readonly property int transition_accentTime:500
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
    readonly property int appButtonHeight:scaleY(4)
    readonly property int appButtonWidth:scaleX(16.5)
    readonly property int appButtonNumberPadHeight:manager.isProfile ? scaleY(12) : scaleY(18)
    readonly property int appButtonNumberPadWidth: manager.isProfile ? scaleX(18) : scaleX(12)
    readonly property int appButtonNumberPadSmallHeight:manager.isProfile ? scaleY(9) : scaleY(15)
    readonly property int appButtonNumberPadSmallWidth:manager.isProfile ? scaleX(15) : scaleX(9)
    readonly property int appButtonLargeHeight:manager.isProfile ? scaleY(15) : scaleY(20)
    readonly property int appButtonLargeWidth:manager.isProfile ? scaleX(28) : scaleX(25)
    readonly property int buttonSpacing:5

    /* Application Fonts */
    readonly property color apptext_color_active:"white"
    readonly property color apptext_color_inactive:"grey"
    readonly property color apptext_color_list_active:"blue"
    readonly property int appFontSize_list:24*dpRatio
    readonly property int appFontSize_header:18*dpRatio
    readonly property int appFontSize_description:28*dpRatio
    readonly property int appFontSize_title:32*dpRatio

    property int fontSize_small:appFontSize_list
    property int fontSize_medium:appFontSize_description
    property int fontSize_large:appFontSize_title

    property int fontSize_listItem:14*dpRatio
    property int fontSize_listTitle:16*dpRatio
    property int fontSize_listAccent:12*dpRatio

    /* Application Navigation Bars */
    readonly property string appbutton_navigationButtonHeight:scaleY(13)
    readonly property string appNavigation_panelHeight: scaleY(7)

    /* Application Menu Panels */
    readonly property int appMenuPanel_height:scaleY(90)
    readonly property int appMenuPanel_widthSmall:scaleX(25)
    readonly property int appMenuPanel_widthMedium:scaleX(45)
    readonly property int appMenuPanel_widthLarge:scaleX(85)

    /* Application List Views */
    readonly property int listViewWidth_large:scaleX(55)
    readonly property int listViewWidth_medium:scaleX(20)
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

    readonly property double remote_albumHeight:manager.b_orientation ? scaleY(50) : scaleY(75)
    readonly property double remote_albumWidth:remote_albumHeight

}
