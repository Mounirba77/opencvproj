#include "videocapture.h"
//random librery,used for the noices,it will be used later
#include <random>
//class contructor,this step is important since it initialtes all the variables of the class
VideoCapture::VideoCapture(QQuickItem *parent)
    : QQuickPaintedItem(parent),
    isCameraOpen(false),
    isRecording(false),
    m_frameRate(30),
    m_applyMedian(false),
    m_applyGaussian(false),
    m_filterAvg(false),
    m_uniformNoise(false),
    m_saltPepperNoise(false),
    m_gaussianNoise(false),
    m_invertColors(false),
    m_blackAndWhite(false),
    m_applyBilateralFilter(false),
    m_applyBoxFilter(false),
    m_applySepFilter(false),
    m_detectFace(false),
    m_realFrameRate(0.0) {
    cap.open(0); // Open default camera
    if (cap.isOpened()) {
        isCameraOpen = true;
        connect(&timer, &QTimer::timeout, this, &VideoCapture::updateFrame);
        timer.start(1000 / m_frameRate);
    } else {
        qWarning() << "Failed to open camera";
    }
}

//destroyer of the class,it closes the video capture and the recording
VideoCapture::~VideoCapture() {
    if (cap.isOpened()) {
        cap.release();
    }
    if (isRecording) {
        stopRecording();
    }
}
//paint is used to display the image in qml GUI
void VideoCapture::paint(QPainter *painter) {
    //test to make sure the frame is not empty before painting it
    if (!frameImage.isNull()) {
        //drawimage is used to show the frame in qml,the first 2 zeros represents the initial x and y to start painting(top left corner in this case)
        //the 'frameImage.scaled' is used to make sure the drawed image will have the same dimension as the captured frame
        //the 'QT::KeepAspectRatio' is used to make sure there are no empty pixels while drawing the image to show it in qml front
        painter->drawImage(0, 0, frameImage.scaled(width(), height(), Qt::KeepAspectRatio));
    }
}
//this function updates the frames based on a timer
//this timer is set based on the current fps
void VideoCapture::updateFrame() {
    //test to make sure the camera is working,else it will not update the frame
    if (!isCameraOpen) return;
    //declaring the cv::mat var to store current captured frame
    cv::Mat frame;
    cap >> frame; // Capture a frame
    //test to make sure frame is not empty
    if (!frame.empty()) {
        //this part of the code will manually calculate the real fps compared to the fps setted by the fps slider in the qml interface
        //the way it works is it saves the time of the capture of the last frame,then saves the capture time of the current frame
        //then it calculates the difference between them ,and finaly realfps=1000/time_difference
        //********************************************************************************************************************************
        //captures the time of the last frame
        static auto lastTime = std::chrono::high_resolution_clock::now();
        //captures the time of the current frame
        auto currentTime = std::chrono::high_resolution_clock::now();
        //calculates the time difference between the last and current frames
        double elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
        //tests if the difference is positive then calculates the framerate
        //then changes the variable of the real fps and sends a signal the realframerate changed to display the new realframerate
        if (elapsed > 0) {
            m_realFrameRate = 1000.0 / elapsed;
            emit realFrameRateChanged();
        }
        //changing the lasttime with current time to prepare to the next calculation when the new frame comes
        lastTime = currentTime;
        //for each filter there is a boolean variable,if its true the flter will be activated
        //some filters are a simple instruction,others are complicated,thats why after each test to check if the user asked for that filter,there are
        //both simple instructions and a call to functions after each test
        if (m_applyMedian) cv::medianBlur(frame, frame, 5);
        if (m_applyGaussian) cv::GaussianBlur(frame, frame, cv::Size(5, 5), 0);
        if (m_filterAvg) cv::blur(frame, frame, cv::Size(5, 5));
        if (m_applyBoxFilter) cv::boxFilter(frame, frame, -1, cv::Size(5, 5));
        if (m_applySepFilter) {
            cv::Mat kernelX = cv::getGaussianKernel(5, 1);
            cv::Mat kernelY = cv::getGaussianKernel(5, 1);
            cv::sepFilter2D(frame, frame, -1, kernelX, kernelY);
        }
        if (m_uniformNoise) addUniformNoise(frame);
        if (m_saltPepperNoise) addSaltPepperNoise(frame);
        if (m_gaussianNoise) addGaussianNoise(frame);
        if (m_invertColors) cv::bitwise_not(frame, frame);
        if (m_blackAndWhite) {
            cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
            cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
        }

        if (m_applyBilateralFilter) {
            try {
                cv::Mat temp;
                cv::bilateralFilter(frame, temp, 9, 75, 75);
                frame = temp;
            } catch (const cv::Exception &e) {
                qWarning() << "Bilateral Filter failed:" << e.what();
            }
        }

        if (m_detectFace) addDetectFace(frame);

        frameImage = cvMatToQImage(frame);
        //captures the time of the last frame
        static auto lastTime1 = std::chrono::high_resolution_clock::now();
        //captures the time of the current frame
        auto currentTime1 = std::chrono::high_resolution_clock::now();
        //calculates the time difference between the last and current frames
        double elapsed1 = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime1 - lastTime1).count();
        //tests if the difference is positive then calculates the framerate
        //then changes the variable of the real fps and sends a signal the realframerate changed to display the new realframerate
        if (elapsed1 > 0) {
            m_realFrameRate = 1000.0 / elapsed1;
            emit realFrameRateChanged();
        }
        //changing the lasttime with current time to prepare to the next calculation when the new frame comes
        lastTime1 = currentTime1;
        if (isRecording && videoWriter.isOpened()) {
            videoWriter.write(frame);
        }
        //this lign will trigger the update again,else it will only capture the frame once
        update();

        // Record video with synchronized FPS
        // This part will make sure that video recorder will save frames when the frames updates to not miss any frame
        if (isRecording && videoWriter.isOpened()) {
            videoWriter.write(frame);
        }
    }

}

