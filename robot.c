#include "robot.h"

void setup_robot(struct Robot *robot) {
    robot->x = 170;
    robot->y = 460;
    robot->true_x = 170;
    robot->true_y = 460;
    robot->width = ROBOT_WIDTH;
    robot->height = ROBOT_HEIGHT;
    robot->direction = 0;
    robot->angle = 0;
    robot->currentSpeed = 0;
    robot->crashed = 0;
    robot->auto_mode = 0;

    printf("Press arrow keys to move manually, or enter to move automatically\n\n");
}

int robot_off_screen(struct Robot * robot) {
    if (robot->x < 0 || robot->y < 0)
        return 0;

    if (robot->x > OVERALL_WINDOW_WIDTH || robot->y > OVERALL_WINDOW_HEIGHT)
        return 0;

    return 1;
}

int checkRobotHitWall(struct Robot * robot, struct Wall * wall) {

    int overlap = checkOverlap(robot->x,robot->width,robot->y,robot->height, robot->angle,
                 wall->x,wall->width,wall->y, wall->height);

    return overlap;
}

int checkRobotHitWalls(struct Robot * robot, struct Wall_collection * head) {
    struct Wall_collection *ptr = head;

    while(ptr != NULL) {
        if (checkRobotHitWall(robot, &ptr->wall))
            return 1;
        ptr = ptr->next;
    }
    return 0;

}

int checkRobotReachedEnd(struct Robot * robot, int x, int y, int width, int height){

    int overlap = checkOverlap(robot->x,robot->width,robot->y,robot->height, robot->angle,
                 x,width,y,height);

    return overlap;
}

void robotCrash(struct Robot * robot) {
    robot->currentSpeed = 0;
    if (!robot->crashed)
        printf("Ouchies!!!!!\n\nPress space to start again\n");
    robot->crashed = 1;
}

void robotSuccess(struct Robot * robot, int msec) {
    robot->currentSpeed = 0;
    if (!robot->crashed){
        printf("Success!!!!!\n\n");
        printf("Time taken %d seconds %d milliseconds \n", msec/1000, msec%1000);
        printf("Press space to start again\n");
    }
    robot->crashed = 1;
}

int checkRobotSensor(int x, int y, int sensorSensitivityLength, int angle, struct Wall * wall)  {
    //viewing_region of sensor is a square of 2 pixels * chosen length of sensitivity
    int overlap = checkOverlap(x,2,y,sensorSensitivityLength, angle,
                 wall->x,wall->width,wall->y, wall->height);

    return overlap;
}


// main calls all of the check robot sensor functions
// instead of having 3 really big, half redundant methods, we have combined them, sort of
// with this sensorAngle variable, we know which sensor we are checking and do the correct math in one method

int sensorAngle;

// for right sensor
int checkRobotSensorFrontRightAllWalls(struct Robot * robot, struct Wall_collection * head) {
    sensorAngle = 30;
    return checkRobotSensorAllWalls(robot, head);
}

// left sensor
int checkRobotSensorFrontLeftAllWalls(struct Robot * robot, struct Wall_collection * head) {
    sensorAngle = -30;
    return checkRobotSensorAllWalls(robot, head);
}

// front sensor
int checkRobotSensorFrontAllWalls(struct Robot * robot, struct Wall_collection * head) {
    sensorAngle = 0;
    return checkRobotSensorAllWalls(robot, head);
}

