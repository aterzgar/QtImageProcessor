#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QPixmap>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), timer(new QTimer(this))
{
    ui->setupUi(this);
    setWindowTitle("QtImageProcessor");

    // Set up button connections
    connect(ui->uploadButton, &QPushButton::clicked, this, &MainWindow::onUploadButtonClicked);
    connect(ui->threshButton, &QPushButton::clicked, this, &MainWindow::onThreshButtonClicked);
    connect(ui->grayButton, &QPushButton::clicked, this, &MainWindow::grayScaleConversion);
    connect(ui->gaussianButton, &QPushButton::clicked, this, &MainWindow::GaussianBluring);

    // Configure labels for displaying images
    ui->label->setScaledContents(true);
    ui->label->setAlignment(Qt::AlignCenter);

    ui->label_1->setScaledContents(true);
    ui->label_1->setAlignment(Qt::AlignCenter);

    // Configure threshold sliders
    ui->sliderLow->setRange(0, 255);
    ui->sliderLow->setValue(128);
    ui->sliderLow->setEnabled(false);

    ui->sliderHigh->setRange(0, 255);
    ui->sliderHigh->setValue(255);
    ui->sliderHigh->setEnabled(false);

    connect(ui->sliderLow, &QSlider::valueChanged, this, &MainWindow::onThreshButtonClicked);
    connect(ui->sliderHigh, &QSlider::valueChanged, this, &MainWindow::onThreshButtonClicked);

    // Configure Gaussian kernel slider
    ui->sliderKernel->setRange(3, 21);
    ui->sliderKernel->setValue(3);
    ui->sliderKernel->setSingleStep(2);
    ui->sliderKernel->setEnabled(false);

    connect(ui->sliderKernel, &QSlider::valueChanged, this, &MainWindow::GaussianBluring);

    // Configure the Median kernel slider
    ui->sliderMedian->setRange(3, 21); // Range: 3 to 21
    ui->sliderMedian->setSingleStep(2); // Increase by 2
    ui->sliderMedian->setValue(3);     // Default value: 3
    ui->sliderMedian->setEnabled(false); // Initially disabled

    connect(ui->medianButton, &QPushButton::clicked, this, &MainWindow::applyMedianFilter);
    connect(ui->sliderMedian, &QSlider::valueChanged, this, &MainWindow::applyMedianFilter);
    connect(ui->histogramButton, &QPushButton::clicked, this, &MainWindow::histogramsEqualization);

    // Configure sliders for erosion and dilation
    ui->sliderErosion->setRange(3, 21);  // Set range for kernel size
    ui->sliderErosion->setSingleStep(2); // Increase by 2
    ui->sliderErosion->setValue(3);      // Default value
    ui->sliderDilation->setRange(3, 21); // Set range for kernel size
    ui->sliderDilation->setSingleStep(2); // Increase by 2
    ui->sliderDilation->setValue(3);     // Default value
    ui->sliderErosion->setEnabled(false);
    ui->sliderDilation->setEnabled(false);

    connect(ui->dilationButton, &QPushButton::clicked, this, &MainWindow::applyDilation);
    connect(ui->erosionButton, &QPushButton::clicked, this, &MainWindow::applyErosion);
    connect(ui->sliderErosion, &QSlider::valueChanged, this, &MainWindow::applyErosion);
    connect(ui->sliderDilation, &QSlider::valueChanged, this, &MainWindow::applyDilation);
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::activateSliders(QSlider* slider1, QSlider* slider2) {
    // Deactivate all sliders
    ui->sliderLow->setEnabled(false);
    ui->sliderHigh->setEnabled(false);
    ui->sliderKernel->setEnabled(false);
    ui->sliderMedian->setEnabled(false);
    ui->sliderErosion->setEnabled(false);
    ui->sliderDilation->setEnabled(false);

    // Activate the provided sliders
    if (slider1) slider1->setEnabled(true);
    if (slider2) slider2->setEnabled(true);
}

void MainWindow::onUploadButtonClicked()
{
    // Open file dialog to select image
    QString filePath = QFileDialog::getOpenFileName(this, "Select Image", QDir::homePath(), "Image Files (*.png *.jpg *.jpeg *.bmp)");

    if (!filePath.isEmpty()) {
        // Load the image using OpenCV
        cv::Mat originalImage = cv::imread(filePath.toStdString());

        if (!originalImage.empty()) {
            // Get the size of the QLabel
            QSize labelSize = ui->label->size();
            // Resize the image using OpenCV
            double aspectRatio = (double)originalImage .cols / (double)originalImage .rows;
            int newWidth = labelSize.width();
            int newHeight = labelSize.height();

            if (originalImage .cols > originalImage .rows) {
                newHeight = static_cast<int>(newWidth / aspectRatio);
            } else {
                newWidth = static_cast<int>(newHeight * aspectRatio);
            }

            cv::resize(originalImage , resizedImage, cv::Size(newWidth, newHeight));

            // Convert the OpenCV Mat to QImage (Qt uses RGB, OpenCV uses BGR)
            cv::cvtColor(resizedImage, resizedImage, cv::COLOR_BGR2RGB); // Convert to RGB
            QImage qImage(resizedImage.data, resizedImage.cols, resizedImage.rows, resizedImage.step, QImage::Format_RGB888);
            // Display the image in the QLabel
            ui->label->setPixmap(QPixmap::fromImage(qImage));
        }
    }
}

void MainWindow::grayScaleConversion() {

    if (resizedImage.empty()) {
        qWarning("No image available for conversion!");
        return;
    }

    // Convert the resized image to grayscale
    cv::cvtColor(resizedImage, grayScaledImage, cv::COLOR_RGB2GRAY);
    std:: cout << "Image converted grayscale" << std::endl;

    QImage qImage(grayScaledImage.data, grayScaledImage.cols, grayScaledImage.rows, grayScaledImage.step, QImage::Format_Grayscale8);

    // Display the thresholded image in the second QLabel
    ui->label_1->setPixmap(QPixmap::fromImage(qImage));
}


void MainWindow::onThreshButtonClicked()
{
    if (resizedImage.empty()) {
        qWarning("No image available for thresholding!");
        return;
    }

    if ((!ui->sliderHigh->isEnabled()) && (!ui->sliderLow->isEnabled())) {
        ui->sliderHigh->setEnabled(true);
        ui->sliderLow->setEnabled(true);
    }

    // Activate sliders for thresholding
    activateSliders(ui->sliderLow, ui->sliderHigh);

    // Get the current values of both sliders
    int lowValue = ui->sliderLow->value();
    int highValue = ui->sliderHigh->value();

    std::cout << "Low Value: " << lowValue<< "  High Value: " << highValue << std::endl;
    // Convert the resized image to grayscale
    cv::cvtColor(resizedImage, grayScaledImage, cv::COLOR_RGB2GRAY);

    // Apply binary thresholding
    cv::threshold(grayScaledImage, threshImage, lowValue, highValue, cv::THRESH_BINARY);

    // Convert to QImage
    QImage qImage(threshImage.data, threshImage.cols, threshImage.rows, threshImage.step, QImage::Format_Grayscale8);

    // Display the thresholded image in the second QLabel
    ui->label_1->setPixmap(QPixmap::fromImage(qImage));
}


void MainWindow::GaussianBluring() {

    if (resizedImage.empty()) {
        qWarning("No image available for bluring!");
        return;
    }

    // Enable kernel slider after button press
    if (!ui->sliderKernel->isEnabled()){
        ui->sliderKernel->setEnabled(true);
    }

    // Activate Gaussian kernel slider
    activateSliders(ui->sliderKernel, nullptr);

    // Get the kernel size from the slider
    int kernelSize = ui->sliderKernel->value();

    // Ensure kernel size is odd
        if (kernelSize % 2 == 0) {
            kernelSize += 1;
        }

    std::cout << "Gaussian Kernel Size: " << "(" <<kernelSize << ", " <<kernelSize<<")" << std::endl;

    cv::GaussianBlur(resizedImage, gaussianBlurredImage, cv::Size(kernelSize, kernelSize), 0);

    QImage qImage(gaussianBlurredImage.data, gaussianBlurredImage.cols, gaussianBlurredImage.rows, gaussianBlurredImage.step, QImage::QImage::Format_RGB888);

    // Display the thresholded image in the second QLabel
    ui->label_1->setPixmap(QPixmap::fromImage(qImage));
}

void MainWindow::applyMedianFilter()
{
    if (resizedImage.empty()) {
        qWarning("No image available for filtering!");
        return;
    }

    if (!ui->sliderMedian->isEnabled()){
        ui->sliderMedian->setEnabled(true);
    }

    // Activate median slider
    activateSliders(ui->sliderMedian, nullptr);

    // Get kernel size from the slider
    int kernelSize = ui->sliderMedian->value();
    // Ensure the kernel size is odd
    if (kernelSize % 2 == 0) {
        kernelSize += 1; // Increment to make it odd
    }
    std::cout << "Median Kernel Size: " << kernelSize << std::endl;
    // Apply the median filter
    cv::medianBlur(resizedImage,  medianFilteredImage, kernelSize);

    // Convert the filtered image to QImage
    QImage qImage(medianFilteredImage.data,  medianFilteredImage.cols,  medianFilteredImage.rows,  medianFilteredImage.step, QImage::Format_RGB888);

    // Display the filtered image in the second QLabel
    ui->label_1->setPixmap(QPixmap::fromImage(qImage));
}

void MainWindow::histogramsEqualization()
{
    if (resizedImage.empty()) {
        qWarning("No image available for filtering!");
        return;
    }

    // Convert the resized image to grayscale
    cv::cvtColor(resizedImage, grayScaledImage, cv::COLOR_RGB2GRAY);

    // Apply the median filter
    cv::equalizeHist(grayScaledImage, equImage);
    std::cout << "Histogram equalization" << std::endl;

    // Convert the filtered image to QImage
    QImage qImage(equImage.data, equImage.cols, equImage.rows,equImage.step, QImage::Format_Grayscale8);

    // Display the filtered image in the second QLabel
    ui->label_1->setPixmap(QPixmap::fromImage(qImage));
}

void MainWindow::applyDilation()
{
    if (resizedImage.empty()) {
        qWarning("No image available for dilation!");
        return;
    }

    // Enable Dilation slider after button press
    if (!ui->sliderDilation->isEnabled()){
        ui->sliderDilation->setEnabled(true);
    }

    // Activate sliders for dilation
    activateSliders(ui->sliderDilation, nullptr);

    // Get the kernel size from the slider
    int dilationSize = ui->sliderDilation->value();

    // Ensure kernel size is odd
    if (dilationSize % 2 == 0) {
        dilationSize += 1;
    }

    std::cout << "Dilation Kernel Size: " << "(" <<dilationSize << ", " <<dilationSize<<")" << std::endl;

    // Define the structuring element (kernel)
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilationSize, dilationSize));

    // Apply dilation
    cv::dilate(resizedImage, dilationImage, kernel);

    // Convert the dilated image to QImage
    QImage qImage(dilationImage.data, dilationImage.cols, dilationImage.rows, dilationImage.step, QImage::Format_RGB888);

    // Display the dilated image in the second QLabel
    ui->label_1->setPixmap(QPixmap::fromImage(qImage));
}

void MainWindow::applyErosion()
{
    if (resizedImage.empty()) {
        qWarning("No image available for erosion!");
        return;
    }

    if (!ui->sliderErosion->isEnabled()) {
        ui->sliderErosion->setEnabled(true);
    }

    // Activate sliders for erosion
    activateSliders(ui->sliderErosion, nullptr);

    // Get the kernel size from the slider
    int erosionSize = ui->sliderErosion->value();

    // Ensure kernel size is odd
    if (erosionSize % 2 == 0) {
        erosionSize += 1;
    }

    std::cout << "Erosion Kernel Size: " << "(" <<erosionSize << ", " <<erosionSize<<")" << std::endl;

    // Define the structuring element (kernel)
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(erosionSize, erosionSize));

    // Apply erosion
    cv::erode(resizedImage, erosionImage, kernel);

    // Convert the eroded image to QImage
    QImage qImage(erosionImage.data, erosionImage.cols, erosionImage.rows, erosionImage.step, QImage::Format_RGB888);

    // Display the eroded image in the second QLabel
    ui->label_1->setPixmap(QPixmap::fromImage(qImage));
}