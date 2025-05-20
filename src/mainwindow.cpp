#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QPixmap>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QDebug>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("QtImageProcessor");

    // Connect buttons to their handlers
    connect(ui->uploadButton, &QPushButton::clicked, this, &MainWindow::onUploadButtonClicked);
    connect(ui->threshButton, &QPushButton::clicked, this, &MainWindow::onThreshButtonClicked);
    connect(ui->grayButton, &QPushButton::clicked, this, &MainWindow::grayScaleConversion);
    connect(ui->gaussianButton, &QPushButton::clicked, this, &MainWindow::GaussianBluring);
    connect(ui->medianButton, &QPushButton::clicked, this, &MainWindow::applyMedianFilter);
    connect(ui->histogramButton, &QPushButton::clicked, this, &MainWindow::histogramsEqualization);
    connect(ui->dilationButton, &QPushButton::clicked, this, &MainWindow::applyDilation);
    connect(ui->erosionButton, &QPushButton::clicked, this, &MainWindow::applyErosion);
    connect(ui->resetButton, &QPushButton::clicked, this, &MainWindow::onResetButtonClicked);
    connect(ui->chainModeCheckBox, &QCheckBox::toggled, this, &MainWindow::onChainModeToggled);

    ui->label->setScaledContents(true);
    ui->label->setAlignment(Qt::AlignCenter);
    ui->label_1->setScaledContents(true);
    ui->label_1->setAlignment(Qt::AlignCenter);

    ui->sliderLow->setRange(0, 255);
    ui->sliderLow->setValue(128);
    ui->sliderHigh->setRange(0, 255);
    ui->sliderHigh->setValue(255);

    ui->sliderKernel->setRange(3, 21);
    ui->sliderKernel->setValue(3);
    ui->sliderKernel->setSingleStep(2);

    ui->sliderMedian->setRange(3, 21);
    ui->sliderMedian->setValue(3);
    ui->sliderMedian->setSingleStep(2);

    ui->sliderErosion->setRange(3, 21);
    ui->sliderErosion->setValue(3);
    ui->sliderErosion->setSingleStep(2);
    ui->sliderDilation->setRange(3, 21);
    ui->sliderDilation->setValue(3);
    ui->sliderDilation->setSingleStep(2);

    connect(ui->sliderLow, &QSlider::valueChanged, this, &MainWindow::onThreshButtonClicked);
    connect(ui->sliderHigh, &QSlider::valueChanged, this, &MainWindow::onThreshButtonClicked);
    connect(ui->sliderKernel, &QSlider::valueChanged, this, &MainWindow::GaussianBluring);
    connect(ui->sliderMedian, &QSlider::valueChanged, this, &MainWindow::applyMedianFilter);
    connect(ui->sliderErosion, &QSlider::valueChanged, this, &MainWindow::applyErosion);
    connect(ui->sliderDilation, &QSlider::valueChanged, this, &MainWindow::applyDilation);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::activateSliders(QSlider* slider1, QSlider* slider2) {
    ui->sliderLow->setEnabled(false);
    ui->sliderHigh->setEnabled(false);
    ui->sliderKernel->setEnabled(false);
    ui->sliderMedian->setEnabled(false);
    ui->sliderErosion->setEnabled(false);
    ui->sliderDilation->setEnabled(false);
    if (slider1) slider1->setEnabled(true);
    if (slider2) slider2->setEnabled(true);
}

QImage MainWindow::matToQImage(const cv::Mat& image) {
    if (image.empty()) {
        qWarning("Cannot convert empty Mat to QImage.");
        return QImage();
    }
    if (image.channels() == 3) {
        return QImage(image.data, image.cols, image.rows, image.step, QImage::Format_RGB888);
    } else if (image.channels() == 1) {
        return QImage(image.data, image.cols, image.rows, image.step, QImage::Format_Grayscale8);
    }
    return QImage();
}

void MainWindow::onChainModeToggled(bool checked) {
    isChainMode = checked;
    if (checked && !ui->operationsTextEdit->toPlainText().isEmpty()) {
        ui->operationsTextEdit->clear();
    }
}

void MainWindow::onResetButtonClicked() {
    if (originalImage.empty()) {
        qWarning("Cannot reset: No original image loaded.");
        return;
    }
    QSize labelSize = ui->label->size();
    cv::Mat tempResizedImage;
    if (!originalImage.empty()) {
        double aspectRatio = (double)originalImage.cols / (double)originalImage.rows;
        int newWidth = labelSize.width();
        int newHeight = labelSize.height();
        if (originalImage.cols > originalImage.rows) {
            newHeight = static_cast<int>(newWidth / aspectRatio);
        } else {
            newWidth = static_cast<int>(newHeight * aspectRatio);
        }
        cv::resize(originalImage, tempResizedImage, cv::Size(newWidth, newHeight));
    } else {
        qWarning("Original image became empty unexpectedly during reset.");
        resizedImage.release();
        workingImage.release();
        ui->label->clear();
        ui->label_1->clear();
        ui->operationsTextEdit->clear();
        activateSliders(nullptr, nullptr);
        return;
    }
    cv::cvtColor(tempResizedImage, resizedImage, cv::COLOR_BGR2RGB);
    workingImage = resizedImage.clone();
    isChainMode = false;
    ui->chainModeCheckBox->setChecked(false);

    QImage qImage = matToQImage(resizedImage);
    ui->label->setPixmap(QPixmap::fromImage(qImage));
    ui->label_1->clear();
    ui->operationsTextEdit->clear();
    activateSliders(nullptr, nullptr);
}

