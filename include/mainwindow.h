#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QSlider>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

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
    void onUploadButtonClicked();
    void onResetButtonClicked();
    void onChainModeToggled(bool checked);
    void onThreshButtonClicked();
    void GaussianBluring();
    void grayScaleConversion();
    void applyMedianFilter();
    void histogramsEqualization();
    void applyDilation();
    void applyErosion();

private:
    // Helper functions
    void activateSliders(QSlider* slider1, QSlider* slider2);
    QImage matToQImage(const cv::Mat& image);

    Ui::MainWindow *ui;

    // Image storage
    cv::Mat originalImage;     // Original uploaded image
    cv::Mat resizedImage;      // Resized image for display
    cv::Mat workingImage;      // Current working image for chaining

    // Operation output images
    cv::Mat grayScaledImage;
    cv::Mat gaussianBlurredImage;
    cv::Mat medianFilteredImage;
    cv::Mat equImage;
    cv::Mat erosionImage;
    cv::Mat dilationImage;
    cv::Mat threshImage;

    // Control flags
    bool isChainMode = false;
};

#endif // MAINWINDOW_H
