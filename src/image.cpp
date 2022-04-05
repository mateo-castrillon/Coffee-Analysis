#include <future>
#include "image.h"

//mask size, should be odd number, also depends on the padding option
int x = 3;


Image::Image(const std::string& filename) {
    _bgr = imread(filename, cv::IMREAD_COLOR);
    _filename = filename;
    // make a deep copy from original, since _bgr will be modified
    _original = _bgr.clone();
}

Image::~Image() {
    std::for_each(threads.begin(), threads.end(), [](std::thread &t) {
        t.join();
    });
}

cv::Mat Image::getOriginal() {
    return _original;
}

void Image::segmentBeans() {
    cv::cvtColor(_bgr, _ycbcr, cv::COLOR_BGR2YCrCb);
    //std::vector<cv::Mat> channelsYCC;
    cv::Mat channelsYCC[3];
    cv::split(_ycbcr, channelsYCC);
    _cb = channelsYCC[2];

    cv::threshold(_cb, _bw, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);
}

void Image::findContours() {
    cv::findContours(_bw, _contours, _hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
}

void Image::prepareMemberImages() {
    segmentBeans();
    findContours();

    // Generation of Variables for labeling regions-------------------------------------------
    _bw2 = cv::Mat::zeros(_cb.size(), CV_8U ); // binary image with small areas filtered
    _regions = cv::Mat::zeros(_cb.size(), CV_8U ); // single channel image with each blob numbered
    _blobs = cv::Mat::zeros(_cb.size(), CV_8UC3 ); // coloured blobs image

    cvtColor(_bgr, _gray, cv::COLOR_BGR2GRAY);
}

void Image::extractBeans(int nThreads){
    int n_regions = 0;
    std::vector<Bean> beanList;

    //Variables for convexity defects -----------------------------------------------------------
    std::vector<std::vector<int>> hull(_contours.size());
    std::vector<std::vector<cv::Vec4i>> convDef(_contours.size());
    std::vector<std::vector<cv::Point>> hullPoints(_contours.size());

    for(int i = 0; i<_contours.size(); i++)    {
        if (_contours[i].size() >= 50) //Filter by size all regions smaller than 50, depends on image size
        {
            // create each bean object and push into _beanList, while at it, chech its form for defects
            n_regions++;
            Bean bean(n_regions, _contours[i], &_bgr);
            bean.calculateConvexityDefects(hull[i], convDef[i], hullPoints, i, &_bgr);

            beanList.emplace_back(bean);

            // fill in the images for labelling regions
            Bean::binaryDraw(&_bw2, i, &_contours, &_hierarchy);
            Bean::countBlobsDraw(&_regions, i, &_contours, n_regions, &_hierarchy);
            Bean::colourDraw(&_blobs, i, &_contours, &_hierarchy);
        }
    }
    // _gray filtered by size
    //cv::bitwise_and(_gray, _bw2, _gray);
    //namedWindow( "Grayimage no background", cv::WINDOW_AUTOSIZE);
    //imshow( "Grayimage no background", _gray);

    cv::Mat mask = cv::Mat::zeros(x, x, CV_8U);
    std::cout  << _filename << " \t| Total number of beans:   "<< n_regions << std::endl;

    _beanList = beanList;

    if (nThreads == 1){
        //if only one thread do not use std::threads
        Image::darkRegionLoop(0, _regions.cols-1);
    }
    else if(nThreads <= 0){
        std::cout << "Invalid number of threads\n";
    }
    else{
        std::cout << "Dividing each image in some threads, this works but it is not recommended, since its slower!!\n";
        // separate image in multiple horizontal crops, each of them will be a thread
        // cut image in n threads
        int crop_w = _regions.cols / nThreads;
        int crop_w_start = 0;
        //std::cout << "width " << _regions.cols << std::endl;
        int crop_id = 0;

        int x1[nThreads] = {};
        int x2[nThreads] = {};
        for (int w = crop_w; w <= _regions.cols; w += crop_w) {
            crop_id++;
            if (crop_id == nThreads) {
                x1[crop_id - 1] = crop_w_start;
                x2[crop_id - 1] = _regions.cols;
            } else {
                x1[crop_id - 1] = crop_w_start;
                x2[crop_id - 1] = w - 1;
            }
            //std::cout << x1[crop_id - 1] << ", " << x2[crop_id - 1] << std::endl;
            threads.emplace_back(std::thread(&Image::darkRegionLoop, this, x1[crop_id - 1], x2[crop_id - 1]));

        }
        // wait for each crop thread
        for (auto &&t: threads) {
            t.join();
        }
    }

}

void Image::darkRegionLoop(int rangeX1, int rangeX2){

    cv::Mat regionsCrop = _regions(cv::Range(0, _regions.rows), cv::Range(rangeX1, rangeX2));
    cv::Mat grayCrop = _gray(cv::Range(0, _regions.rows), cv::Range(rangeX1, rangeX2));

    cv::Mat mask = cv::Mat::zeros(x, x, CV_8U);

    //auto ti = (double) cv::getTickCount();
// loop through all pixels to get dark regions and save to each bean instance its related pixels intensities
// and corresponding positions
    for(int j = 0+x; j<=grayCrop.cols-x; j++)
    {
        for(int i = 0+x; i<=grayCrop.rows-x; i++)
        {
            cv::Vec3b point = regionsCrop.at<cv::Vec3b>(i, j, 0);
            // when the point belongs to a region (point >0 in _regions (numbered blobs)) ===> proceed otherwise carry on shifting through
            if(point.val[0]> 0)
            {
                // push into the bean's _pointList and _values the respective positions and bgr values of each bean
                // _regions has 0 as background and 1 as first id, therefore -1 to get right bean id

                mask = grayCrop.rowRange(i,i+x).colRange(j,j+x);//fills mask from point
                cv::Scalar non_zero = countNonZero(mask); //counts non zero elements inside created mask
                cv::Scalar mean = sum(mask) / non_zero; //gets average from mask

                _mutex.lock();
                if(mean.val[0] < 60 ) //if average smaller than ===> point belongs to a dark region
                {
                    // colour the point where dark spots were observed
                    _bgr.at<cv::Vec3b>(i,j)[0] = 0;
                    _bgr.at<cv::Vec3b>(i,j)[1] = 255;
                    _bgr.at<cv::Vec3b>(i,j)[2] = 0;

                    _beanList[point.val[0] - 1]._affectedPixels++; //counts how many times this happened on a region
                }
                _beanList[point.val[0] -1]._pointList.emplace_back(cv::Point(i, j));
                /*
                beanList[point.val[0] -1]._intensities.emplace_back(cv::Point3d(_original.at<cv::Vec3b>(i,j)[0],
                                                                                 _original.at<cv::Vec3b>(i,j)[1],
                                                                                 _original.at<cv::Vec3b>(i,j)[2]));
                */
                _beanList[point.val[0] - 1]._beanSize++; //calculate region size
                _mutex.unlock();

            }
        }
    }
    //ti = (double) cv::getTickCount() - ti;
    //std::cout << "\n\tthread dark regions loop: " << ti * 1000. / cv::getTickFrequency() << " ms\n";

}