void MainWindow::onUploadButtonClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Select Image", QDir::homePath(), "Image Files (*.png *.jpg *.jpeg *.bmp)");
    if (!filePath.isEmpty()) {
        originalImage = cv::imread(filePath.toStdString());
        if (!originalImage.empty()) {
            QSize labelSize = ui->label->size();
            double aspectRatio = (double)originalImage.cols / (double)originalImage.rows;
            int newWidth = labelSize.width();
            int newHeight = labelSize.height();
            if (originalImage.cols > originalImage.rows) {
                newHeight = static_cast<int>(newWidth / aspectRatio);
            } else {
                newWidth = static_cast<int>(newHeight * aspectRatio);
            }
            cv::resize(originalImage, resizedImage, cv::Size(newWidth, newHeight));
            cv::cvtColor(resizedImage, resizedImage, cv::COLOR_BGR2RGB);
            workingImage = resizedImage.clone();
            ui->operationsTextEdit->clear();

            QImage qImage = matToQImage(resizedImage);
            ui->label->setPixmap(QPixmap::fromImage(qImage));
            ui->label_1->clear();

            isChainMode = false;
            ui->chainModeCheckBox->setChecked(false);
            activateSliders(nullptr, nullptr);
        } else {
            QMessageBox::critical(this, "Error", "Could not open or find the image.");
        }
    }
}

void MainWindow::grayScaleConversion() {
    cv::Mat inputImage = isChainMode ? workingImage : resizedImage;
    if (inputImage.empty()) {
        qWarning("No image available for grayscale conversion!");
        ui->label_1->clear();
        return;
    }

    activateSliders(nullptr, nullptr);

    cv::Mat outputImage;
    if (inputImage.channels() == 3 || inputImage.channels() == 4) {
        cv::cvtColor(inputImage, outputImage, cv::COLOR_RGB2GRAY);
    } else if (inputImage.channels() == 1) {
        outputImage = inputImage.clone();
    }  else {
        qWarning("Unsupported channel count for grayscale conversion: %d", inputImage.channels());
        return;
    }

    grayScaledImage = outputImage;
    if (isChainMode) {
        workingImage = grayScaledImage.clone();
        QString currentOps = ui->operationsTextEdit->toPlainText();
        currentOps += "→ Grayscale Conversion\n";
        ui->operationsTextEdit->setPlainText(currentOps);
    }

    QImage qImage = matToQImage(grayScaledImage);
    ui->label_1->setPixmap(QPixmap::fromImage(qImage));
}

void MainWindow::onThreshButtonClicked() {
    cv::Mat inputImage = isChainMode ? workingImage : resizedImage;
    if (inputImage.empty()) {
        qWarning("No image available for thresholding!");
        ui->label_1->clear();
        return;
    }

    activateSliders(ui->sliderLow, ui->sliderHigh);

    int lowValue = ui->sliderLow->value();
    int highValue = ui->sliderHigh->value();

    cv::Mat grayInput;
    if (inputImage.channels() == 3) {
        cv::cvtColor(inputImage, grayInput, cv::COLOR_RGB2GRAY);
    } else if (inputImage.channels() == 1) {
        grayInput = inputImage.clone();
    } else {
         qWarning("Unsupported channel count for thresholding: %d", inputImage.channels());
         return;
    }

    cv::threshold(grayInput, threshImage, lowValue, highValue, cv::THRESH_BINARY);

    if (isChainMode) {
        workingImage = threshImage.clone();
        QString currentOps = ui->operationsTextEdit->toPlainText();
        currentOps += QString("→ Threshold (low=%1, high=%2)\n").arg(lowValue).arg(highValue);
        ui->operationsTextEdit->setPlainText(currentOps);
    }

    QImage qImage = matToQImage(threshImage);
    ui->label_1->setPixmap(QPixmap::fromImage(qImage));
}

