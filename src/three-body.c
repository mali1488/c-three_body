#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <limits.h>
#include "raylib.h"

#define WIDTH 800
#define HEIGHT 600

#define BODY_SIZE 2
#define G 6.67300e-11
#define EPS 10
#define N 3

typedef struct {
  float mass;
  float x;
  float y;
  float z;
  float vx;
  float vy;
  float vz;
  float fx;
  float fy;
  float fz;
  Color color;
} Body;

float distance(Body* a, Body* b) {
  return sqrt(pow((b->x - a->x), 2) + pow((b->y - a->y), 2) + pow((b->z - a->z), 2));
}

void apply_forces(Body* stars, int n, float dt) {
  for(size_t i = 0; i < n; i++) {
    float m = stars[i].mass;
    stars[i].fy = (-G * m) * stars[i].fy;
    stars[i].fx = (-G * m) * stars[i].fx;
    stars[i].fz = (-G * m) * stars[i].fz;

    float ax = stars[i].fy / m;
    float ay = stars[i].fx / m;
    float az = stars[i].fz / m;
    
    stars[i].vx += dt * ax;
    stars[i].vy += dt * ay;
    stars[i].vz += dt * az;

    stars[i].x += dt * stars[i].vx;
    stars[i].y += dt * stars[i].vy;
    stars[i].z += dt * stars[i].vz;

    stars[i].fy = 0.0;
    stars[i].fx = 0.0;
    stars[i].fz = 0.0;
  }
}

void force(Body* s1, Body* s2) {
  float d = distance(s1, s2);
  float delta_x = s1->x - s2->x;
  float delta_y = s1->y - s2->y;
  float delta_z = s1->z - s2->z;

  float m = s1->mass * s2->mass * G;
  float ds = pow(abs(d) + EPS, 3);

  s1->fy += (m * delta_x) / ds;
  s1->fx += (m * delta_y) / ds;
  s1->fz += (m * delta_z) / ds;
}

void sim(Body* bodies, int n, float dt) {
  for(size_t i = 0; i < n; i++) {
    for(size_t j = 0; j < n; j++) {
      if (i == j) continue;
      force(&bodies[i], &bodies[j]);
      apply_forces(bodies, 3, dt);
    }
  }
}

void draw_body(Body body) {
  float s = body.z * 0.02;
  if (s < 1) s = 1;
  DrawCircle(body.x + s/2, body.y + s/2, s, body.color);
}

void init_bodies(Body* bodies) {
  float mass = 10000000000000;

  float cx = WIDTH / 2;
  float cy = HEIGHT / 2;
  float p = 20;
  bodies[0].x = cx - p,
  bodies[0].y = cy - p,
  bodies[0].z = 250,
  bodies[0].mass = mass,
  bodies[0].color = BEIGE;
  
  bodies[1].x = cx + p,
  bodies[1].y = cy + p,
  bodies[1].z = 250,
  bodies[1].mass = mass*0.7,
  bodies[1].color = DARKBROWN;
  
  bodies[2].x = cx + p,
  bodies[2].y = cy - p,
  bodies[2].z = 0,
  bodies[2].mass = mass*0.6,
  bodies[2].color = DARKPURPLE;
}

Vector2 get_camera_pos(Body* bodies, int n) {
  int max_x = 0;
  int min_x = INT_MAX;
  int max_y = 0;
  int min_y = INT_MAX;
  for (size_t i = 0; i < n; i++) {
    Body b = bodies[i];
    if (b.x > max_x) max_x = b.x;
    if (b.x < min_x) min_x = b.x;

    if (b.y > max_y) max_y = b.y;
    if (b.y < min_y) min_y = b.y;
  }
  return (Vector2) { min_x + (max_x - min_x) / 2, min_y + (max_y - min_y) / 2 };
}

// 0.......................................500
//      100..125...150.....................500

int main(void) {
  Body bodies[N] = {0};
  init_bodies(&bodies);
  
  InitWindow(WIDTH, HEIGHT, "Three-body");
  SetTargetFPS(60);
  Camera2D camera = { 0 };
  // camera.target = (Vector2){ WIDTH / 2, HEIGHT / 2 };
  // camera.target = (Vector2){ 0, 0 };
  camera.offset = (Vector2){ 0, 0 };
  // camera.offset = //(Vector2){ WIDTH / 4, HEIGHT / 4 };
  // camera.offset = (Vector2){ WIDTH, HEIGHT };
  camera.rotation = 0.0f;
  camera.zoom = 1.0f;
  while (!WindowShouldClose()) {
    //camera.offset = 
    BeginDrawing();
    Vector2 pos = get_camera_pos(&bodies, N);
    camera.target = pos;
    camera.offset = pos;
    printf("%f, %f \n", pos.x, pos.y);
    BeginMode2D(camera);
      ClearBackground(BLACK);
      sim(bodies, N, GetFrameTime());
      for(size_t i = 0; i < N; i++) {
        draw_body(bodies[i]);
      }
    EndMode2D();
    EndDrawing();
  }
}