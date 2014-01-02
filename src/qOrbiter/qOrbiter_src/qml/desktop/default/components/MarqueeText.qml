import QtQuick 1.1

import "../effects"
Text {
    width: 100
    height: 62
    font.family: myFont.name
    color: textColor

    EffectGlow{
        dividerValue: 1
        source: parent
    }
}