//this function starts the video recording
//i made 2 functions,one accepts only 1 variable,other one uses 2 variables which are file name and the recording fps
//this is the first finction
void VideoCapture::startRecording(const QString &filename) {
    //test to make sure that the frame capturing is active and its not already recording else it will close the recording
    if (!isCameraOpen || isRecording) return;
    //prepares the videowriter required parametres to start the recording
    //prepâring the width of the recording
    int frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    //preparing the height of the recording
    int frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    //initating the fps variable for recording
    //then uses that variable to store the videostream fps as the recording fps
    int fps = static_cast<int>(cap.get(cv::CAP_PROP_FPS));
    //incase it can't load the stream fps,it addes it manually from the fps chossen by user from qml interface
    if (fps <= 0) fps = m_frameRate;
    //video recording function,it uses videowriter variable to record
    //first variable is the files name,second one is the used video incoder
    //third variable is the recording fps and fourth variable is the recording frame dimensions
    videoWriter.open(filename.toStdString(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fps,
                     cv::Size(frameWidth, frameHeight));
    //test to make change the 'isRecording' status from false to true,also it shows a notification about the recording status
    //also the recording fps.
    if (videoWriter.isOpened()) {
        isRecording = true;
        qDebug() << "Recording started to file:" << filename;
        qDebug() << "recording fps: " << m_frameRate;
    } else {
        qWarning() << "Failed to start recording";
    }
}
//this is the second recordig function,it uses 2 parametres which are the file name and the recording frame rate
void VideoCapture::startRecording(const QString &filename, float framerate) {
    //test to make sure that the frame capturing is active and its not already recording else it will close the recording
    if (!isCameraOpen || isRecording) return;
    //prepares the videowriter required parametres to start the recording
    //prepâring the width of the recording
    int frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    //preparing the height of the recording
    int frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    //int fps = static_cast<int>(cap.get(cv::CAP_PROP_FPS));
    // if (fps <= 0) fps = m_frameRate;
    // cap.set(cv::CAP_PROP_FPS,framerate);
    // Record video with synchronized FPS
    double fr = double(framerate);
    //video recording function,it uses videowriter variable to record
    //first variable is the files name,second one is the used video incoder
    //third variable is the recording fps and fourth variable is the recording frame dimensions
    videoWriter.open(filename.toStdString(), cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), fr,
                     cv::Size(frameWidth, frameHeight));
    //test to make change the 'isRecording' status from false to true,also it shows a notification about the recording status
    //also the recording fps.And incase recording didn't start,it notifies this in appliation output.
    if (videoWriter.isOpened()) {
        isRecording = true;
        qDebug() << "Recording started to file:" << filename;
        qDebug() << "recording fps: " << fr;
    } else {
        qWarning() << "Failed to start recording";
    }
}
//this function stops the video recording
void VideoCapture::stopRecording() {
    //tests the status of video recording,if it's off,it exits the functions
    if (!isRecording) return;
    //release will close the videowriter var to stop the recording
    videoWriter.release();
    //Changes the video recording status from true to false
    isRecording = false;
    //shows notification in application output section that video recording is off
    qDebug() << "Recording stopped";
}
//This function captures an image of th video stream
//to make sure that the image includes the applied filters,i added the filters status test to apply them
void VideoCapture::captureImage() {
    //test to make sure that the frame is not empty,else it will close the function
    if (!isCameraOpen) return;
    //saves the current frame in a new cv::mat variable to apply the filters
    cv::Mat frame; cap >> frame;
    //same as videoupdate,it tests to make sure the frame is not empty,then applies the filters
    if (!frame.empty()) {
        if (m_applyMedian) cv::medianBlur(frame, frame, 5);
        if (m_applyGaussian) cv::GaussianBlur(frame, frame, cv::Size(5, 5), 0);
        if (m_filterAvg) cv::blur(frame, frame, cv::Size(5, 5));
        if (m_uniformNoise) addUniformNoise(frame);
        if (m_saltPepperNoise) addSaltPepperNoise(frame);
        if (m_gaussianNoise) addGaussianNoise(frame);
        if (m_invertColors) cv::bitwise_not(frame, frame);
        if (m_applyBoxFilter) cv::boxFilter(frame, frame, -1, cv::Size(5, 5));
        if (m_applySepFilter) {
            cv::Mat kernelX = cv::getGaussianKernel(5, 1);
            cv::Mat kernelY = cv::getGaussianKernel(5, 1);
            cv::sepFilter2D(frame, frame, -1, kernelX, kernelY);
        }
        if (m_blackAndWhite) {
            cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
            cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);
        }

        if (m_applyBilateralFilter) {
            try {
                cv::Mat temp;
                cv::bilateralFilter(frame, temp, 9, 75, 75);
                frame = temp;
            } catch (const cv::Exception &e) {
                qWarning() << "Bilateral Filter failed:" << e.what();
            }
        }

        if (m_detectFace) addDetectFace(frame);

        // Save the frame into a file
        imwrite("image_output.jpg", frame);
        qDebug() << "cheese";
    }
}
//Since this project is based on opencv,opencv's frame type is cv::mat,knowing that this is a qt project,qt uses QImage type
//this function converts the captured frame from cv::mat to qimage to use it in qt project.
QImage VideoCapture::cvMatToQImage(const cv::Mat &mat) {
    //the conversion will happen for either cv_8uc3 or cv_8uc1 based on the webcam's input
    if (mat.type() == CV_8UC3) {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888).rgbSwapped();
    } else if (mat.type() == CV_8UC1) {
        return QImage(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
    } else {
        return QImage();
    }
}

