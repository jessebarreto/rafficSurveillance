#include "bgmog2detector.h"

#include "trafficsurveillancecommon.h"
#include "trafficdebug.h"

BGMOG2Detector::BGMOG2Detector(int historySize, float thresholdBGModel, bool shadowDetectionEnabled, double learningRate,
               int thresholdValue, int morphBoxSize) :
    _historyValue(historySize),
    _thresholdBGModel(thresholdBGModel),
    _shadowDetectionFlag(shadowDetectionEnabled),
    _learningRate(learningRate),
    _thresholdValue(thresholdValue),
    _morphboxSize(morphBoxSize),
    _ptrSubtractor(nullptr),
    _startFlag(true),
    _showFGMask(false),
    _showBGMask(false),
    _showBinarizedFGMask(true),
    _showMorphFGMask(true),
    _fgMaskWinName("Foreground Mask"),
    _bgMaskWinName("Background Mask"),
    _binarizedFGMaskWinName("Foreground Binarized"),
    _morphFGMaskWinName("Morphed FG Mask")
{
}

void BGMOG2Detector::process(const cv::Mat &frame, cv::Mat &result)
{
    if (_startFlag) {
        result = cv::Mat::zeros(frame.size(), CV_8UC1);

        _ptrSubtractor = new cv::BackgroundSubtractorMOG2(_historyValue, _thresholdBGModel, _shadowDetectionFlag);
        _ptrSubtractor->operator ()(frame, _fgMask, _learningRate);

        if (_showFGMask) {
            setWindow(_fgMaskWinName);
        }

        if (_showBGMask) {
            setWindow(_bgMaskWinName);
        }

        if (_showBinarizedFGMask) {
            setWindow(_binarizedFGMaskWinName);
        }

        if (_showMorphFGMask) {
            setWindow(_morphFGMaskWinName);
//            cv::createTrackbar("Morph Size", _morphFGMaskWinName, &_morphboxSize, 100,
//                               _filterParamMorphSettingHandler, static_cast<void *>(&_morphboxSize));
        }

        _startFlag = false;
    } else {
        // Obtain foreground
        _ptrSubtractor->operator ()(frame, _fgMask, _learningRate);
        if (_showFGMask) {
            cv::imshow(_fgMaskWinName, _fgMask);
        }

        // Obtain background
        _ptrSubtractor->getBackgroundImage(_bgMask);
        if (_showBGMask) {
            cv::imshow(_bgMaskWinName, _bgMask);
        }

        // Apply Threshold
        cv::Mat binarizedFGMask;
        cv::threshold(_fgMask, binarizedFGMask, _thresholdValue, 255, cv::THRESH_BINARY);
        if (_showBinarizedFGMask) {
            cv::imshow(_binarizedFGMaskWinName, binarizedFGMask);
        }

        // Apply Morph filter
        _morphFilter(binarizedFGMask, result, cv::MORPH_OPEN, _morphboxSize);
        if (_showMorphFGMask) {
            cv::imshow(_morphFGMaskWinName, result);
        }
    }
}

void BGMOG2Detector::setMorphSize(int morphSize)
{
    _morphboxSize = morphSize;
}

void BGMOG2Detector::init()
{

}

void BGMOG2Detector::_morphFilter(const cv::Mat &src, cv::Mat &dst, int operation, int structElementSize)
{
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(structElementSize, structElementSize));

    if (operation == cv::MORPH_OPEN) {
        // Remove Small Blobs
        cv::Mat opening;
        cv::morphologyEx(src, opening, cv::MORPH_OPEN, kernel);

        // Fill holes
        cv::Mat closing;
        cv::morphologyEx(opening, closing, cv::MORPH_CLOSE, kernel);

        // Merge adjacent big blobs
        cv::dilate(closing, dst, kernel, cv::Point(-1,-1), 2);
    } else {
        // Fill holes
        cv::Mat closing;
        cv::morphologyEx(src, closing, cv::MORPH_CLOSE, kernel);

        // Remove Small Blobs
        cv::Mat opening;
        cv::morphologyEx(closing, opening, cv::MORPH_OPEN, kernel);

        // Merge adjacent big blobs
        cv::dilate(opening, dst, kernel, cv::Point(-1,-1), 2);
    }
}

void BGMOG2Detector::_filterParamMorphSettingHandler(int value, void *data)
{
    int *morph = static_cast<int *>(data);
    *morph = std::max(*morph, 1);
}
