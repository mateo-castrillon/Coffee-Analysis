# Coffee Demo

This repo shows a few samples of image processing anylysis on wet coffe beans.

![img1](assets/readme_dark.png)

It runs with C++17 and CMake together with [OpenCV 4.x](https://docs.opencv.org/master/d7/d9f/tutorial_linux_install.html).

Defects to be detected are false elliptical shapes (beans which have suffered from mechanichal damage and beans which were attacked by insects before the harvest.

The repo works with cmake:

```
mkdir build
cd build
cmake ..
make
./capstone
```

The algorithm is designed to load images from the folder `images/` and analyze them in multiple threads. This number of threads correspond to the batch size on the main file and can be changed. Tests have been done with up to 10 threads.

The algo also has an extra internal implementation of threads, where an image croped in different subimages and each of them is treated as a separate thread. This method works fine, but its speed is quite slow, compared to sequential execution. 






![img2](assets/readme_mech_damage.jpg)
