#include <iostream>
#include <cmath>
#include <utility>
#include <limits>

#include <MLV/MLV_all.h>

#include <Snake.hpp>
#include <ChamferDistance.hpp>


Snake::Snake(MLV_Image *t_image, int t_width, int t_height, std::vector<Point> t_points) :
    image(t_image), width(t_width), height(t_height), snake(std::move(t_points)) {
    this->uniformityEnergy = vector2D<double>(width, std::vector<double>(height));
    this->curvatureEnergy = vector2D<double>(width, std::vector<double>(height));
    this->inertiaEnergy = vector2D<double>(width, std::vector<double>(height));
    this->flowEnergy = vector2D<double>(width, std::vector<double>(height));
    
    computeGradientAndFlow();
}

/////////////////////////////////////// PRIVATE ////////////////////////////////

void Snake::normalizeEnergy(vector2D<double> &energy) {
    double sum = 0;
    
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            sum += std::abs(energy[i][j]);
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


void Snake::computeGradientAndFlow() {
    int red, blue, green, alpha;
    int lum[this->width][this->height];
    
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            MLV_get_pixel_on_image(this->image, j, i, &red, &green, &blue, &alpha);
            lum[j][i] = int(0.299 * red + 0.587 * green + 0.114 * blue);
        }
    }
    
    // Gradient
    this->gradient = vector2D<int>(this->width, std::vector<int>(this->height));
    int maxgradient = 0;
    for (int y = 0; y < this->height - 2; y++) {
        for (int x = 0; x < this->width - 2; x++) {
            int p00 = lum[x + 0][y + 0];
            int p10 = lum[x + 1][y + 0];
            int p20 = lum[x + 2][y + 0];
            int p01 = lum[x + 0][y + 1];
            int p21 = lum[x + 2][y + 1];
            int p02 = lum[x + 0][y + 2];
            int p12 = lum[x + 1][y + 2];
            int p22 = lum[x + 2][y + 2];
            int sx = (p20 + 2 * p21 + p22) - (p00 + 2 * p01 + p02);
            int sy = (p02 + 2 * p12 + p22) - (p00 + 2 * p10 + p10);
            int snorm = int(std::sqrt(sx * sx + sy * sy));
            this->gradient[x + 1][y + 1] = snorm;
            maxgradient = std::max(maxgradient, snorm);
        }
    }
    
    // Thresholding
    bool **binaryGradient = nullptr;
    binaryGradient = new bool *[this->width];
    for (int i = 0; i < this->width; i++) {
        binaryGradient[i] = new bool[this->height];
    }
    
    double threshold = 50. * maxgradient / 100.;
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            if (gradient[x][y] > threshold) {
                binaryGradient[x][y] = true;
            }
            else {
                binaryGradient[x][y] = false;
                gradient[x][y] = 0;
            }
        }
    }
    
    vector2D<double> cdist = ChamferDistance().compute(binaryGradient, this->width, this->height);
    this->flow = vector2D<int>(this->width, std::vector<int>(this->height));
    for (int i = 0; i < this->height; i++) {
        for (int j = 0; j < this->width; j++) {
            this->flow[j][i] = (int) (5 * cdist[j][i]);
        }
    }
    
    uint32_t r, g, b;
    this->imgFlow = MLV_create_image(this->width, this->height);
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            int vflow = this->flow[x][y];
            int vgrad = binaryGradient[x][y] ? 255 : 0;
            
            if (vgrad > 0) {
                r = 0;
                g = vgrad;
                b = 0;
            }
            else {
                r = std::max(0, 255 - vflow);
                g = 0;
                b = 0;
            }
            MLV_set_pixel_on_image(x, y, MLV_rgba(r, g, b, 255), this->imgFlow);
        }
    }
}


double Snake::computeUniformity(Point previous, Point current) {
    double distance = previous.distance(current);
    double mean = this->length / this->snake.size();
    double u = std::abs(distance - mean);
    
    return u * u;
}


double Snake::computeCurvature(Point previous, Point current, Point next) {
    int cpx, cpy, pnx, pny;
    cpx = current.x - previous.x;
    cpy = current.y - previous.y;
    pnx = current.x - next.x;
    pny = current.y - next.y;
    
    double cp = sqrt(cpx * cpx + cpy * cpy);
    double pn = sqrt(pnx * pnx + pny * pny);
    
    if (cp == 0 || pn == 0) {
        return 0;
    }
    double x = (cpx + pnx) / (cp * pn);
    double y = (cpy + pny) / (cp * pn);
    
    return x * x + y * y;
}


double Snake::computeFlow(Point current, Point point) {
    return this->flow[point.x][point.y] - this->flow[current.x][current.y];
}


double Snake::computeInertia(Point current, Point point) {
    return current.distance(point) * this->gradient[current.x][current.y];
}


