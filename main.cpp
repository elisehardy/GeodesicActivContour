#include "Snake.hpp"
#include "chamferDistance.hpp"
#include <MLV/MLV_all.h>
#include <vector>
#include <cmath>


std::vector<std::vector<int>> flow;
std::vector<std::vector<int>> gradient;


void calcul_gradient_flow(int width, int height, MLV_Image &image){
    //calcul grayLevelScale
    int clum[width][height];
    int red, blue, green, alpha;
    for(int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            MLV_get_pixel_on_image(&image, j, i, &red, &green, &blue, &alpha);
            clum[j][i] = (int) (0.299 * red + 0.587 * green + 0.114 * blue);
        }
    }
    std::cout << "clum" << std::endl;

    //gradient
        int gradient[width][height];
        int maxgradient =0;
        for(int i =0; i< height-2; i++){
            for(int j =0; j<width-2;j++){
                int p00 = clum[j+0][i+0]; int p10 = clum[j+1][i+0]; int p20 = clum[j+2][i+0];
                int p01 = clum[j+0][i+1]; /*-------------------- */ int p21 = clum[j+2][i+1];
                int p02 = clum[j+0][i+2]; int p12 = clum[j+1][i+2]; int p22 = clum[j+2][i+2];
                int sx = (p20+2*p21+p22)-(p00+2*p01+p02);
                int sy = (p02+2*p12+p22)-(p00+2*p10+p10);
                int snorm = (int)sqrt(sx*sx+sy*sy);
                gradient[j+1][i+1] = snorm;
                maxgradient = std::max(maxgradient, snorm);
            }
        }
    std::cout << "gradient" << std::endl;

        bool binarygradient[640][480];
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++)
            if (gradient[j][i] > 100 * maxgradient / 100) {
                binarygradient[j][i] = true;
            } else {
                gradient[j][i] = 0;
            }
    }
    std::cout << "binary" << std::endl;

    std::vector<std::vector<int>> flow;
    std::cout << "calcul" << std::endl;

    std::vector<std::vector<double>> cdist = chamferDistance().calcul(binarygradient, width, height);
    std::cout << "fin calcul" << std::endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            flow[j][i] = (int) (5 * cdist[j][i]);
        }
    }

    std::cout << "fin graidnet flow" << std::endl;





}


void drawSnake(Snake snake, MLV_Image &image){

    //draw image
    MLV_draw_image( &image, 0, 0 );


    //draw lines and points
    Point p1(0,0), p2(0,0);
    int sizeSnake = snake.getSizePoint();
    for(int i = 0; i < sizeSnake; i++) {
        p1 = snake.getSnake()[i];
        p2 = snake.getSnake()[(i+1)*sizeSnake];
        MLV_draw_line(p1.x, p1.y, p2.x, p2.y, MLV_COLOR_RED);
        MLV_draw_filled_circle( p1.x, p1.y, 5, MLV_COLOR_BLUE );
    }
}

void create_and_boucle(int width, int height, MLV_Image &image){
    double radius = (width/2+height/2)/2;
    double perimeter = 6.28*radius;
    int nmb = (int)(perimeter/16);
    std::vector<Point> point_depart;
    double pointX, pointY;
    for(int i=0; i< nmb; i++){
        pointX = (width/2) + (width/2-2)*cos((6.28*i)/nmb);
        pointY = (height/2) +(height/2-2)*sin((6.28*i)/nmb);
        point_depart.push_back(Point(pointX, pointY));
    }

    //create snake
    Snake snake = Snake(width, height, gradient, flow, point_depart);
    drawSnake(snake, image);
    std::cout << "END" << std::endl;

}


int main(int argc, char** argv){
    int width = 640, height = 480;
    int image_width, image_height;
    MLV_create_window(
            "advanced - 1 - texts and boxes", "texts and boxes", width, height );

    MLV_Image* image = MLV_load_image( argv[1] );
    if( !image ) {
        fprintf(
                stderr, "It was impossible to load the file %s.\n",
                argv[1]
        );
        return -1;
    }
    MLV_resize_image_with_proportions( image, width, height);
    MLV_get_image_size( image, &image_width, &image_height );
    MLV_change_window_size( image_width, image_height );
    MLV_draw_image( image, 0, 0 );

    MLV_Keyboard_button touche;
    while(
            MLV_get_event (
                    &touche, NULL, NULL,
                    NULL, NULL,
                    NULL, NULL, NULL,
                    NULL
            ) == MLV_NONE ||
            touche != MLV_KEYBOARD_SPACE
            ) {
        MLV_actualise_window();
    }
    std::cout << "aaa" << std::endl;

    MLV_actualise_window();
    std::cout << "actualise" << std::endl;

    calcul_gradient_flow(width, height, *image);
    std::cout << "calcul gradient flow" << std::endl;
    create_and_boucle(width, height, *image);
    MLV_actualise_window();
    MLV_wait_seconds( 5 );
    MLV_free_window();

    return 0;
}