//setter and getter for the frame rate
//the getter will import the wanted user's fps from the qml
//then the setter will change the current fps with the new fps chosen by the user
//the setter will also start the timer based on the new fps,also it will send a signal for the qproperty to work.
int VideoCapture::frameRate() const { return m_frameRate; }
void VideoCapture::setFrameRate(int rate) {
    if (m_frameRate != rate) {
        m_frameRate = rate;
        timer.start(1000 / m_frameRate);
        emit frameRateChanged();
    }
}
//for each filter there is a setter and a getter
//the setter and getter will change the boolean variable to use it later to know which filters the user wants.
// Median filter property
bool VideoCapture::applyMedian() const { return m_applyMedian; }
void VideoCapture::setApplyMedian(bool value) {
    if (m_applyMedian != value) {
        m_applyMedian = value;
        emit applyMedianChanged();
    }
}

// Gaussian filter property
bool VideoCapture::applyGaussian() const { return m_applyGaussian; }
void VideoCapture::setApplyGaussian(bool value) {
    if (m_applyGaussian != value) {
        m_applyGaussian = value;
        emit applyGaussianChanged();
    }
}

// Getter and Setter for filterAvg
bool VideoCapture::filterAvg() const { return m_filterAvg; }
void VideoCapture::setFilterAvg(bool value) {
    if (m_filterAvg != value) {
        m_filterAvg = value;
        emit filterAvgChanged();
    }
}

