#include "chamferDistance.hpp"

chamferDistance::chamferDistance(): normalizer(5), chamferWidth(0), chamferHeigth(0){};
void chamferDistance::testAndSet(std::vector<std::vector<double>> output, int x, int y, double newvalue){
    if(x<0 || x>=this->chamferWidth) return;
    if(y<0 || y>=this->chamferHeigth) return;
    double v = output[x][y];
    if (v>=0 && v<newvalue) return;
    output[x][y] = newvalue;
};
std::vector<std::vector<double>> chamferDistance::calcul(std::vector<std::vector<bool>> input, int width, int height){
    this->chamferWidth = width;
    this->chamferHeigth = height;
    double output[width][height];

    // initialize distance
    for (int y=0; y<height; y++)
        for (int x=0; x<width; x++)
            if (  input[x][y] )
                output[x][y]=0; // inside the object -> distance=0
            else
                output[x][y]=-1; // outside the object -> to be computed

    // forward
    for (int y=0; y<=height-1; y++) {
        for (int x=0; x<=width-1; x++) {
            double v = output[x][y];
            if (v<0) continue;
            for(int k=0;k<tab.length;k++) {
                int dx = tab[k][0];
                int dy = tab[k][1];
                int dt = tab[k][2];

                testAndSet(output, x+dx, y+dy, v+dt);
                if (dy!=0) testAndSet(output, x-dx, y+dy, v+dt);
                if (dx!=dy) {
                    testAndSet(output, x+dy, y+dx, v+dt);
                    if (dy!=0) testAndSet(output, x-dy, y+dx, v+dt);
                }
            }
        }
    }

    // backward
    for (int y=height-1; y>=0; y--) {
        for (int x=width-1; x>=0; x--) {
            double v = output[x][y];
            if (v<0) continue;
            for(int k=0;k<tab.length;k++) {
                int dx = tab[k][0];
                int dy = tab[k][1];
                int dt = tab[k][2];

                testAndSet(output, x-dx, y-dy, v+dt);
                if (dy!=0) testAndSet(output, x+dx, y-dy, v+dt);
                if (dx!=dy) {
                    testAndSet(output, x-dy, y-dx, v+dt);
                    if (dy!=0) testAndSet(output, x+dy, y-dx, v+dt);
                }
            }
        }
    }

    // normalize
    for (int y=0; y<height; y++)
        for (int x=0; x<width; x++)
            output[x][y] = output[x][y]/normalizer;

    return output;
}
};
