# QtImageProcessor

A Qt-based application for image processing using OpenCV.

## Features
- Upload and display images.
- Apply thresholding, Gaussian blur, grayscale conversion, median filtering, and more.

![User Interface](images/ui_screenshot.png)

## Prerequisites
- Qt 5.15.3
- OpenCV 4.7.0

## Build Instructions
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/QtImageProcessor.git
   cd QtImageProcessor
   mkdir build 
   cmake .. 
   cmake --build .