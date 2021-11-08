#include "formulas.h"
#include "structures.h"

// Vec2 object "constructor"
// these can be largely considered points in a 2D plane
Vec2 new(double x, double y) {
    Vec2 a;
    a.x = x;
    a.y = y;
    return a;
}

// add two Vec2's, return the added vector
Vec2 add(Vec2 a, Vec2 b) {
    return new(a.x+b.x, a.y+b.y);
}

// subtract two Vec2's, return the new vector
Vec2 sub(Vec2 a, Vec2 b) {
    return new(a.x-b.x,a.y-b.y);
}

// dot product of two vectors
double dot(Vec2 a, Vec2 b) {
    return a.x*b.x + a.y*b.y;
}

// new checkOverlap function that actually works correctly when the robot turns unlike the starter code
/*
    This function uses some pretty cool vector things and linear algebra things to check if the wall and robot overlap
    NOTE: shortcuts are taken here based on the knowledge that we will always be working with rectangular objects
    This essentially takes each edge of the robot, and check if that edge can be used as a "separating line" between the robot and the wall
    if any edge can be used as a separating line, then the robot has not hit the wall
    if no edge can be used as a separating line, then the robot has hit the wall :(

    First of all, we get the corner points of the robot
    Then, the corner points of the wall
    Now we get some normal vectors, these are the axis that we will project the wall and robot on to

    Knowing that we will only work with rectangular objects, instead of looping through each point and creating edges
    to loop through, we just loop 4 times (except special case just twice)

    Inside this loop, we have two loops, one for the robot points, and the other for the wall points
    This is where we create the projection of the robot and wall    
    This projection allows us to check if the edge can be used as a separating line
    If that edge can be used as a separating line, then we return 0 (false)
    Otherwise we check the next edge.

    After going through every edge of one object (we do the robot), and finding know separating lines,
    we can conclude that the objects do intersect, return 1 (true)

    for a more helpful and visual explanation :
    https://www.codeproject.com/Articles/15573/2D-Polygon-Collision-Detection

    the first coloured image is extremely helpful in understanding the projection and how it can conclude whether or not two objects could be intersecting
*/
int checkOverlap(
    int object1X, int object1width,
    int object1Y, int object1height,
    int object1Angle,
    int object2X, int object2width,
    int object2Y, int object2height
) { 

    // variables to calculate robot corner points
    double radAng = PI*object1Angle/180.0;

    Vec2 centre = new(object1X + object1width/2.0, object1Y + object1height/2.0);
    Vec2 frontEdgeCentre = new(object1height/2.0 * sin(radAng), object1height/2.0 * cos(radAng));
    Vec2 leftEdgeCentre = new(object1width/2.0 * -cos(radAng), object1width/2.0 * sin(radAng));
    Vec2 rightEdgeCentre = new(-leftEdgeCentre.x, -leftEdgeCentre.y);
    Vec2 backEdgeCentre = new(-frontEdgeCentre.x, -frontEdgeCentre.y);

    // real rotated robot (object 1) corner points
    Vec2 robotFrontLeft = add(add(leftEdgeCentre, frontEdgeCentre), centre);
    Vec2 robotFrontRight = add(add(rightEdgeCentre, frontEdgeCentre), centre);
    Vec2 robotBackLeft = add(add(leftEdgeCentre, backEdgeCentre), centre);
    Vec2 robotBackRight = add(add(rightEdgeCentre, backEdgeCentre), centre);

    // real wall (object 2) points
    Vec2 wallTopLeft = new(object2X, object2Y);
    Vec2 wallTopRight = new(object2X + object2width, object2Y);
    Vec2 wallBottomRight = new(object2X + object2width, object2Y + object2height);
    Vec2 wallBottomLeft = new(object2X, object2Y + object2height);
    
    // all normal vectors
    // walls are made of non rotate rectangles!! very easy vector
    Vec2 e1 = new(1,0);
    Vec2 e2 = new(0,1);

    // robot is a square! we only need two normal vectors
    // front normal == back normal
    // left normal == right normal
    Vec2 normal1 = sub(robotFrontLeft, robotFrontRight);
    Vec2 normal2 = sub(robotFrontLeft, robotBackLeft);

    // lists of our normal vectors, robot points, and wall points
    Vec2 normals[] = {e1, e2, normal1, normal2};
    Vec2 robotPoints[] = {robotFrontLeft, robotFrontRight, robotBackRight, robotBackLeft};
    Vec2 wallPoints[] = {wallTopLeft, wallTopRight, wallBottomRight, wallBottomLeft};

    // slight optimisation for a robot at a 90 degree angle
    // only doing the loop below twice instead of 4 times
    int i = 2*(object1Angle%90 == 0);
    // ^^^^^^^^ this code is equivalent to commented code below, just waaaayyyy cooler
    // if (object1Angle % 90 == 0) 
    //     i = 2;
    // else 
    //     i = 0;
    
    // loop through edges of the robot
    for (; i < 4; i++) {
        // initalise the projected points
        double minRobot = dot(normals[i], robotPoints[0]);
        double maxRobot = minRobot;
        // loop through the rest of the points and project them
        // only take the min and max projections out
        for (int j = 1; j < 4; j++) {
            double projected = dot(normals[i], robotPoints[j]);
            if (projected < minRobot) 
                minRobot = projected;
            if (projected > maxRobot)
                maxRobot = projected;
        }

        // same as above, except now we do the wall, not the robot
        double minWall = dot(normals[i], wallPoints[0]);
        double maxWall = minWall;
        for (int j = 1; j < 4; j++) {
            double projected = dot(normals[i], wallPoints[j]);
            if (projected < minWall)
                minWall = projected;
            if (projected > maxWall)
                maxWall = projected;
        }

        // if the edge can be used as a separating line
        if (maxRobot < minWall || minRobot > maxWall)
            return 0;
    }
    // we have found no edge of the robot that can separate the robot and the wall, therefore, they must be intersecting :(
    return 1;
}

// please tell me someone read this and the comments were useful and i didn't just waste my time commenting all of this
// thanks for reading the comments, you are a champion