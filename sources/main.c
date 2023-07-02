#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>
#define DELAUNAY_IMPLEMENTATION
#include "delaunay.h"

//WINDOW
#define SCREEN_SCALE 100
#define SCREEN_WIDTH (16*SCREEN_SCALE)
#define SCREEN_HEIGHT (9*SCREEN_SCALE)
#define WINDOW_TITLE "Animation Test"

#define NUMBER_OF_POINTS 50


int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(144);

    srand(time(NULL));

    // generate points
    float pointCloud2D[NUMBER_OF_POINTS][2];

    for (size_t i = 0; i < NUMBER_OF_POINTS; i++)
    {
        pointCloud2D[i][0] = (rand() % (SCREEN_WIDTH - 2)) + 1;
        pointCloud2D[i][1] = (rand() % (SCREEN_HEIGHT - 2)) + 1;
    }

    float(*pointsA)[2] = malloc(NUMBER_OF_POINTS * 2 * sizeof(float));
    for (int i = 0; i < NUMBER_OF_POINTS; i++) {
        pointsA[i][0] = pointCloud2D[i][0];
        pointsA[i][1] = pointCloud2D[i][1];
    }

    // generate speeds
    bool pointSpeeds[NUMBER_OF_POINTS][2];

    for (size_t i = 0; i < NUMBER_OF_POINTS; i++)
    {
        pointSpeeds[i][0] = rand() & 1;
        pointSpeeds[i][1] = rand() & 1;
    }


    printf("SCREEN RESOLUTION: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    while (!WindowShouldClose())
    {    
        BeginDrawing();
        {
            ClearBackground(DARKGRAY);
            delaunayTriangulate(pointsA, NUMBER_OF_POINTS);

            for (size_t i = 0; i < NUMBER_OF_POINTS; i++)
            {
                if ((pointsA[i][0] + 1) >= SCREEN_WIDTH ||
                    (pointsA[i][0] - 1) <= 0)
                {
                    printf("OUT OF BOUNDS[X]: x: %f, y: %f, speed: [%u, %u]\n", pointsA[i][0], pointsA[i][1], pointSpeeds[i][0], pointSpeeds[i][1]);
                    pointSpeeds[i][0] = !pointSpeeds[i][0];
                    pointsA[i][0] += pointSpeeds[i][0] ? 1 : -1;
                }
                if ((pointsA[i][1] + 1) >= SCREEN_HEIGHT ||
                    (pointsA[i][1] - 1) <= 0)
                {
                    printf("OUT OF BOUNDS[Y]: x: %f, y: %f, speed: [%u, %u]\n", pointsA[i][0], pointsA[i][1], pointSpeeds[i][0], pointSpeeds[i][1]);
                    pointSpeeds[i][1] = !pointSpeeds[i][1];
                    pointsA[i][1] += pointSpeeds[i][1] ? 1 : -1;
                }

                float speed = 0.05f;
                pointsA[i][0] += pointSpeeds[i][0] ? speed : -speed;
                pointsA[i][1] += pointSpeeds[i][1] ? speed : -speed;

                // chance for the point to change directions randomly
                if (((float)rand() / (float)RAND_MAX) < 0.0005) {
                    printf("aa\n");
                    pointSpeeds[i][0] = !pointSpeeds[i][0];
                }
                if (((float)rand() / (float)RAND_MAX) < 0.0005) {
                    printf("bb\n");
                    pointSpeeds[i][1] = !pointSpeeds[i][1];
                }
            }
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
