#ifndef CAPSTONE_IMAGE_H
#define CAPSTONE_IMAGE_H
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <thread>
#include <condition_variable>
#include <mutex>

#include "bean.h"
class Image {

public:
    Image(const std::string& filename);
    //Image &operator=(const Image &&source);
    ~Image();

    cv::Mat _bgr; // this is the Mat where result are drawn
    //variables for drawing some results
    cv::Mat _bw2; // binary image with small areas filtered
    cv::Mat _blobs; // coloured blobs image
    cv::Mat _regions; // single channel image with each blob numbered
    cv::Mat _gray;

    std::string _filename;
    std::vector<Bean> _beanList;
    std::vector<std::thread> threads;

    cv::Mat getOriginal(); // use this to get original image since its private

    void prepareMemberImages();
    void extractBeans(int nThreads);
    void darkRegionLoop(int rangeX1, int rangeX2);

private:
    std::vector<std::vector<cv::Point>> _contours;
    std::vector<cv::Vec4i> _hierarchy;
    cv::Mat _original;
    cv::Mat _ycbcr;
    cv::Mat _bw;
    cv::Mat _cb;

    void segmentBeans();
    void findContours();

    std::mutex _mutex;
};


#endif //CAPSTONE_IMAGE_H
