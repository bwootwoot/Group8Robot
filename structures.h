#ifndef STRUCTURES_H_INCLUDED
#define STRUCTURES_H_INCLUDED

#define STRAIGHT 0
#define LEFT 1
#define RIGHT 2

#define OVERALL_WINDOW_WIDTH 640
#define OVERALL_WINDOW_HEIGHT 480

#define ROBOT_WIDTH 20
#define ROBOT_HEIGHT 20

#define WALL_WIDTH 10

#define DEFAULT_ANGLE_CHANGE 15
#define DEFAULT_SPEED_CHANGE 5
#define MAX_ROBOT_SPEED 25
#define PI 3.14159265
#define SENSOR_VISION 60

struct Wall {
    int x,y;
    int width, height;
};

struct Wall_collection {
    int key;
    struct Wall wall;
    struct Wall_collection *next;
};

struct Robot {
    int x, y;
    double true_x, true_y;
    int direction;
    int changeSpeed;
    int angle;
    int currentSpeed;
    int width, height;
    int crashed;
    int auto_mode;
};

typedef struct {
    double x,y;
} Vec2;

#endif // STRUCTURES_H_INCLUDED