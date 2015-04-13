import QtQuick 2.2
Item{



Component{

        Timer{
            id:singleshot
            repeat: false
            interval: 1000
            triggeredOnStart: false
        }
}

Component{

        Timer{
            id:securitytimer
            repeat: true
            interval: 5000
            triggeredOnStart: true

        }

}

}
