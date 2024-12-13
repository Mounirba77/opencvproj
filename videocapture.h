#ifndef VIDEOCAPTURE_H
#define VIDEOCAPTURE_H
//used libreries
#include <QQuickPaintedItem>
#include <QImage>
#include <QTimer>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/videoio/videoio.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/flann/flann.hpp"
#include <QPainter>
#include <QDebug>
#include <QObject>
//class constructer
//properties to connect qml front interface to c++ back
class VideoCapture : public QQuickPaintedItem {
    Q_OBJECT
    Q_PROPERTY(int frameRate READ frameRate WRITE setFrameRate NOTIFY frameRateChanged)
    Q_PROPERTY(bool applyMedian READ applyMedian WRITE setApplyMedian NOTIFY applyMedianChanged)
    Q_PROPERTY(bool applyGaussian READ applyGaussian WRITE setApplyGaussian NOTIFY applyGaussianChanged)
    Q_PROPERTY(bool filterAvg READ filterAvg WRITE setFilterAvg NOTIFY filterAvgChanged)
    Q_PROPERTY(bool uniformNoise READ uniformNoise WRITE setUniformNoise NOTIFY uniformNoiseChanged)
    Q_PROPERTY(bool saltPepperNoise READ saltPepperNoise WRITE setSaltPepperNoise NOTIFY saltPepperNoiseChanged)
    Q_PROPERTY(bool gaussianNoise READ gaussianNoise WRITE setGaussianNoise NOTIFY gaussianNoiseChanged)
    Q_PROPERTY(bool invertColors READ invertColors WRITE setInvertColors NOTIFY invertColorsChanged)
    Q_PROPERTY(bool blackAndWhite READ blackAndWhite WRITE setBlackAndWhite NOTIFY blackAndWhiteChanged)
    Q_PROPERTY(bool applyBilateralFilter READ applyBilateralFilter WRITE setApplyBilateralFilter NOTIFY applyBilateralFilterChanged)
    Q_PROPERTY(bool applyBoxFilter READ applyBoxFilter WRITE setApplyBoxFilter NOTIFY applyBoxFilterChanged)
    Q_PROPERTY(bool applyDetectFace READ applyDetectFace WRITE setApplyDetectFace NOTIFY detectFaceChanged)
    Q_PROPERTY(bool applySepFilter READ applySepFilter WRITE setApplySepFilter NOTIFY applySepFilterChanged)
    Q_PROPERTY(double realFrameRate READ realFrameRate NOTIFY realFrameRateChanged)


public:
    //constructer and destroyer of the class
    VideoCapture(QQuickItem *parent = nullptr);
    ~VideoCapture();
    //paint procedure to show image in qml
    void paint(QPainter *painter) override;
    //each proporty will have a setter and a getter,they are needed for the properies to work
    //properpies are used for the fps slider to get the fps value,to transfer the real fps in the qml interface
    //and to get the boolean values of each filter for it to work

    //fps slider's setter and getter
    int frameRate() const;
    void setFrameRate(int rate);
    //each filter needs a setter and getter
    bool applyMedian() const;
    void setApplyMedian(bool value);

    bool applyGaussian() const;
    void setApplyGaussian(bool value);

    bool filterAvg() const;
    void setFilterAvg(bool value);

    bool uniformNoise() const;
    void setUniformNoise(bool value);

    bool saltPepperNoise() const;
    void setSaltPepperNoise(bool value);

    bool gaussianNoise() const;
    void setGaussianNoise(bool value);

    bool invertColors() const;
    void setInvertColors(bool value);

    bool blackAndWhite() const;
    void setBlackAndWhite(bool value);

    bool applyBilateralFilter() const;
    void setApplyBilateralFilter(bool value);

    bool applyBoxFilter() const;
    void setApplyBoxFilter(bool value);

    bool applySepFilter() const;
    void setApplySepFilter(bool value);

    bool applyDetectFace() const;
    void setApplyDetectFace(bool value);
    //a simple setter to show the real fps in the applicatin
    double realFrameRate() const;

public slots:
    //frame updater to capture multiple frames to make the video
    void updateFrame();
    //recording functions
    //the first one only takes the filename as a parametre
    //the second one takes the files name and the recording frame rate
    void startRecording(const QString &filename);
    void startRecording(const QString &filename,float framerate);
    void stopRecording();
    //this procedure will capture a screenshot of the video
    void captureImage();

signals:
    //these signals are needed for the properies to work to make the connection between qml and c++
    void frameRateChanged();
    void applyMedianChanged();
    void applyGaussianChanged();
    void filterAvgChanged();
    void uniformNoiseChanged();
    void saltPepperNoiseChanged();
    void gaussianNoiseChanged();
    void invertColorsChanged();
    void blackAndWhiteChanged();
    void applyBilateralFilterChanged();
    void applyBoxFilterChanged();
    void applySepFilterChanged();
    void detectFaceChanged();
    void realFrameRateChanged();
private:
    //the videocapture variable to capture the frames using opencv
    cv::VideoCapture cap;
    //the videowriter variable to record the videos later
    cv::VideoWriter videoWriter;
    //Qimage variable to store the frame
    QImage frameImage;
    //Qtimer variable to set the time between each uodate based on the fps
    QTimer timer;
    //boolean variables to track the state or camera and recording
    bool isCameraOpen;
    bool isRecording;
    //boolean variables to tracl the state of each filter,used by properties to maintain the connection between qml and c++
    int m_frameRate;
    bool m_applyMedian;
    bool m_applyGaussian;
    bool m_filterAvg;
    bool m_uniformNoise;
    bool m_saltPepperNoise;
    bool m_gaussianNoise;
    bool m_invertColors;
    bool m_blackAndWhite;
    bool m_applyBilateralFilter;
    bool m_applyBoxFilter;
    bool m_applySepFilter;
    bool m_detectFace;
    double m_realFrameRate;
    //some filers are simply 1 line ,that's why they don't need any extra function,but some filters are complicated
    //thos they need dedicated functions for these filters
    void addUniformNoise(cv::Mat &mat);
    void addSaltPepperNoise(cv::Mat &mat);
    void addGaussianNoise(cv::Mat &mat);
    void addDetectFace(cv::Mat &frame);
    //since opencv gives cvmat images,and qt does'nt know it,this function will change cvmat frames to qimage which qt undrestands
    QImage cvMatToQImage(const cv::Mat &mat);
};

#endif // VIDEOCAPTURE_H
