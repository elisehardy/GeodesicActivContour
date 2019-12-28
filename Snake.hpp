//
// Created by ehardy on 12/4/19.
//

#ifndef SNAKE_H
#define SNAKE_H

#include <iostream>
#include <vector>
#include "Point.hpp"


class Snake {
public:
    const int ITERATION_MAX = 100;
    const double alpha = 1.1, beta = 1.2, gamma = 1.5, delta = 3.0;
    const int AUTO_LOOP = 10;
    const int AUTO_MIN = 8;
    const int AUTO_MAX = 16;
    const bool AUTO = true;
private:
    std::vector <Point> snake;
    double length;
    int width_image;
    int height_image;
    std::vector <std::vector<int>> gradient;
    std::vector <std::vector<int>> flow;
    std::vector <std::vector<double>> uniformityEnergy;
    std::vector <std::vector<double>> curvatureEnergy;
    std::vector <std::vector<double>> flowEnergy;
    std::vector <std::vector<double>> inertiaEnergy;

    bool update();

    double getLength();

public:

    Snake(int _width_image, int _height_image, const std::vector <std::vector<int>> &gradient,
          const std::vector <std::vector<int>> &flow, std::vector <Point> points);

    ~Snake();

    std::vector <Point> main_loop();

    static void normalizeEnergy(std::vector <std::vector<double>> energy);

    double calculUniformity(Point preview, Point current, Point next);

    double calculCurvature(Point preview, Point current, Point next);

    double calculFlow(Point current, Point point);

    double calculInertia(Point current, Point p);

    void recreate(int space);

    void removePoint(int min);

    void addPoint(int max);

    int getSizePoint();

    std::vector <Point> getSnake();

};


#endif //SNAKE_H
