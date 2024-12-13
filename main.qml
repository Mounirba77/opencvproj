//used qt libraries for the GUI design
//mariemomri
import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
//used librery to include videoCapture class as a customcomponent
import CustomComponents
ApplicationWindow {
//width and height of the GUI
    width: 800
    height: 680
    visible: true
    //top tiltle of the user interface
    title: qsTr("Enhanced Camera Feed")
    //boolean variable,it will controll the color or a circular shape,red if video recording is off,and green if its on
    property bool isrecording: false

    // Main layout container
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20 // Use margins instead of padding
        spacing: 20
        //top row ,includes the title of the GUI and the video recording circul status
        RowLayout{
            //this is a circulal shape that shows the video recording state in the GUI
            //red means recording is off and green means on
            Rectangle {
                //circuls radious
                width: 50
                height: 50
                radius: width / 2
                color: isrecording ? "lightgreen" : "red" // Color changes based on the recording state
                // Add a border for visibility (optional)
                border.color: "black"
                border.width: 2
              }
        // Title section
        Label {
            text: "Camera Feed"
            //size of the text
            font.pixelSize: 24
            //alignement of the title in the top middle of the GUI
            horizontalAlignment: Text.AlignHCenter
            Layout.alignment: Qt.AlignCenter
            leftPadding: 270
        }
        }
        // Video feed display
        Rectangle {
            //alignement of the display rectangle in the top middle under the title
            Layout.alignment: Qt.AlignHCenter
            //width and height of the displaying rectangle
            width: 493
            height: 370
            color: "transparent"
            border.color: "lightgray" //#F0F0F0
            border.width: 2
            radius: 5
            clip: true
            //videostream of the camera
            VideoCapture {
                id: videoCapture
                anchors.fill: parent
                frameRate: 30
            }
        }

        // Control section
        //this row includes the start and stop of the video recording buttons,also the image capture button
        RowLayout {
            spacing: 20
            Layout.alignment: Qt.AlignHCenter
            //start recording button
            Button {
                text: "Start Recording"
                //when the button is clicked it starts the recording
                //it uses the starrecording function from videocapture class,using the files name and the selected fps by the user as parametres
                //also it changes the 'isGreen' value from false to true to update the video recording circul's color
                onClicked:{
                    videoCapture.startRecording("video_output.avi",Slider.value)
                    // videoCapture.startRecording("video_output.avi")
                    isrecording = true
                }
            }
            //stops recording button
            Button {
                text: "Stop Recording"
                //when clicked it stops the video recording using stopRecording function from videocapture class
                //also it changes 'isGreeb' value from true to false to change the video recording status circul' color
                onClicked:{
                    videoCapture.stopRecording()
                    isrecording = false
                }
            }
            //saves the current frame with the activated filters as a photo
            Button {
                   text: "Take Photo"
                //when cliked it uses captureImage function from video capture class to save the current frame
                onClicked: videoCapture.captureImage()
            }
        }

        // Frame rate controls
        RowLayout {
            spacing: 10
            Layout.alignment: Qt.AlignHCenter

            Label { text: "Frame Rate:"; font.pixelSize: 16 }
            //slider to gibe the user the ability to change fps from the GUI
            Slider {
                //setting the frame interval from 1 to 60
                from: 1
                to: 60
                //setting the initial fps to 30
                value: 30
                //setting the changing step to 1
                stepSize: 1
                width: 200
                //sends the new fps tp the c++ to change it
                onValueChanged: videoCapture.frameRate = value
            }
            //shows the chosen framerate by the user in the GUI
            Label {
                text: videoCapture.frameRate + " FPS"
                font.pixelSize: 16
            }
            //this label shows the real fps,not the one chosen by user,but the real fps manually calculated in c++ class
            //the realFrameRate will show the calculated fps by the c++ video capture class in the GUI
            Label {
                  text: "Real FPS: " + videoCapture.realFrameRate.toFixed(2)
                  font.pixelSize: 16
              }
        }

        // Filters and effects section
        //this gridlayout includes checkboxs,each checkbox represents a filter,
        GridLayout {
            //number of columns
            columns: 3
            rowSpacing: 10
            columnSpacing: 20
            Layout.alignment: Qt.AlignHCenter
            //each filter will use its setter to change its boolean variable in c++ to enable and disable filters
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
