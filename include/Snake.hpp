#ifndef SNAKE_H
#define SNAKE_H

#include <vector>

#include <Snake.hpp>
#include <Point.hpp>


template<class T>
using vector2D = std::vector<std::vector<T>>;



class Snake {
    public:
        static constexpr double ALPHA = 1.1; /**< Uniformity (higher value force an equals distance between points). */
        static constexpr double BETA = 1.2; /**< Curvature (higher value force a smoother curvature). */
        static constexpr double GAMMA = 1.5; /** Flow (higher value force the gradient attraction). */
        static constexpr double DELTA = 3.0; /**< Inertia (higher value for to stick to the gradient). */
        
        static constexpr int ITERATION_MAX = 100; /** Maximum number of iterations (if no convergence). */
        
        static constexpr int AUTO_LOOP = 10;
        static constexpr int AUTO_MIN = 8;
        static constexpr int AUTO_MAX = 16;
        static constexpr bool AUTO = true;
    
    private:
        std::vector<Point> snake;
        vector2D<int> gradient;
        vector2D<int> flow;
        int height;
        int width;
        
        vector2D<double> uniformityEnergy = {};
        vector2D<double> curvatureEnergy = {};
        vector2D<double> inertiaEnergy = {};
        vector2D<double> flowEnergy = {};
        double length = 0;
        
        static void normalizeEnergy(vector2D<double> energy);
        
        double calculUniformity(Point previous, Point current, Point next);
        
        double calculCurvature(Point previous, Point current, Point next);
        
        double calculFlow(Point current, Point point);
        
        double calculInertia(Point current, Point p);
        
        void recreate(int space);
        
        void removePoint(int min);
        
        void addPoint(int max);
        
        double getLength();
        
        bool update();
    
    public:
        
        Snake(int t_width, int t_height, vector2D<int> t_gradient, vector2D<int> t_flow, std::vector<Point> t_points);
        
        ~Snake() = default;
        
        std::vector<Point> main_loop();
        
        std::vector<Point> getSnake();
};



#endif //SNAKE_H
