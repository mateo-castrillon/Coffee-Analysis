#ifndef CAPSTONE_CENTROID_H
#define CAPSTONE_CENTROID_H


class Centroid {
public:
    Centroid(int posX, int posY) : _posX(posX), _posY(posY) {};
    ~Centroid() = default;
private:
    int _posX, _posY;
};


#endif //CAPSTONE_CENTROID_H