// general sensor check
int checkRobotSensorAllWalls(struct Robot * robot, struct Wall_collection * head) {
    struct Wall_collection *ptr, *head_store;
    head_store = head;

    int sensorSensitivityLength = floor(SENSOR_VISION/10);
    int robotCentreX = robot->x+ROBOT_WIDTH/2;
    int robotCentreY = robot->y+ROBOT_HEIGHT/2;

    int xTL, yTL;

    // loops backwards because closer wall is way more important than further wall
    // this also prevents the sensors from seeing through walls
    for (int i = 9; i >= 0; i--)
    {
        ptr = head_store;

        // right sensor
        if (sensorAngle == 30) {
            xTL = (int) round(robotCentreX+(ROBOT_WIDTH/2-2)*cos(((robot->angle)+30)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin(((robot->angle)+30)*PI/180));
            yTL = (int) round(robotCentreY-(-ROBOT_WIDTH/2-2)*sin(((robot->angle)+30)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos(((robot->angle)+30)*PI/180));
        }
        // left sensor 
        else if (sensorAngle == -30) {
            xTL = (int) round(robotCentreX+(-ROBOT_WIDTH/2)*cos(((robot->angle)-30)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*sin(((robot->angle)-30)*PI/180));
            yTL = (int) round(robotCentreY-(ROBOT_WIDTH/2)*sin(((robot->angle)-30)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensorSensitivityLength*i)*cos(((robot->angle)-30)*PI/180));
        } 
        // front sensor
        else {
            xTL = (int) round(robotCentreX+(ROBOT_WIDTH/2000)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/1-SENSOR_VISION*3+sensorSensitivityLength*i*3)*sin((robot->angle)*PI/180));
            yTL = (int) round(robotCentreY+(ROBOT_WIDTH/2000)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/1-SENSOR_VISION*3+sensorSensitivityLength*i*3)*cos((robot->angle)*PI/180));
        }

        // check all the walls against the sensor
        while(ptr != NULL) {
            if (checkRobotSensor(xTL, yTL, sensorSensitivityLength, robot->angle, &ptr->wall))
                return i;
            ptr = ptr->next;
        }
    }
    return 0;
}

// draw robot and sensor
void robotUpdate(struct SDL_Renderer * renderer, struct Robot * robot) {

    int robotCentreX, robotCentreY, xTR, yTR, xTL, yTL, xBR, yBR, xBL, yBL;
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);

    //Rotating Square
    //Vector rotation to work out corners x2 = x1cos(angle)-y1sin(angle), y2 = x1sin(angle)+y1cos(angle)
    robotCentreX = robot->x+ROBOT_WIDTH/2;
    robotCentreY = robot->y+ROBOT_HEIGHT/2;
    
    xTR = (int) round(robotCentreX+(ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2)*sin((robot->angle)*PI/180));
    yTR = (int) round(robotCentreY+(ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2)*cos((robot->angle)*PI/180));
    
    xBR = (int) round(robotCentreX+(ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(ROBOT_HEIGHT/2)*sin((robot->angle)*PI/180));
    yBR = (int) round(robotCentreY+(ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(ROBOT_HEIGHT/2)*cos((robot->angle)*PI/180));

    xBL = (int) round(robotCentreX+(-ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(ROBOT_HEIGHT/2)*sin((robot->angle)*PI/180));
    yBL = (int) round(robotCentreY+(-ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(ROBOT_HEIGHT/2)*cos((robot->angle)*PI/180));

    xTL = (int) round(robotCentreX+(-ROBOT_WIDTH/2)*cos((robot->angle)*PI/180)-(-ROBOT_HEIGHT/2)*sin((robot->angle)*PI/180));
    yTL = (int) round(robotCentreY+(-ROBOT_WIDTH/2)*sin((robot->angle)*PI/180)+(-ROBOT_HEIGHT/2)*cos((robot->angle)*PI/180));

    SDL_RenderDrawLine(renderer,xTR, yTR, xBR, yBR);
    SDL_RenderDrawLine(renderer,xBR, yBR, xBL, yBL);
    SDL_RenderDrawLine(renderer,xBL, yBL, xTL, yTL);
    SDL_RenderDrawLine(renderer,xTL, yTL, xTR, yTR);

    // Front Right Sensor
    int sensor_sensitivity =  floor(SENSOR_VISION/10);
    int i;
    for (i = 0; i < 10; i++)
    {
        xTL = (int) round(robotCentreX+(ROBOT_WIDTH/2-2)*cos(((robot->angle)+30)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*sin(((robot->angle)+30)*PI/180));
        yTL = (int) round(robotCentreY-(-ROBOT_WIDTH/2-2)*sin(((robot->angle)+30)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*cos(((robot->angle)+30)*PI/180));

        xTR = (int) round(robotCentreX+(ROBOT_WIDTH/2)*cos(((robot->angle)+30)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*sin(((robot->angle)+30)*PI/180));
        yTR = (int) round(robotCentreY-(-ROBOT_WIDTH/2)*sin(((robot->angle)+30)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*cos(((robot->angle)+30)*PI/180));

        xBL = (int) round(robotCentreX+(ROBOT_WIDTH/2-2)*cos(((robot->angle)+30)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*(i-1))*sin(((robot->angle)+30)*PI/180));
        yBL = (int) round(robotCentreY-(-ROBOT_WIDTH/2-2)*sin(((robot->angle)+30)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*(i-1))*cos(((robot->angle)+30)*PI/180));

        xBR = (int) round(robotCentreX+(ROBOT_WIDTH/2)*cos(((robot->angle)+30)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*(i-1))*sin(((robot->angle)+30)*PI/180));
        yBR = (int) round(robotCentreY-(-ROBOT_WIDTH/2)*sin(((robot->angle)+30)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*(i-1))*cos(((robot->angle)+30)*PI/180));

        SDL_SetRenderDrawColor(renderer, 80+(20*(5-i)), 80+(20*(5-i)), 80+(20*(5-i)), 255);

        SDL_RenderDrawLine(renderer,xTR, yTR, xBR, yBR);
        SDL_RenderDrawLine(renderer,xBR, yBR, xBL, yBL);
        SDL_RenderDrawLine(renderer,xBL, yBL, xTL, yTL);
        SDL_RenderDrawLine(renderer,xTL, yTL, xTR, yTR);
    }

    // Front Left Sensor
    for (i = 0; i < 10; i++)
    {
        xTL = (int) round(robotCentreX+(-ROBOT_WIDTH/2)*cos(((robot->angle)-30)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*sin(((robot->angle)-30)*PI/180));
        yTL = (int) round(robotCentreY-(ROBOT_WIDTH/2)*sin(((robot->angle)-30)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*cos(((robot->angle)-30)*PI/180));

        xTR = (int) round(robotCentreX+(-ROBOT_WIDTH/2+2)*cos(((robot->angle)-30)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*sin(((robot->angle)-30)*PI/180));
        yTR = (int) round(robotCentreY-(ROBOT_WIDTH/2+2)*sin(((robot->angle)-30)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*i)*cos(((robot->angle)-30)*PI/180));

        xBL = (int) round(robotCentreX+(-ROBOT_WIDTH/2)*cos(((robot->angle)-30)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*(i-1))*sin(((robot->angle)-30)*PI/180));
        yBL = (int) round(robotCentreY-(ROBOT_WIDTH/2)*sin(((robot->angle)-30)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*(i-1))*cos(((robot->angle)-30)*PI/180));

        xBR = (int) round(robotCentreX+(-ROBOT_WIDTH/2+2)*cos(((robot->angle)-30)*PI/180)-(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*(i-1))*sin(((robot->angle)-30)*PI/180));
        yBR = (int) round(robotCentreY-(ROBOT_WIDTH/2+2)*sin(((robot->angle)-30)*PI/180)+(-ROBOT_HEIGHT/2-SENSOR_VISION+sensor_sensitivity*(i-1))*cos(((robot->angle)-30)*PI/180));

        SDL_SetRenderDrawColor(renderer, 80+(20*(5-i)), 80+(20*(5-i)), 80+(20*(5-i)), 255);

        SDL_RenderDrawLine(renderer,xTR, yTR, xBR, yBR);
        SDL_RenderDrawLine(renderer,xBR, yBR, xBL, yBL);
        SDL_RenderDrawLine(renderer,xBL, yBL, xTL, yTL);
        SDL_RenderDrawLine(renderer,xTL, yTL, xTR, yTR);
    }

	// Front Sensor
    for (i = 0; i < 10; i++)
    {
        xTL = (int) round(robotCentreX+(-ROBOT_WIDTH/2000)*cos(((robot->angle))*PI/180)-(-ROBOT_HEIGHT/2000-SENSOR_VISION*4.16+sensor_sensitivity*i*4.16)*sin(((robot->angle))*PI/180));
        yTL = (int) round(robotCentreY+(-ROBOT_WIDTH/2000)*sin(((robot->angle))*PI/180)+(-ROBOT_HEIGHT/2000-SENSOR_VISION*4.16+sensor_sensitivity*i*4.16)*cos(((robot->angle))*PI/180));

        xTR = (int) round(robotCentreX+(-ROBOT_WIDTH/2000-2)*cos(((robot->angle))*PI/180)-(-ROBOT_HEIGHT/2000-SENSOR_VISION*4.16+sensor_sensitivity*i*4.16)*sin(((robot->angle))*PI/180));
        yTR = (int) round(robotCentreY+(-ROBOT_WIDTH/2000-2)*sin(((robot->angle))*PI/180)+(-ROBOT_HEIGHT/2000-SENSOR_VISION*4.16+sensor_sensitivity*i*4.16)*cos(((robot->angle))*PI/180));

        xBL = (int) round(robotCentreX+(-ROBOT_WIDTH/2000)*cos(((robot->angle))*PI/180)-(-ROBOT_HEIGHT/2000-SENSOR_VISION*4.16+sensor_sensitivity*(i-1)*4.16)*sin(((robot->angle))*PI/180));
        yBL = (int) round(robotCentreY+(-ROBOT_WIDTH/2000)*sin(((robot->angle))*PI/180)+(-ROBOT_HEIGHT/2000-SENSOR_VISION*4.16+sensor_sensitivity*(i-1)*4.16)*cos(((robot->angle))*PI/180));

        xBR = (int) round(robotCentreX+(-ROBOT_WIDTH/2000-2)*cos(((robot->angle))*PI/180)-(-ROBOT_HEIGHT/2000-SENSOR_VISION*4.16+sensor_sensitivity*(i-1)*4.16)*sin(((robot->angle))*PI/180));
        yBR = (int) round(robotCentreY+(-ROBOT_WIDTH/2000-2)*sin(((robot->angle))*PI/180)+(-ROBOT_HEIGHT/2000-SENSOR_VISION*4.16+sensor_sensitivity*(i-1)*4.16)*cos(((robot->angle))*PI/180));

        SDL_SetRenderDrawColor(renderer, 80+(20*(5-i)), 80+(20*(5-i)), 80+(20*(5-i)), 255);

        SDL_RenderDrawLine(renderer,xTR, yTR, xBR, yBR);
        SDL_RenderDrawLine(renderer,xBR, yBR, xBL, yBL);
        SDL_RenderDrawLine(renderer,xBL, yBL, xTL, yTL);
        SDL_RenderDrawLine(renderer,xTL, yTL, xTR, yTR);
    }
}

// manual movement with arrow keys
void robotMotorMove(struct Robot * robot) {

    // handle turning robot
    switch (robot->direction) {
        case LEFT :
            robot->angle = (robot->angle-DEFAULT_ANGLE_CHANGE)%360;
            break;
        case RIGHT :
            robot->angle = (robot->angle+DEFAULT_ANGLE_CHANGE)%360;
            break;
    }

    // handle speed of robot
    switch (robot->changeSpeed) {
        case 1:
            robot->currentSpeed += DEFAULT_SPEED_CHANGE;
            if (robot->currentSpeed > MAX_ROBOT_SPEED)
                robot->currentSpeed = MAX_ROBOT_SPEED;
            break;
        case -1:
            robot->currentSpeed -= DEFAULT_SPEED_CHANGE;
            if (robot->currentSpeed < -MAX_ROBOT_SPEED)
                robot->currentSpeed = -MAX_ROBOT_SPEED;
            break;
    }

    robot->direction = 0;

    robot->true_x += (-robot->currentSpeed * sin(-robot->angle*PI/180));
    robot->true_y += (-robot->currentSpeed * cos(-robot->angle*PI/180));

    robot->x = (int) round(robot->true_x);
    robot->y = (int) round(robot->true_y);
}

int wallfound = 0;
int wallfollowing = 0;
int panicSpot = 0;

// auto movement and navigation of the robot
void robotAutoMotorMove(struct Robot * robot, int front_left_sensor, int front_right_sensor, int front_sensor) {

    // PANIC SPOT
    // dead ends
    if (panicSpot > 0) {
        robot->direction = RIGHT;
        panicSpot--;
        if (front_right_sensor <= 2 && front_sensor <= 5) {
            panicSpot = 0;
            robot->direction = STRAIGHT;
        }
        return;
    }

    if ((front_sensor > 7 && front_left_sensor > 7 && front_right_sensor > 7) ) {
        panicSpot = 12;
        robot->currentSpeed = 0;
        robot->changeSpeed = 0;
        robot->direction = STRAIGHT;
        wallfound = 0;
        wallfollowing = 0;
        return;
    }

    // we need to be moving!
    if (robot->currentSpeed == 0) {
        robot->changeSpeed = 1;
    }

    // speed up/down if wall is far/close
    if (front_sensor == 0) 
        robot->changeSpeed = 1;
    else if (front_sensor > 0 && robot->currentSpeed > 4 && robot->currentSpeed != 0)
        robot->changeSpeed = -1;

    // found a wall to follow
    if (wallfound == 0 && front_sensor > 6) {
        wallfound = 1;
    } else {

        // main navigation with sensors
        
        // too close to a wall on left or too close infront but right is clear
        if (front_left_sensor > 6 ||
           (front_sensor > 6 && front_right_sensor < 3))
            robot->direction = RIGHT;

        // too far from left wall to follow, stay closer
        else if (front_left_sensor < 5 && wallfollowing == 1)
            robot->direction = LEFT;

        // otherwise, we are where we want to be in relation to the left wall
        else
            robot->direction = STRAIGHT;

        // this will override the direction set to left or right on lines 321, and 325
        // if ahead is relatively clear and we aren't too close to the right wall
        // it doesn't really matter how far away we are from the left wall
        // we can just go straight
        if (front_sensor <= 1 && front_right_sensor <= 5)
            robot->direction = STRAIGHT;

        // this handles left turns that aren't just getting closer to the wall
        // on both left side of the "track" and right side
        if ((front_left_sensor <= 2 && wallfollowing == 1) ||   // left side
            (front_sensor > 6 && front_right_sensor > 4))       // right side
            robot->direction = LEFT;

        // TOO CLOSE TO WALL!!
        // failsafes to not hit wall
        if (front_left_sensor > 7)
            robot->direction = RIGHT;
        else if (front_right_sensor > 7)
            robot->direction = LEFT;

    }

    if (wallfollowing == 0 && front_left_sensor > 2 && front_right_sensor < front_left_sensor)
        wallfollowing = 1;
}
