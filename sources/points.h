#pragma once
#include "utils.h"

#define POINTS_SIZE 50
#define POINTS_SPEED 0.1f
#define POINTS_TURN_CHANCE 0.0005f

// DEFINITIONS
void generate_points(float points[][2], int size);
void generete_speeds(float speeds[][2], int size);
void array_copy(float a[][2], float b[][2], int size);

// IMPLEMENTATION
#ifdef POINTS_IMPLEMENTATION
void generate_points(float points[][2], int size)
{
  for (size_t i = 0; i < size; i++)
  {
    points[i][0] = 1 + rand() % (SCREEN_WIDTH);
    points[i][1] = 1 + rand() % (SCREEN_HEIGHT);
  }
}

void generete_speeds(float speeds[][2], int size)
{
  for (size_t i = 0; i < size; i++)
  {
    speeds[i][0] = (rand() & 1) ? 1 : -1;
    speeds[i][1] = (rand() & 1) ? 1 : -1;
  }
}

void array_copy(float a[][2], float b[][2], int size)
{
  for (int i = 0; i < size; i++)
  {
    a[i][0] = b[i][0];
    a[i][1] = b[i][1];
  }
}
#endif
