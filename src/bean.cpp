#include <opencv2/imgproc.hpp>
#include <iostream>
#include <algorithm> // std::min_element


#include "bean.h"


Bean::Bean(int n_bean, std::vector<cv::Point> contours, cv::Mat* img_rgb){
    _id = n_bean;
    _beanContours = std::move(contours);
    _img = img_rgb;
}

Bean::~Bean() = default;

int Bean::getId() const {
    return _id;
}

cv::Point Bean::getPosition() const {
    return _centroid;
}

void Bean::binaryDraw(cv::Mat *im, int i, std::vector<std::vector<cv::Point>> *full_contours, std::vector<cv::Vec4i>* hierarchy) {
    drawContours(*im, *full_contours, i, cv::Scalar(255), -2, 8, *hierarchy, 0, cv::Point());
}

void Bean::countBlobsDraw(cv::Mat *im, int i, std::vector<std::vector<cv::Point>> *full_contours, int n_regions, std::vector<cv::Vec4i>* hierarchy) {
    drawContours(*im, *full_contours, i, cv::Scalar(n_regions), -2, 8, *hierarchy, 0, cv::Point());
}

void Bean::colourDraw(cv::Mat *im, int i, std::vector<std::vector<cv::Point>> *full_contours, std::vector<cv::Vec4i>* hierarchy) {
    cv::Scalar colour = cv::Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    drawContours(*im, *full_contours, i, colour, -2, 8, *hierarchy, 0, cv::Point());
}


void Bean::calculateConvexityDefects(std::vector<int> hull, std::vector<cv::Vec4i> conv_def,
                               std::vector<std::vector<cv::Point>> hull_points, int i, cv::Mat *img){

    convexHull(_beanContours, hull, false);
    convexityDefects(_beanContours, hull, conv_def);
    int defect = 0;
    for(auto const& ind : hull)
    {
        hull_points[i].push_back(_beanContours[ind]);
    }
    for(auto const& def : conv_def)
    {
        if(def[3] > 600) { // filter defects by depth
            defect++;

            int ind_0 = def[0];
            int ind_1 = def[1];
            int ind_2 = def[2];

            //draw shape and its possible deformities
            cv::circle(*img, _beanContours[ind_0], 3, cv::Scalar(0, 255, 0), -1);
            cv::circle(*img, _beanContours[ind_1], 3, cv::Scalar(0, 255, 0), -1);
            cv::circle(*img, _beanContours[ind_2], 3, cv::Scalar(0, 0, 255), -1);
            cv::line(*img, _beanContours[ind_2], _beanContours[ind_0], cv::Scalar(0, 255, 255), 1);
            cv::line(*img, _beanContours[ind_2], _beanContours[ind_1], cv::Scalar(0, 255, 255), 1);
        }

    }
    //cout << shape_issues << endl;
    cv::drawContours( *img, hull_points, i, cv::Scalar(255,255,255), 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point() );
    _shapeIssues = defect;
}

void Bean::calculateCentroid() {
    cv::Scalar cent = cv::mean(_pointList);
    //std::cout << int(cent[0]) << ", " << int(cent[1]) << " id " << _id <<  " damages " << _shapeIssues << std::endl;
    _centroid = cv::Point(int(cent[1]), int(cent[0]));
}

void Bean::drawCentroid(cv::Mat *im) const{
    //cv::circle(*im, _centroid, 3, cv::Scalar(247, 63, 234), -1);
    cv::drawMarker(*im, _centroid, cv::Scalar(0, 0, 255), cv::MARKER_TILTED_CROSS, 10, 2, cv::LINE_AA);
}

void Bean::drawId(cv::Mat *im) const {
    // get upper left corner from point list
    cv::Rect bbox = cv::boundingRect(_pointList);
    // right order in this case is point(y, x) ... because of opencv internal (height, width)
    auto upperleft = cv::Point(bbox.tl().y, bbox.tl().x);

    cv::putText(*im, "iD: " + std::to_string(_id), upperleft, cv::FONT_HERSHEY_SIMPLEX, 0.5, (0));
}
