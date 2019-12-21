//
// Created by ehardy on 12/4/19.
//

#include "Snake.hpp"
#include "Point.hpp"
#include <limits>
#include <cmath>


Snake::Snake(int _width_image, int _height_image, const std::vector<std::vector<int>> &_gradient,
             const std::vector<std::vector<int>> &_flow, std::vector<Point> points) :
        width_image(_width_image), height_image(_height_image), gradient(_gradient), flow(_flow) {
    
    for (const auto &p: points) {
        this->snake.push_back(p);
    }
}


Snake::~Snake() {
};


double Snake::calculUniformity(Point preview, Point current, Point next) {
    double distance = preview.calculDistance(current);
    double mean = this->length / this->snake.size();
    return abs(distance - mean) * abs(distance - mean);
}


double Snake::calculCurvature(Point preview, Point current, Point next) {
    int cpx, cpy, pnx, pny;
    cpx = current.x - preview.x;
    cpy = current.y - preview.y;
    pnx = current.x - next.x;
    pny = current.y - next.y;
    
    double distancecp = sqrt(cpx * cpx + cpy * cpy);
    double distancepn = sqrt(pnx * pnx + pny * pny);
    
    if (distancecp == 0 || distancepn == 0) {
        return 0;
    }
    double x = (cpx + pnx) / (distancecp * distancepn);
    double y = (cpy + pny) / (distancecp * distancepn);
    
    return x * x + y * y;
}


double Snake::calculFlow(Point current, Point point) {
    return this->flowEnergy[point.x][point.y] - this->flowEnergy[current.x][current.y];
}


double Snake::calculInertia(Point current, Point point) {
    return current.calculDistance(point) * this->gradient[current.x][current.y];
}


double Snake::getLength() {
    double res = 0;
    int size = snake.size();
    for (int i = 0; i < size; i++) {
        res += snake[i].calculDistance(snake[(i + 1) % size]);
    }
    return res;
}


void Snake::normalizeEnergy(std::vector<std::vector<double>> energy) {
    double sum = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            sum += abs(energy[i][j]);
        }
    }
    if (sum == 0) {
        return;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            energy[i][j] /= sum;
        }
    }
}


bool Snake::update() {
    bool flag = false;
    Point p_tmp(0, 0);
    length = getLength();
    std::vector<Point> tmpSnake;
    int size = snake.size();
    for (int i = 0; i < size; i++) {
        Point preview = snake[(i + size - 1) % size];
        Point current = snake[i];
        Point next = snake[((i + 1) % size)];
        
        //calcul energies
        for (int j = -1; j <= 1; j++) {
            for (int h = -1; h <= 1; h++) {
                p_tmp.setLocation(current.x + h, current.y + j);
                this->flowEnergy[1 + h][1 + j] = calculFlow(current, p_tmp);
                this->curvatureEnergy[1 + h][1 + j] = calculCurvature(preview, p_tmp, next);
                this->uniformityEnergy[1 + h][1 + j] = calculUniformity(preview, next, p_tmp);
                this->inertiaEnergy[1 + h][1 + j] = calculInertia(current, p_tmp);
            }
        }
        
        //energy normalize
        normalizeEnergy(flowEnergy);
        normalizeEnergy(curvatureEnergy);
        normalizeEnergy(uniformityEnergy);
        normalizeEnergy(inertiaEnergy);
        
        //find min
        double energyMin = std::numeric_limits<double>::max();
        double energy = 0;
        int x, y;
        
        for (int j = -1; j <= 1; j++) {
            for (int h = -1; h <= 1; h++) {
                energy = 0;
                energy += (alpha * uniformityEnergy[1 + h][1 + j])
                          + (beta * curvatureEnergy[1 + h][1 + j])
                          + (gamma * flowEnergy[1 + h][1 + j])
                          + (delta * inertiaEnergy[1 + h][1 + j]);
                if (energy < energyMin) {
                    energyMin = energy;
                    x = current.x + h;
                    y = current.y + j;
                }
            }
        }
        if (x < 1) {
            x = 1;
        }
        if (x >= (this->width_image - 1)) {
            x = this->width_image - 2;
        }
        if (y < 1) {
            y = 1;
        }
        if (y >= (this->height_image - 1)) {
            y = this->height_image - 2;
        }
        
        if (x != current.x || y != current.y) {
            flag = true;
        }
        tmpSnake.push_back(Point(x, y));
    }
    
    this->snake = tmpSnake;
    return flag;
}