void MainWindow::GaussianBluring() {
    cv::Mat inputImage = isChainMode ? workingImage : resizedImage;
    if (inputImage.empty()) {
        qWarning("No image available for blurring!");
        ui->label_1->clear();
        return;
    }

    activateSliders(ui->sliderKernel, nullptr);

    int kernelSize = ui->sliderKernel->value();
    if (kernelSize % 2 == 0) { kernelSize += 1; }
    if (kernelSize < 3) kernelSize = 3;

    cv::GaussianBlur(inputImage, gaussianBlurredImage, cv::Size(kernelSize, kernelSize), 0, 0);
    if (isChainMode) {
        workingImage = gaussianBlurredImage.clone();
        QString currentOps = ui->operationsTextEdit->toPlainText();
        currentOps += QString("→ Gaussian Blur (kernel=%1)\n").arg(kernelSize);
        ui->operationsTextEdit->setPlainText(currentOps);
    }

    QImage qImage = matToQImage(gaussianBlurredImage);
    ui->label_1->setPixmap(QPixmap::fromImage(qImage));
}

void MainWindow::applyMedianFilter() {
    cv::Mat inputImage = isChainMode ? workingImage : resizedImage;
    if (inputImage.empty()) {
        qWarning("No image available for median filtering!");
        ui->label_1->clear();
        return;
    }

    activateSliders(ui->sliderMedian, nullptr);

    int kernelSize = ui->sliderMedian->value();
    if (kernelSize % 2 == 0) { kernelSize += 1; }
    if (kernelSize < 3) kernelSize = 3;

    cv::medianBlur(inputImage, medianFilteredImage, kernelSize);
    if (isChainMode) {
        workingImage = medianFilteredImage.clone();
        QString currentOps = ui->operationsTextEdit->toPlainText();
        currentOps += QString("→ Median Filter (kernel=%1)\n").arg(kernelSize);
        ui->operationsTextEdit->setPlainText(currentOps);
    }

    QImage qImage = matToQImage(medianFilteredImage);
    ui->label_1->setPixmap(QPixmap::fromImage(qImage));
}

void MainWindow::histogramsEqualization() {
    cv::Mat inputImage = isChainMode ? workingImage : resizedImage;
    if (inputImage.empty()) {
        qWarning("No image available for histogram equalization!");
        ui->label_1->clear();
        return;
    }

    activateSliders(nullptr, nullptr);

    cv::Mat grayInput;
    if (inputImage.channels() == 3) {
        cv::cvtColor(inputImage, grayInput, cv::COLOR_RGB2GRAY);
    } else if (inputImage.channels() == 1) {
        grayInput = inputImage.clone();
    } else {
         qWarning("Unsupported channel count for histogram equalization: %d", inputImage.channels());
        return;
    }

    cv::equalizeHist(grayInput, equImage);
    if (isChainMode) {
        workingImage = equImage.clone();
        QString currentOps = ui->operationsTextEdit->toPlainText();
        currentOps += "→ Histogram Equalization\n";
        ui->operationsTextEdit->setPlainText(currentOps);
    }

    QImage qImage = matToQImage(equImage);
    ui->label_1->setPixmap(QPixmap::fromImage(qImage));
}

void MainWindow::applyDilation() {
    cv::Mat inputImage = isChainMode ? workingImage : resizedImage;
    if (inputImage.empty()) {
        qWarning("No image available for dilation!");
        ui->label_1->clear();
        return;
    }

    activateSliders(ui->sliderDilation, nullptr);

    int dilationSize = ui->sliderDilation->value();
    if (dilationSize % 2 == 0) { dilationSize += 1; }
    if (dilationSize < 1) dilationSize = 1;

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(dilationSize, dilationSize));
    cv::dilate(inputImage, dilationImage, kernel);

    if (isChainMode) {
        workingImage = dilationImage.clone();
        QString currentOps = ui->operationsTextEdit->toPlainText();
        currentOps += QString("→ Dilation (kernel=%1)\n").arg(dilationSize);
        ui->operationsTextEdit->setPlainText(currentOps);
    }

    QImage qImage = matToQImage(dilationImage);
    ui->label_1->setPixmap(QPixmap::fromImage(qImage));
}

void MainWindow::applyErosion() {
    cv::Mat inputImage = isChainMode ? workingImage : resizedImage;
    if (inputImage.empty()) {
        qWarning("No image available for erosion!");
        ui->label_1->clear();
        return;
    }

    activateSliders(ui->sliderErosion, nullptr);

    int erosionSize = ui->sliderErosion->value();
     if (erosionSize % 2 == 0) { erosionSize += 1; }
     if (erosionSize < 1) erosionSize = 1;

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(erosionSize, erosionSize));
    cv::erode(inputImage, erosionImage, kernel);

    if (isChainMode) {
        workingImage = erosionImage.clone();
        QString currentOps = ui->operationsTextEdit->toPlainText();
        currentOps += QString("→ Erosion (kernel=%1)\n").arg(erosionSize);
        ui->operationsTextEdit->setPlainText(currentOps);
    }

    QImage qImage = matToQImage(erosionImage);
    ui->label_1->setPixmap(QPixmap::fromImage(qImage));
}