// Getter and Setter for uniformNoise
bool VideoCapture::uniformNoise() const { return m_uniformNoise; }
void VideoCapture::setUniformNoise(bool value) {
    if (m_uniformNoise != value) {
        m_uniformNoise = value;
        emit uniformNoiseChanged();
    }
}

// Getter and Setter for saltPepperNoise
bool VideoCapture::saltPepperNoise() const { return m_saltPepperNoise; }
void VideoCapture::setSaltPepperNoise(bool value) {
    if (m_saltPepperNoise != value) {
        m_saltPepperNoise = value;
        emit saltPepperNoiseChanged();
    }
}

// Getter and Setter for gaussianNoise
bool VideoCapture::gaussianNoise() const { return m_gaussianNoise; }
void VideoCapture::setGaussianNoise(bool value) {
    if (m_gaussianNoise != value) {
        m_gaussianNoise = value;
        emit gaussianNoiseChanged();
    }
}

// Getter and Setter for invertColors
bool VideoCapture::invertColors() const { return m_invertColors; }
void VideoCapture::setInvertColors(bool value) {
    if (m_invertColors != value) {
        m_invertColors = value;
        emit invertColorsChanged();
    }
}

// Getter and Setter for blackAndWhite
bool VideoCapture::blackAndWhite() const { return m_blackAndWhite; }
void VideoCapture::setBlackAndWhite(bool value) {
    if (m_blackAndWhite != value) {
        m_blackAndWhite = value;
        emit blackAndWhiteChanged();
    }
}
// Getter and Setter for applyBilateralFilter
bool VideoCapture::applyBilateralFilter() const { return m_applyBilateralFilter; }
void VideoCapture::setApplyBilateralFilter(bool value) {
    if (m_applyBilateralFilter != value) {
        m_applyBilateralFilter = value;
        emit applyBilateralFilterChanged();
    }
}

// Getter and Setter for applyBoxFilter
bool VideoCapture::applyBoxFilter() const { return m_applyBoxFilter; }
void VideoCapture::setApplyBoxFilter(bool value) {
    if (m_applyBoxFilter != value) {
        m_applyBoxFilter = value;
        emit applyBoxFilterChanged();
    }
}

// Getter and Setter for applySepFilter
bool VideoCapture::applySepFilter() const { return m_applySepFilter; }
void VideoCapture::setApplySepFilter(bool value) {
    if (m_applySepFilter != value) {
        m_applySepFilter = value;
        emit applySepFilterChanged();
    }
}