void Snake::removePoint(int min) {
    
    int size = snake.size();
    for (int i = 0; i < size; i++) {
        Point current = snake[i];
        
        for (int di = 1 + size / 2; di > 0; di--) {
            Point end = snake[(i + di) % size];
            double dist = current.calculDistance(end);
            
            if (dist > min) {
                continue;
            };
            
            for (int j = 0; j < di; j++) {
                snake.erase(snake.begin() + ((i + 1) % size));
            };
            break;
        }
    }
}


void Snake::addPoint(int max) {
    
    int size = snake.size();
    for (int i = 0; i < size; i++) {
        Point prev = snake[(i + size - 1) % size];
        Point current = snake[i];
        Point next = snake[(i + 1) % snake.size()];
        Point next_next = snake[(i + 2) % snake.size()];
        
        if (current.calculDistance(next) > max) {
            
            double c0 = 0.125 / 6.0, c1 = 2.875 / 6.0, c2 = 2.875 / 6.0, c3 = 0.125 / 6.0;
            double x = prev.x * c3 + current.x * c2 + next.x * c1 + next_next.x * c0;
            double y = prev.y * c3 + current.y * c2 + next.y * c1 + next_next.y * c0;
            Point newpoint = Point((int) (0.5 + x), (int) (0.5 + y));
            
            snake.insert(snake.begin() + (i + 1), newpoint);
            i--;
        }
    }
}


std::vector<Point> Snake::main_loop() {
    int i = 0;
    while (update() && i < Snake::ITERATION_MAX) {
        if (Snake::AUTO && (i % Snake::AUTO_LOOP) == 0) {
            removePoint(Snake::AUTO_MIN);
            addPoint(Snake::AUTO_MAX);
        }
        i++;
    }
    if (Snake::AUTO) {
        recreate(Snake::AUTO_MAX);
    }
    return snake;
}


void Snake::recreate(int space) {
    
    double lengths_snake[snake.size() + 1];
    lengths_snake[0] = 0;
    int size = snake.size();
    for (int i = 0; i < size; i++) {
        Point current = snake[i];
        Point next = snake[(i + 1) % size];
        lengths_snake[i + 1] = lengths_snake[i] + current.calculDistance(next);
    }
    
    double total = lengths_snake[size];
    int nmb = (int) (0.5 + total / space);
    
    // build a new snake
    std::vector<Point> snake_tmp;
    for (int i = 0, j = 0; j < nmb; j++) {
        // current length in the new snake
        double snake_lenght = (j * total) / nmb;
        
        // find corresponding interval of points in the original snake
        while (!(lengths_snake[i] <= snake_lenght && snake_lenght < lengths_snake[i + 1])) {
            i++;
        };
        
        // get points (P-1,P,P+1,P+2) in the original snake
        Point prev = snake[(i + size - 1) % size];
        Point current = snake[i];
        Point next = snake[(i + 1) % size];
        Point next_next = snake[(i + 2) % size];
        
        // do cubic spline interpolation
        double t = (snake_lenght - lengths_snake[i]) / (lengths_snake[i + 1] - lengths_snake[i]);
        double t2 = t * t, t3 = t2 * t;
        double c0 = 1 * t3;
        double c1 = -3 * t3 + 3 * t2 + 3 * t + 1;
        double c2 = 3 * t3 - 6 * t2 + 4;
        double c3 = -1 * t3 + 3 * t2 - 3 * t + 1;
        double x = prev.x * c3 + current.x * c2 + next.x * c1 + next_next.x * c0;
        double y = prev.y * c3 + current.y * c2 + next.y * c1 + next_next.y * c0;
        Point newpoint = Point((int) (0.5 + x / 6), (int) (0.5 + y / 6));
        
        // add computed point to the new snake
        snake_tmp.push_back(newpoint);
    }
    this->snake = snake_tmp;
}
