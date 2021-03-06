#include "simpletracker.h"

#include "trafficsurveillancecommon.h"

SimpleTracker::SimpleTracker(const cv::Size &minCarSize, const cv::Size &maxCarSize, int maxUnseenFrames, int maxSpeed) :
    _minCarSize(minCarSize),
    _maxCarSize(maxCarSize),
    _maxUnseenFrames(maxUnseenFrames),
    _maxSpeed(maxSpeed),
    _carCounter(0),
    _startFlag(true),
    _boundRectColor(CV_RGB(0,0,255))
{

}

int SimpleTracker::process(const cv::Mat &frame, const cv::Mat &srcBlobs, ImageLine &imageLine,
                                  std::vector<Car *> &cars)
{
    // Find Contours
    cv::findContours(srcBlobs, _carContours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Find Contours that fits a car
    std::vector<std::pair<cv::Point , cv::Rect>> sceneCars;
    for (size_t id = 0; id < _carContours.size(); id++) {
        cv::Rect boundRect = cv::boundingRect(_carContours.at(id));
        if (!_validateSceneCar(boundRect)) {
            continue;
        }
        sceneCars.push_back(std::make_pair(_getCarCentroid(boundRect), boundRect));
    }

    // Update the Existing Cars
    _updateCars(cars, sceneCars, imageLine);

    // Create New Cars with the new sceneCars
    for (std::pair<cv::Point , cv::Rect> &sceneCar : sceneCars) {
        cars.push_back(new Car(cars.size(), sceneCar.first, sceneCar.second));
    }

    // Count Cars that crosses the line
    for (Car *car : cars) {
        if (!car->wasCounted() && carCrossesLine(*car, imageLine)) {
            _carCounter++;
            car->countCar(true);
        }
    }

    // Remove cars which did not appear for a while
    for (size_t id = 0; id < cars.size(); id++) {
        if (cars.at(id)->getLastSeenFrame() >= _maxUnseenFrames) {
            delete cars[id];
            cars.erase(cars.begin() + id);
        }
    }

    _startFlag = false;
    return _carCounter;
}

bool SimpleTracker:: _validateSceneCar(const cv::Rect &boundRect)
{
    return (boundRect.width >= _minCarSize.width && boundRect.height >= _minCarSize.height &&
            boundRect.width < _maxCarSize.width && boundRect.height < _maxCarSize.height);
}

cv::Point SimpleTracker::_getCarCentroid(const cv::Rect &boundRect)
{
    int cx = boundRect.x + boundRect.width / 2;
    int cy = boundRect.y + boundRect.height / 2;

    return cv::Point(cx, cy);
}

void SimpleTracker::_updateCars(std::vector<Car *> &cars, std::vector<std::pair<cv::Point, cv::Rect> > &sceneCars, ImageLine &line)
{
    if (!cars.empty()) {
        for (Car *car : cars) {
            car->setMatched(false);
        }

        int idFrame = 0;
        for (std::pair<cv::Point , cv::Rect> &sceneCar : sceneCars) {
            double leastDistance = std::numeric_limits<double>::max();
            int indexLeastDistance = 0;
            for (size_t id = 0; id < cars.size(); id++) {
                double magnitude, angle;
                getVectorPolarCoord(sceneCar.first, cars[id]->getPredictedPosition(), &magnitude, &angle);
                if (magnitude < leastDistance) {
                    leastDistance = magnitude;
                    indexLeastDistance = id;
                }
            }

            if (leastDistance < cars[indexLeastDistance]->getCarDiagonalSize()) {
                cars[indexLeastDistance]->update(sceneCar.first, sceneCar.second);
                sceneCars.erase(sceneCars.begin() + idFrame);
                cars[indexLeastDistance]->setMatched(true);
            }
            idFrame++;
        }

        for (Car *car : cars) {
            if (!car->getMatched()) {
                car->notFounded();
            }
        }
    }
}
