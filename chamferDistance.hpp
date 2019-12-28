
#ifndef CHAMFERDISTANCE_H
#define CHAMFERDISTANCE_H

#include <vector>

class chamferDistance {
private:
    std::vector<std::vector<int>> tab = {{1,0,5},{1,1,7},{2,1,11}};
    int normalizer;
    int chamferWidth;
    int chamferHeigth;

public:
    chamferDistance();
    void testAndSet(std::vector<std::vector<double>> output, int x, int y, double newvalue);
    std::vector<std::vector<double>> calcul(std::vector<std::vector<bool>> input, int width, int height);
};

#endif //CHAMFERDISTANCE_H
