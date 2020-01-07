#include <vector>
#include <cmath>
#include <iostream>

#include <Snake.hpp>
#include <MLV/MLV_all.h>


void create_and_boucle(int width, int height, MLV_Image *image) {
    double radius = (width / 2. + height / 2.) / 2.;
    double perimeter = 6.28 * radius;
    int nmb = int(perimeter / 16);
    std::vector<Point> startPoints;
    double pointX, pointY;
    
    for (int i = 0; i < nmb; i++) {
        pointX = (width / 2.) + (width / 2. - 2) * cos(6.28 * i / nmb);
        pointY = (height / 2.) + (height / 2. - 2) * sin(6.28 * i / nmb);
        startPoints.emplace_back(pointX, pointY);
    }
    
    Snake snake = Snake(image, width, height, startPoints);
    snake.loop();
}


int main(int argc, char **argv) {
    if (argc < 2) {
        std::cerr << "Image's path is missing" << std::endl;
        exit(EXIT_SUCCESS);
    }
    
    int width, height;
    
    MLV_create_window("Research", nullptr, 100, 100);
    
    MLV_Image *image = MLV_load_image(argv[1]);
    if (!image) {
        std::cerr << "Could not load images '" << argv[1] << "'" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    MLV_get_image_size(image, &width, &height);
    MLV_change_window_size(width * 2, height);
    
    create_and_boucle(width, height, image);
    
    MLV_wait_keyboard(nullptr, nullptr, nullptr);
    MLV_free_window();
    
    return 0;
}
