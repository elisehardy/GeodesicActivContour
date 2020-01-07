//
// Created by ehardy on 12/4/19.
//

#ifndef UNTITLED2_POINT_H
#define UNTITLED2_POINT_H



class Point {
    
    public:
        int x;
        int y;
        
        Point(int _x, int _y);
        
        ~Point() = default;
        
        double distance(Point p);
        
        void setLocation(int _x, int _y);
};



#endif //UNTITLED2_POINT_H