bool VideoCapture::applyDetectFace() const { return m_detectFace; }
void VideoCapture::setApplyDetectFace(bool value) {
    if (m_detectFace != value) {
        m_detectFace = value;
        emit detectFaceChanged();
    }
}
//this function takes a cv::mat as a parametre then finds the faces inside it using an ai model from opencv.
void VideoCapture::addDetectFace(cv::Mat &frame) {
    //loads the 'haarcascade_frontalface_default' model using CascadeClassifier variable
    //also it includes a warning incase there is a problem while loading the ai model
    static cv::CascadeClassifier faceCascade;
    if (faceCascade.empty()) {
        if (!faceCascade.load("haarcascade_frontalface_default.xml")) {
            qWarning() << "Failed to load cascade file.";
            return;
        }
    }
    //creates a new cv:mat variable which will have the black and white version of the frame
    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
    //enhances contrast of the black and white frame to make finding faces easier
    cv::equalizeHist(gray, gray);

    // Reduce resolution for faster detection
    // creates a new cv::mat variable to store the resized black and white frame
    cv::Mat smallGray;
    // stores the resized frame with a 1/2 scale to use it later in the face detection
    cv::resize(gray, smallGray, cv::Size(gray.cols / 2, gray.rows / 2));
    //enhences contrast of the black and white frame to make finding faces easier
    cv::equalizeHist(smallGray, smallGray);
    // preparing variable vector to store the faces later
    std::vector<cv::Rect> faces;
    //uses the loaded ai module to detect faces
    //the first parametre is the used frame to detect faces,the second parametre is the vector that stores the faces
    //third parametre is the scale factor,it controls the increas of the search window,the fourth is the minimum number of close rectangles
    //to detect a face,fifth means no additionnal glags to use and the final one is the minimum size of faces that it can detect
    faceCascade.detectMultiScale(smallGray, faces, 1.1, 3, 0, cv::Size(50, 50));
    //this 'for' structure will go through all the faces,detect their real x and y cordinates then draw a rectangle arround them
    for (auto &face : faces) {
        // Scale back the face coordinates
        face.x *= 2;
        face.y *= 2;
        face.width *= 2;
        face.height *= 2;
        //draws a rectangle arround the faces and add a text to indicate the faces
        cv::rectangle(frame, face, cv::Scalar(0, 255, 0), 2);
        cv::putText(frame, "Face", cv::Point(face.x, face.y - 10),
                    cv::FONT_HERSHEY_SIMPLEX, 0.9, cv::Scalar(0, 255, 0), 2);
    }
    // Record video with synchronized FPS
    // This part will make sure that video recorder will save frames when the frames updates to not miss any frame
    if (isRecording && videoWriter.isOpened()) {
        videoWriter.write(frame);
    }
}

double VideoCapture::realFrameRate() const {
    return m_realFrameRate;
}

//this function adds the uniformnoice
//it uses the randu function to create uniform noise
void VideoCapture::addUniformNoise(cv::Mat &mat) {
    cv::Mat noise(mat.size(), mat.type());
    cv::randu(noise, 0, 256);
    mat = mat + noise;
}
//this function adds salt and pepper noise
void VideoCapture::addSaltPepperNoise(cv::Mat &mat) {
    //this part will create a new randomnes device which will be used later to create the salt and pepper noise later
    //A source of randomness, used to seed the random number generator
    std::random_device rd;
    //A Mersenne Twister random number generator seeded with rd
    std::mt19937 gen(rd());
    //Generates random integers uniformly distributed between 0 and the number of columns (width of the image minus one)
    std::uniform_int_distribution<> dist(0, mat.cols - 1);
    //this is the color version of the random generator
    // std::uniform_int_distribution<> colorDist(0, 255);
    //The number of "salt" and "pepper" pixels is calculated as 2% of the total number of pixels in the image
    //numSalt: Number of white pixels to add.
    //numPepper: Number of black pixels to add.
    int numSalt = mat.rows * mat.cols * 0.02;  // 2% salt
    int numPepper = mat.rows * mat.cols * 0.02;  // 2% pepper
    //add salt noise
    //dist will pick random pixels then apply salt noise on them
    for (int i = 0; i < numSalt; ++i) {
        int x = dist(gen);
        int y = dist(gen) % mat.rows;
        if (mat.channels() == 3) { // Color image
            mat.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);
        } else if (mat.channels() == 1) { // Grayscale image
            mat.at<uchar>(y, x) = 255;
        }
    }
    //add peper noise
    //dist will pick random picxels the apply salt noice on them
    for (int i = 0; i < numPepper; ++i) {
        int x = dist(gen);
        int y = dist(gen) % mat.rows;
        if (mat.channels() == 3) { // Color image
            mat.at<cv::Vec3b>(y, x) = cv::Vec3b(0, 0, 0);
        } else if (mat.channels() == 1) { // Grayscale image
            mat.at<uchar>(y, x) = 0;
        }
    }
}
//this function adds gaussian noise
void VideoCapture::addGaussianNoise(cv::Mat &mat) {
    //creates a new cv::mat variable with the same dimensions and type of the cv::mat parametre variable
    cv::Mat noise(mat.size(), mat.type());
    //adds a random noise in the new cv::mat variable using randu function
    cv::randn(noise, 0, 25);
    //adds the noisy frame to the input frame to add the gaussian noise
    mat = mat + noise;
}
