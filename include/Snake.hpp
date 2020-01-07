#ifndef SNAKE_H
#define SNAKE_H

#include <vector>

#include <MLV/MLV_image.h>

#include <Snake.hpp>
#include <Point.hpp>
#include <vector>


template<class T>
using vector2D = std::vector<std::vector<T>>;



class Snake {
    public:
        static constexpr double ALPHA = 1.1; /**< Uniformity (higher value force an equals distance between points). */
        static constexpr double BETA = 1.2;  /**< Curvature (higher value force a smoother curvature). */
        static constexpr double GAMMA = 1.5; /** Flow (higher value force the gradient attraction). */
        static constexpr double DELTA = 3.0; /**< Inertia (higher value for to stick to the gradient). */
        
        static constexpr int ITERATION_MAX = 1000; /** Maximum number of iterations (if no convergence). */
        
        static constexpr int AUTO_LOOP = 10;
        static constexpr int AUTO_MIN = 8;
        static constexpr int AUTO_MAX = 16;
    
    private:
        std::vector<Point> snake;
        vector2D<int> gradient;
        vector2D<int> flow;
        MLV_Image *image;
        MLV_Image *imgFlow;
        int height;
        int width;
        
        vector2D<double> uniformityEnergy;
        vector2D<double> curvatureEnergy;
        vector2D<double> inertiaEnergy;
        vector2D<double> flowEnergy;
        double length = 0;
        
        static void normalizeEnergy(vector2D<double> &energy);
        
        void computeGradientAndFlow();
        
        double computeUniformity(Point previous, Point current);
        
        double computeCurvature(Point previous, Point current, Point next);
        
        double computeFlow(Point current, Point point);
        
        double computeInertia(Point current, Point p);
        
        void rebuild(int space);
        
        void removePoint(int min);
        
        void addPoint(int max);
        
        double getLength();
        
        bool step();
    
    public:
        
        Snake(MLV_Image *image, int t_width, int t_height, std::vector<Point> t_points);
        
        ~Snake() = default;
        
        int loop();
        
        void draw() const;
        
        std::vector<Point> getSnake() const;
};



#endif //SNAKE_H
