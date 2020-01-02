#ifndef CHAMFERDISTANCE_H
#define CHAMFERDISTANCE_H

#include <vector>
#include <iostream>


template<class T>
using vector2D = std::vector<std::vector<T>>;



class chamferDistance {
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
        
        chamferDistance();
        
        vector2D<double> calcul(bool **input, int width, int height);
};



#endif //CHAMFERDISTANCE_H
