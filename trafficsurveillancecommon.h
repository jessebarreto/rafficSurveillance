#ifndef TRAFFICSURVEILLANCECOMMON_H
#define TRAFFICSURVEILLANCECOMMON_H

// OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/opencv_modules.hpp>

// STL
#include <vector>
#include <iostream>
#include <chrono>

// User Libraries
#include "imageline.h"
#include "car.h"
#include "trafficdebug.h"
#include "trafficvideo.h"
#include "trafficimagedataset.h"
#include "virtualdetection.h"
#include "virtualtrack.h"

/*!
 * \brief MAIN WINDOW
 */
static const std::string mainWindow("Video Window");

/*!
 * \brief Run application setup
 */
int runSetup(cv::VideoCapture &video, ImageLine &line, int captureAreaSize);

/*!
 * \brief Run Video in a Loop
 */
void runVideo(cv::VideoCapture &video, cv::Mat &frame, ImageLine &imageLine, std::vector<Car> &cars, bool loop,
              int videoSpeed, bool hasTimer, VirtualDetection *indetificator, VirtualTrack *tracker);

/*!
 * \brief Set a window
 */
void setWindow(const std::string &winName);

/*!
 * \brief drawLineHandler callback function
 */
void drawLineHandler(int event, int x, int y, int flags, void* ptrLine);

/*!
 * \brief Calculates a vector from 2 points in polar coordinates
 */
void getVectorPolarCoord(const cv::Point &src, const cv::Point &dst, double *magnitude, double *angle);

#endif // TRAFFICSURVEILLANCECOMMON_H