void Snake::rebuild(int space) {
    int size = snake.size();
    double clength[size + 1];
    
    clength[0] = 0;
    for (int i = 0; i < size; i++) {
        Point current = snake[i];
        Point next = snake[(i + 1) % size];
        clength[i + 1] = clength[i] + current.distance(next);
    }
    
    double total = clength[size];
    int nmb = int(lround(0.5 + total / space));
    
    // build a new snake
    std::vector<Point> snake;
    for (int i = 0, j = 0; j < nmb; j++) {
        // current length in the new snake
        double dist = (j * total) / nmb;
        
        // find corresponding interval of points in the original snake
        while (!(clength[i] <= dist && dist < clength[i + 1])) {
            i++;
        }
        
        // get points (P-1,P,P+1,P+2) in the original snake
        Point prev = this->snake[(i + size - 1) % size];
        Point current = this->snake[i];
        Point next = this->snake[(i + 1) % size];
        Point next2 = this->snake[(i + 2) % size];
        
        // do cubic spline interpolation
        double t = (dist - clength[i]) / (clength[i + 1] - clength[i]);
        double t2 = t * t;
        double t3 = t2 * t;
        double c0 = t3;
        double c1 = -3 * t3 + 3 * t2 + 3 * t + 1;
        double c2 = 3 * t3 - 6 * t2 + 4;
        double c3 = -t3 + 3 * t2 - 3 * t + 1;
        double x = prev.x * c3 + current.x * c2 + next.x * c1 + next2.x * c0;
        double y = prev.y * c3 + current.y * c2 + next.y * c1 + next2.y * c0;
        
        Point newPoint = Point(int(lround(0.5 + x / 6)), int(lround(0.5 + y / 6)));
        snake.push_back(newPoint);
    }
    this->snake = snake;
}


void Snake::removePoint(int min) {
    int size = snake.size();
    
    for (int i = 0; i < size; i++) {
        Point current = snake[i];
        
        for (int di = 1 + size / 2; di > 0; di--) {
            Point end = snake[(i + di) % size];
            double dist = current.distance(end);
            
            if (dist > min || dist == 0) {
                continue;
            }
            
            for (int j = 0; j < di; j++) {
                snake.erase(snake.begin() + ((i + 1) % snake.size()));
            }
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
        
        if (current.distance(next) > max) {
            double c0 = 0.125 / 6.0, c1 = 2.875 / 6.0, c2 = 2.875 / 6.0, c3 = 0.125 / 6.0;
            double x = prev.x * c3 + current.x * c2 + next.x * c1 + next_next.x * c0;
            double y = prev.y * c3 + current.y * c2 + next.y * c1 + next_next.y * c0;
            Point newPoint = Point(int(lround(0.5 + x)), int(lround(0.5 + y)));
            snake.insert(snake.begin() + (i + 1), newPoint);
            i--;
        }
    }
}


double Snake::getLength() {
    int size = snake.size();
    double res = 0;
    
    for (int i = 0; i < size; i++) {
        res += this->snake[i].distance(this->snake[(i + 1) % size]);
    }
    
    return res;
}


bool Snake::step() {
    bool flag = false;
    Point p = Point(0, 0);
    int size = snake.size();
    std::vector<Point> newSnake;
    
    this->length = getLength();
    
    for (int i = 0; i < size; i++) {
        Point previous = snake[(i + size - 1) % size];
        Point current = snake[i];
        Point next = snake[((i + 1) % size)];
        
        //compute energies
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                p.setLocation(current.x + dx, current.y + dy);
                
                this->flowEnergy[1 + dx][1 + dy] = computeFlow(current, p);
                this->curvatureEnergy[1 + dx][1 + dy] = computeCurvature(previous, p, next);
                this->uniformityEnergy[1 + dx][1 + dy] = computeUniformity(previous, next);
                this->inertiaEnergy[1 + dx][1 + dy] = computeInertia(current, p);
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
        int x = 0, y = 0;
        
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                energy = (
                    ALPHA * uniformityEnergy[1 + dx][1 + dy]
                    + BETA * curvatureEnergy[1 + dx][1 + dy]
                    + GAMMA * flowEnergy[1 + dx][1 + dy]
                    + DELTA * inertiaEnergy[1 + dx][1 + dy]
                );
                if (energy < energyMin) {
                    energyMin = energy;
                    x = current.x + dx;
                    y = current.y + dy;
                }
            }
        }
        
        if (x < 1) {
            x = 1;
        }
        if (x >= (this->width - 1)) {
            x = this->width - 2;
        }
        if (y < 1) {
            y = 1;
        }
        if (y >= (this->height - 1)) {
            y = this->height - 2;
        }
        
        if (x != current.x || y != current.y) {
            flag = true;
        }
        newSnake.emplace_back(x, y);
    }
    
    this->snake = newSnake;
    return flag;
}

/////////////////////////////////// PUBLIC /////////////////////////////////////

int Snake::loop() {
    int loop;
    
    for (loop = 0; step() && loop < Snake::ITERATION_MAX; loop++) {
        if (loop % Snake::AUTO_LOOP == 0) {
            removePoint(Snake::AUTO_MIN);
            addPoint(Snake::AUTO_MAX);
        }
        
        this->draw();
        MLV_wait_milliseconds(10);
    }
    
    rebuild(Snake::AUTO_MAX);
    
    return loop;
}


void Snake::draw() const {
    std::vector<Point> points = this->getSnake();
    int size = points.size();
    
    MLV_clear_window(MLV_COLOR_WHITE);
    
    MLV_draw_image(this->image, 0, 0);
    MLV_draw_image(this->imgFlow, width, 0);
    
    Point p1(0, 0), p2(0, 0);
    for (int i = 0; i < size; i++) {
        p1 = points[i];
        p2 = points[(i + 1) % size];
        
        MLV_draw_line(p1.x, p1.y, p2.x, p2.y, MLV_COLOR_RED);
        MLV_draw_filled_circle(p1.x, p1.y, 5, MLV_COLOR_BLUE);
    }
    
    MLV_actualise_window();
}


std::vector<Point> Snake::getSnake() const {
    return std::vector<Point>(snake);
}
