#ifndef CHAMFERDISTANCE_H
#define CHAMFERDISTANCE_H

#include <vector>
#include <iostream>


template<class T>
using vector2D = std::vector<std::vector<T>>;



class ChamferDistance {
    private:
        int tab[3][3] = {
            { 1, 0, 5 },
            { 1, 1, 7 },
            { 2, 1, 11 }
        };
        
        int normalizer;
        int chamferWidth;
        int chamferHeight;
        
        void testAndSet(vector2D<double> &output, int x, int y, double newvalue);
    
    public:
        
        ChamferDistance();
        
        vector2D<double> compute(bool **input, int width, int height);
};



#endif //CHAMFERDISTANCE_H
