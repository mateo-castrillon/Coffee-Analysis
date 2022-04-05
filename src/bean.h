#ifndef CAPSTONE_BEAN_H
#define CAPSTONE_BEAN_H
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

static cv::RNG rng(12345);


class Bean {
public:
    Bean(int n_bean, std::vector<cv::Point> contours, cv::Mat* img_rgb);
    ~Bean();

    cv::Point getPosition() const;
    int getId() const;

    bool _damaged = false;
    double _darkArea = 0.0;
    double _beanSize = 0.0;
    int _shapeIssues = 0;
    int _affectedPixels = 0;

    std::vector<cv::Point> _pointList;
    //std::vector<cv::Point3d> _intensities;

    static void binaryDraw(cv::Mat *im, int i, std::vector<std::vector<cv::Point>> *full_contours,
                    std::vector<cv::Vec4i>* hierarchy);
    static void countBlobsDraw(cv::Mat *im, int i, std::vector<std::vector<cv::Point> > *full_contours, int n_regions,
                        std::vector<cv::Vec4i>* hierarchy);
    static void colourDraw(cv::Mat *im, int i, std::vector<std::vector<cv::Point> > *full_contours,
                    std::vector<cv::Vec4i>* hierarchy);

    void calculateConvexityDefects(std::vector<int> hull, std::vector<cv::Vec4i> conv_def,
                                   std::vector<std::vector<cv::Point>> hull_points, int i, cv::Mat *img);

    void calculateCentroid();
    void drawCentroid(cv::Mat *im) const;
    void drawId(cv::Mat *im) const;

    cv::Mat* getImgPtr();

private:
    //Centroid _position;

    int _id;
    cv::Point _centroid;
    cv::Mat* _img = nullptr;
    std::vector<cv::Point> _beanContours;


};


#endif //CAPSTONE_BEAN_H
