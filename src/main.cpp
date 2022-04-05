#include <iostream>
#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <filesystem>
#include <future>
#include <iomanip>

#include "bean.h"
#include "utils.h"
#include "vutils.h"
#include "image.h"


Image& threadFn(const std::string& path){
    auto t0 = (double) cv::getTickCount();
    //TODO try a unique pointer here
    auto* img = new Image(path);

    if (img->_bgr.empty())
    {
        cerr << "Can't read image from the file: " << path << endl;
        exit(-1);
    }
    img->prepareMemberImages();

    //// using threads in this case does not bring any benefits, leave at 1!
    img->extractBeans(1);

    for (auto & bean : img->_beanList){
        bean.calculateCentroid();
        bean.drawId(&img->_bgr);

        bean._darkArea = bean._affectedPixels / bean._beanSize;
        bean._darkArea = bean._affectedPixels / bean._beanSize;

        //std::cout <<"Bean id " << bean.getId() << " Size: " << bean._beanSize << "\t-> Affectation index " <<
        //          bean._darkArea << "\t-> Mech damages " << bean._shapeIssues << std::endl;

        if (bean._darkArea > 0 || bean._shapeIssues > 0) {
            bean.drawCentroid(&img->_bgr);
            bean._damaged = true;
            // paint in red to cout whenever a bean has to be sorted out, also output its centroid (target)
            //std::cout << "\033[1;31m Rejected bean with id " << bean.getId() <<  " at position: " << bean.getPosition() << "\033[0m"<< std::endl;
        }
    }

    t0 = (double) cv::getTickCount() - t0;
    std::cout << "Thread time: " <<  t0 * 1000. / cv::getTickFrequency() << " ms\n" ;

    return *img;
}

void printNumber(int x) {
    std::cout << "X:" << std::setw(6) << x << ":X\n";
}

void printStuff() {
    printNumber(528);
    printNumber(3);
    printNumber(73826);
    printNumber(37);
}

int main() {
    std::vector<std::thread> threads;

    std::string img_name;
    string path = GetPreviousPathDir(GetCurrentBuildDir());
    std::vector<std::string> paths;

    for (const auto& dirEntry : std::filesystem::recursive_directory_iterator("../images/")) {
        paths.emplace_back(dirEntry.path());
    }

    int batch_size = 2;
    auto namesPartition = vutils::partition(paths.begin(), paths.end(), batch_size);

    for (unsigned batchCount = 0; batchCount < namesPartition.size(); ++batchCount) {
        auto tt = (double) cv::getTickCount();

        auto batch = namesPartition[batchCount];
        std::vector<std::future<Image&>> ftr_results;

        std::cout << std::setfill('-');
        std::cout << std::endl <<  std::setw(55) << "-";
        std::cout << "Batch #" << batchCount + 1 << " initializing..." << std::endl;

        for (const auto &item: batch) {
            std::cout << "  Thread processing: " << item << std::endl;
            ftr_results.emplace_back(std::async(std::launch::async, threadFn, item));
        }

        // wait for the results
        std::for_each(ftr_results.begin(), ftr_results.end(), [](std::future<Image &> &ftr) {
            ftr.wait();
        });
        // print total time
        tt = (double) cv::getTickCount() - tt;

        // from this loop more public members from image class (result) can be taken.
        // the loop is used just to show the results
        for (auto &ftr: ftr_results) {
            //cv::Mat bgr = ftr.get()._bgr;
            // move from future to result obj
            auto &&result = std::move(ftr.get());
            std::vector<Bean> beanList = result._beanList;

            std::cout << std::setfill('-');
            std::cout << std::endl <<  std::setw(20) << "-";
            std::cout << " Showing beans results from image: " << result._filename << std::endl;

            // loop through beans on a given result image to show their scores
            for (auto &bean: beanList) {
                std::cout << std::setfill(' ');
                //std::cout << "Left-aligned\n";
                std::cout << std::left;
                std::cout << "Bean id " << std::setw(4) << bean.getId() << " Size: " << std::setw(7) << bean._beanSize;
                std::cout << "-> Affectation index " << std::setw(12) << bean._darkArea;
                std::cout << " -> Mech damages " << bean._shapeIssues << "\n";


                // show in red which ones are damaged and where their center is
                if (bean._damaged) {
                    // paint in red to cout whenever a bean has to be sorted out, also output its centroid (target)
                    std::cout << "\033[1;31m Rejected bean with id " << bean.getId() << " at position: "
                              << bean.getPosition() << "\033[0m" << std::endl;
                }
            }

            // display results for each image in that batch
            cv::namedWindow("result", cv::WINDOW_AUTOSIZE);
            cv::imshow("result", result._bgr);
            cv::waitKey(0);
        }
        std::cout << std::setfill('-');
        std::cout << std::endl <<  std::setw(55) << "-";
        std::cout << "Batch #"  << batchCount + 1 << " terminated" << std::endl;
        std::cout << "\tTotal time: " << tt * 1000. / cv::getTickFrequency() << " ms\n";

    }

    return 0;
}
