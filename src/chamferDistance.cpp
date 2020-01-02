#include <chamferDistance.hpp>
#include <Snake.hpp>


chamferDistance::chamferDistance() :
    chamferWidth(0), chamferHeight(0) {
    this->normalizer = tab[0][2];
}


void chamferDistance::testAndSet(vector2D<double> &output, int x, int y, double newvalue) {
    if (x < 0 || x >= this->chamferWidth || y < 0 || y >= this->chamferHeight) {
        return;
    }
    
    double v = output[x][y];
    if (v >= 0 && v < newvalue) {
        return;
    }
    output[x][y] = newvalue;
}


vector2D<double> chamferDistance::calcul(bool **input, int width, int height) {
    vector2D<double> output = vector2D<double>(width, std::vector<double>(height));
    
    this->chamferWidth = width;
    this->chamferHeight = height;
    
    // initialize calculDistance
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (input[x][y]) {
                output[x][y] = 0; // inside the object -> calculDistance=0
            }
            else {
                output[x][y] = -1;
            } // outside the object -> to be computed
        }
    }
    std::cout << "forward" << std::endl;
    
    // forward
    for (int y = 0; y <= height - 1; y++) {
        for (int x = 0; x <= width - 1; x++) {
            double v = output[x][y];
            if (v < 0) {
                continue;
            }
            for (auto &k : tab) {
                int dx = k[0];
                int dy = k[1];
                int dt = k[2];
                
                testAndSet(output, x + dx, y + dy, v + dt);
                if (dy != 0) {
                    testAndSet(output, x - dx, y + dy, v + dt);
                }
                if (dx != dy) {
                    testAndSet(output, x + dy, y + dx, v + dt);
                    if (dy != 0) {
                        testAndSet(output, x - dy, y + dx, v + dt);
                    }
                }
            }
        }
    }
    
    std::cout << "backward" << std::endl;
    // backward
    for (int y = height - 1; y >= 0; y--) {
        for (int x = width - 1; x >= 0; x--) {
            double v = output[x][y];
            
            if (v < 0) {
                continue;
            }
            
            for (auto &k : tab) {
                int dx = k[0];
                int dy = k[1];
                int dt = k[2];
                
                testAndSet(output, x - dx, y - dy, v + dt);
                if (dy != 0) {
                    testAndSet(output, x + dx, y - dy, v + dt);
                }
                if (dx != dy) {
                    testAndSet(output, x - dy, y - dx, v + dt);
                    if (dy != 0) {
                        testAndSet(output, x + dy, y - dx, v + dt);
                    }
                }
            }
        }
    }
    
    std::cout << "normalize" << std::endl;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            output[x][y] = output[x][y] / normalizer;
        }
    }
    
    return output;
}

