// import QtQuick 2.0 // to target S60 5th Edition or Maemo 5
/*
this is the desktop version of the animation presets. It is an experimental file that will be used to contain the animations
and in some cases integrated with opengl effects for various pages. The idea is that by making this common, we can expose the animations on
a wider scale than having to include them in every single page
  */
import QtQuick 2.0
//import Qt.labs.shaders 1.0

Item {

    property string anim_target: ""
    property int opacity_anim_length: 0
    property int scale_anim_length: 0
    property int rotation_anim_length: 0


        ParallelAnimation {
            id:fade_and_scale
            running: false
            loops: Animation.alwaysRunToEnd
            PropertyAnimation { target: anim_target; property: "opacity"; to: 1; duration: opacity_anim_length}
            PropertyAnimation { target: anim_target; property: "scale"; to: 1; duration: scale_anim_length}

        }




}

