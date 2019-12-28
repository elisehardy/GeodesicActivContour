
#ifndef CHAMFERDISTANCE_H
#define CHAMFERDISTANCE_H

#include <vector>
#include <iostream>

class chamferDistance {
private:
    std::vector<std::vector<int>> tab = {{1,0,5},{1,1,7},{2,1,11}};
    int normalizer;
    int chamferWidth;
    int chamferHeigth;

public:
    chamferDistance();
    void testAndSet(double output[640][480], int x, int y, double newvalue);
    double** calcul(bool input[640][480], int width, int height);
};

#endif //CHAMFERDISTANCE_H
