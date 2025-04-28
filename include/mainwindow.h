#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSlider>
#include <opencv2/opencv.hpp>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void activateSliders(QSlider* slider1, QSlider* slider2);
    void onUploadButtonClicked();
    void onThreshButtonClicked();
    void GaussianBluring();
    void grayScaleConversion();
    void applyMedianFilter();
    void histogramsEqualization();
    void applyDilation();
    void applyErosion();

private:
    Ui::MainWindow *ui;
    cv::Mat resizedImage; // Store the resized image for reuse
    cv::Mat grayScaledImage;
    cv::Mat gaussianBlurredImage;
    cv::Mat medianFilteredImage;
    cv::Mat equImage;
    cv::Mat erosionImage;
    cv::Mat dilationImage;
    cv::Mat threshImage;
    QTimer *timer;         // Timer to refresh frames
};

#endif
