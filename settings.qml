// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0
    Rectangle {
        id: grandpa
        width: 700
        height: 350
        color: "black";
        Flickable{
            id: bigFlick
            anchors.fill: parent
            anchors.margins: 15
            contentHeight: contentItem.childrenRect.height
            Column{
//                anchors.fill: grandpa;
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
                        checked: params.getSetting("showHelp",true);
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
                        checked: params.getSetting("focusOnTap",false);
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
                Row {
                    spacing: 10
                    Text{
                        text: "aspect ratio"
                        color: "white"
                        font.pixelSize: 20
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    ButtonRow {
                        width: 400
                        Button{
                            text: "4:3"
                            id: fourthirds
                            checked : params.getSetting("AspectRatio",0)==0
                            property int value: 0
                        }
                        Button{
                            text: "16:9"
                            id: wide
                            checked: params.getSetting("AspectRatio",0)==1
                            property int value: 1
                        }
                        onCheckedButtonChanged: params.setAspectRatio(checkedButton.value);
                    }
                }

            Row {
                    spacing: 10
                    Text{
                        id: stabilizationLbl
                        text: "stabilization"
                        color: "white"
                        font.pixelSize: 20
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    ButtonRow {
                        width: 400
                        Button{
                            text: "Off"
                            id: stabilizationOff
                            property int value: 0;
                            checked : params.getSetting("stabilization",0)==value
                        }
                        Button{
                            text: "Burst"
                            id: burstAssistedStabilization
                            property int value: 1;
                            checked : params.getSetting("stabilization",0)==value
                        }
                        Button{
                            text: "Accelerometer"
                            id: accelerometerAssistedStabilization
                            property int value: 2;
                            checked : params.getSetting("stabilization",0)==value
                        }
                        onCheckedButtonChanged: params.setStabilization(checkedButton.value);
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
        ScrollDecorator{
            flickableItem: bigFlick
        }
    }
