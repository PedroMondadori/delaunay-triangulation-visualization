#include <stdio.h>
#include <stdlib.h>
#define DELAUNAY_IMPLEMENTATION
#include "delaunay.h"
#define POINTS_IMPLEMENTATION
#include "points.h"
#include "utils.h"

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(144);

    srand(time(NULL));

    float pointCloud2D[POINTS_SIZE][2];
    float(*points)[2] = malloc(POINTS_SIZE * 2 * sizeof(float));
    float pointSpeeds[POINTS_SIZE][2];

    generate_points(pointCloud2D, POINTS_SIZE);
    generete_speeds(pointSpeeds, POINTS_SIZE);
    array_copy(points, pointCloud2D, POINTS_SIZE);

    while (!WindowShouldClose())
    {    
        BeginDrawing();
        {
            ClearBackground(DARKGRAY);
            delaunayTriangulate(points, POINTS_SIZE);

            for (size_t i = 0; i < POINTS_SIZE; i++)
            {
                if ((points[i][0] + 1) >= SCREEN_WIDTH ||
                    (points[i][0] - 1) <= 0)
                {
                    pointSpeeds[i][0] *= -1;
                    points[i][0] += pointSpeeds[i][0];
                }
                if ((points[i][1] + 1) >= SCREEN_HEIGHT ||
                    (points[i][1] - 1) <= 0)
                {
                    pointSpeeds[i][1] *= -1;
                    points[i][1] += pointSpeeds[i][1];
                }

                points[i][0] += pointSpeeds[i][0]*POINTS_SPEED;
                points[i][1] += pointSpeeds[i][1]*POINTS_SPEED;

                // chance for the point to change directions randomly
                if (((float)rand() / (float)RAND_MAX) < POINTS_TURN_CHANCE) {
                    pointSpeeds[i][0] *= -1;
                }
                if (((float)rand() / (float)RAND_MAX) < POINTS_TURN_CHANCE) {
                    pointSpeeds[i][1] *= -1;
                }
            }
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
