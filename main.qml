import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import CustomComponents

ApplicationWindow {
    width: 800
    height: 680
    visible: true
    title: qsTr("Enhanced Camera Feed")
    property bool isrecording: false
    property real elapsedTime: 0.0 // Tracks elapsed time in milliseconds

    Timer {
        id: recordingTimer
        interval: 100 // 0.1 second interval
        repeat: true
        running: false
        onTriggered: elapsedTime += 0.1 // Increment elapsed time in 0.1-second steps
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        RowLayout {
            Rectangle {
                width: 50
                height: 50
                radius: width / 2
                color: isrecording ? "lightgreen" : "red"
                border.color: "black"
                border.width: 2
            }

            Label {
                text: "Camera Feed"
                font.pixelSize: 24
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignCenter
                leftPadding: 270
            }
        }

        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 493
            height: 370
            color: "transparent"
            border.color: "lightgray"
            border.width: 2
            radius: 5
            clip: true

            VideoCapture {
                id: videoCapture
                anchors.fill: parent
                frameRate: 30
            }
        }

        RowLayout {
            spacing: 20
            Layout.alignment: Qt.AlignHCenter

            Button {
                text: "Start Recording"
                onClicked: {
                    videoCapture.startRecording("video_output.avi", Slider.value)
                    isrecording = true
                    elapsedTime = 0.0 // Reset elapsed time
                    recordingTimer.start() // Start the timer
                }
            }

            Button {
                text: "Stop Recording"
                onClicked: {
                    videoCapture.stopRecording()
                    isrecording = false
                    recordingTimer.stop() // Stop the timer
                }
            }

            Button {
                text: "Take Photo"
                onClicked: videoCapture.captureImage()
            }
        }

        RowLayout {
            spacing: 10
            Layout.alignment: Qt.AlignHCenter

            Label { text: "Frame Rate:"; font.pixelSize: 16 }
            Slider {
                from: 1
                to: 60
                value: 30
                stepSize: 1
                width: 200
                onValueChanged: videoCapture.frameRate = value
            }
            Label { text: videoCapture.frameRate + " FPS"; font.pixelSize: 16 }
            Label { text: "Real FPS: " + videoCapture.realFrameRate.toFixed(2); font.pixelSize: 16 }
        }

        RowLayout {
            spacing: 10
            Layout.alignment: Qt.AlignHCenter

            Label {
                text: "Recording Time: " + elapsedTime.toFixed(1) + " s" // Display time in seconds with one decimal place
                font.pixelSize: 16
            }
        }

        GridLayout {
            columns: 3
            rowSpacing: 10
            columnSpacing: 20
            Layout.alignment: Qt.AlignHCenter

            CheckBox { text: "Median Filter"; onCheckedChanged: videoCapture.applyMedian = checked }
            CheckBox { text: "Gaussian Filter"; onCheckedChanged: videoCapture.applyGaussian = checked }
            CheckBox { text: "Average Filter"; onCheckedChanged: videoCapture.filterAvg = checked }
            CheckBox { text: "Uniform Noise"; onCheckedChanged: videoCapture.uniformNoise = checked }
            CheckBox { text: "Salt & Pepper Noise"; onCheckedChanged: videoCapture.saltPepperNoise = checked }
            CheckBox { text: "Gaussian Noise"; onCheckedChanged: videoCapture.gaussianNoise = checked }
            CheckBox { text: "Invert Colors"; onCheckedChanged: videoCapture.invertColors = checked }
            CheckBox { text: "Black and White"; onCheckedChanged: videoCapture.blackAndWhite = checked }
            CheckBox { text: "Bilateral Filter"; onCheckedChanged: videoCapture.applyBilateralFilter = checked }
            CheckBox { text: "Box Filter"; onCheckedChanged: videoCapture.applyBoxFilter = checked }
            CheckBox { text: "SepFilter2D"; onCheckedChanged: videoCapture.applySepFilter = checked }
            CheckBox { text: "Face Detection"; onCheckedChanged: videoCapture.applyDetectFace = checked }
        }
    }
}
