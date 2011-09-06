import QtQuick 1.0


        Timer{
            id:singleshot
            repeat: false
            interval: 1000
            triggeredOnStart: false
        }




        Timer{
            id:securitytimer
            repeat: true
            interval: 5000
            triggeredOnStart: true

        }



