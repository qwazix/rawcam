// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

Rectangle {
    width: 700
    height: 350
    color: "black";
    Column{
        anchors.fill: parent;
        anchors.margins: 20
        spacing: 10

        Row {
            id: rowRow
            spacing: 10
            width: parent.width;
            height: childrenRect.height

            Switch {
                id: dngSw
                checked: params.getSetting("saveDng",true);
                onCheckedChanged: params.setSetting("saveDng",checked)
            }

            Text {
                color: "white"
                width: rowRow.width - rowRow.spacing - dngSw.width
                height: dngSw.height
                verticalAlignment: Text.AlignVCenter
                text: dngSw.checked ? ".dng file save enabled" : ".dng file save disabled"
                font.pixelSize: 20
            }
        }
        Row {
            id: row2
            spacing: 10
            width: parent.width;
            height: childrenRect.height

            Switch {
                id: helpSw
                checked: params.getSetting("showHelp",!params.isN900());
                onCheckedChanged: params.setSetting("showHelp",checked)
            }

            Text {
                color: "white"
                width: row2.width - row2.spacing - helpSw.width
                height: helpSw.height
                verticalAlignment: Text.AlignVCenter
                text: helpSw.checked ? "Focus help baloon on" : "Focus help baloon off"
                font.pixelSize:20
            }
        }
        Row {
            id: row3
            spacing: 10
            width: parent.width;
            height: childrenRect.height

            Switch {
                id: focusOnTapSw
                checked: params.getSetting("focusOnTap",params.isN900());
                onCheckedChanged: params.setSetting("focusOnTap",checked)
            }

            Text {
                color: "white"
                height: focusOnTapSw.height
                verticalAlignment: Text.AlignVCenter
                text: focusOnTapSw.checked ? "Focus on capture on" : "Focus on capture off"
                font.pixelSize: 20
            }
        }
        Row {
            spacing: 20
            Text {
                color: "white"
                text: "White balance";
                font.pixelSize:20
                anchors.verticalCenter: parent.verticalCenter
            }
            Switch {
                id: wbSwitch
                checked: true
                onCheckedChanged: params.setWhiteBalanceMode(checked?0:1)
            }

            Rectangle {
                Text {

                    anchors.horizontalCenter: parent.horizontalCenter;
                    anchors.verticalCenter: parent.verticalCenter;
                    text: (wbSwitch.checked?"Auto":(Math.round(wbSlider.value)) + "K")
                    font.pixelSize : 17
                    color: "#000"

                }
                anchors.verticalCenter: parent.verticalCenter
                color: "#999";
                border.color: "#222"
                radius: 11
                width: 80
                height: 30

            }
        }
        Row {
            Slider {
                id: wbSlider
                minimumValue: 3000
                maximumValue: 8000
                value: 6500
                width: 600
                stepSize: 100
                valueIndicatorVisible: false
                onValueChanged: params.setWhiteBalance(value);
            }
        }
//        Row {
//            spacing: 20
//            Text {
//                text: "Exposure compensation";
//                font.pixelSize: 20
//            }
//            Rectangle {
//                Text {

//                    anchors.horizontalCenter: parent.horizontalCenter;
//                    anchors.verticalCenter: parent.verticalCenter;
//                    text: (expComp.value>0?"+":"") + (Math.round(expComp.value*100)/100) + " eV"
//                    font.pixelSize : 17
//                    color: "#fff"
//                }
//                color: "#444444";
//                border.color: "#dddddd"
//                radius: 11
//                width: 80
//                height: 30

//            }
//        }
//        Row {
//            Slider {
//                id: expComp
//                maximumValue: +4
//                minimumValue: -4
//                value: 0
//                width: 600
//                stepSize: 0.333333333333333
//                valueIndicatorVisible: false
//                onValueChanged: params.setExposureCompensation(value);
//            }
//        }
        Component.onCompleted: theme.inverted = true;
    }
}
