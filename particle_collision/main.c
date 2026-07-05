#include <raylib.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

#define WIDTH 900
#define HEIGHT 600
#define RAD 8
#define PARTICLE_COUNT 400
#define GRAVITY 0.0f
#define STEP_TIME 1.0f
#define DAMPING_FORCE 0.02f

// define a particle structure
typedef struct {
    Vector2 coords; // an object (x,y) depicting particle position
    Vector2 speed;  // an object (x,y) depicting the particle speed
    float r;
} Particle;

// Global Variables
Particle particles[PARTICLE_COUNT];

// --------------------- HELPER FUNCTIONS --------------------- //

void swap_speeds(Vector2 *A, Vector2 *B) {
    Vector2 C = *A;
    *A = *B;
    *B = C;
}

// --------------------- HELPER FUNCTIONS --------------------- //

// --------------------- RAYLIB FUNCTIONS --------------------- //

// Draw Particle
void DrawParticle(Particle *particle) {
    DrawCircle(particle->coords.x, particle->coords.y, particle->r, WHITE);
}

// Update Particle
void UpdateParticle(Particle *particle) {
    // apply gravitational force downwards
    // semi-euler integration (velocity first, position next)
    particle->speed.y += (GRAVITY) * STEP_TIME;

    particle->coords.x += particle->speed.x;
    particle->coords.y += (particle->speed.y);

    // detect collision with the boundaries
    if (particle->coords.x <= RAD) {
        particle->coords.x = RAD;
        // left boundary
        particle->speed.x *= -1;
    }

    if (particle->coords.x >= WIDTH - RAD) {
        particle->coords.x = WIDTH - RAD;
        // right boundary
        particle->speed.x *= -1;
    }

    if (particle->coords.y <= RAD) {
        particle->coords.y = RAD;
        // top boundary
        particle->speed.y *= -1;
        particle->speed.y -= (particle->speed.y * DAMPING_FORCE);
    }

    if (particle->coords.y >= HEIGHT - RAD) {
        particle->coords.y = HEIGHT - RAD;
        // bottom boundary
        particle->speed.y *= -1;
        particle->speed.y -= (particle->speed.y * DAMPING_FORCE);
    }

}

// Initialize particles
void InitParticles() {
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        particles[i].coords.x = GetRandomValue(RAD, WIDTH - RAD);
        particles[i].coords.y = GetRandomValue(RAD, HEIGHT - RAD);
        particles[i].r = RAD;

        int speedX = GetRandomValue(-15, 15);
        int speedY = GetRandomValue(-15, 15);
        particles[i].speed.x = speedX == 0 ? 5 : speedX;
        particles[i].speed.y = speedY == 0 ? 5 : speedY;
    }
}

// Draw particles
void DrawParticles() {
    for (int i = 0; i < PARTICLE_COUNT; i++)
        DrawParticle(&particles[i]);
}

// Update Particles
void UpdateParticles() {
    for (int i = 0; i < PARTICLE_COUNT - 1; i++) {
        for (int j = i + 1; j < PARTICLE_COUNT; j++) {
            // check if particle X has collided with every other particle
            if (CheckCollisionCircles(particles[i].coords, particles[i].r,
                        particles[j].coords, particles[j].r)) {

                // reflect (particle A, particle B)
                /*********************************************
                 *
                 * **** Applying 1D NEWTONIAN MOTION in 2D ****
                 *
                 * A Before collision :  Ax1, Ay1
                 * B Before collision :  Bx1, By1
                 *
                 * A after collision  :  Bx1, By1
                 * B after collision  :  Ax1, Ay1
                 *
                 * **** Law of Conservation of Momentum ****
                 *
                 * Ax2 = Bx1
                 * Ay2 = By1
                 * Bx2 = Ax1
                 * By2 = Ay1
                 *
                 * // swapping the velocities of the two colliding balls
                 * swap_speeds(&particles[i].speed, &particles[j].speed);
                 *
                 ********************************************/

                float dx = (particles[j].coords.x - particles[i].coords.x);
                float dy = (particles[j].coords.y - particles[i].coords.y);

                float angle = atan2f(dy, dx); // angle in radians
                                              // printf("Angle: %f\n", angle);

                float sumOfRadii = particles[i].r + particles[j].r;
                float abs_d = sqrtf(powf(dy, 2.0f) + powf(dx, 2.0f)); 
                float distanceToMove = sumOfRadii - abs_d;

                // simulating collision - preventing particle overlap
                particles[i].coords.x -= (cosf(angle) * (distanceToMove / 2));
                particles[i].coords.y -= (sinf(angle) * (distanceToMove / 2));
                particles[j].coords.x += (cosf(angle) * (distanceToMove / 2));
                particles[j].coords.y += (sinf(angle) * (distanceToMove / 2));

                // making the particles reflect off of each other
                // get the unit vectors of the normal and tangent

                float nx = dx / abs_d;
                float ny = dy / abs_d;

                // this is because the normal and tangent are perpendicular
                // therefore the dot product is 0 
                float tx = -ny;
                float ty = nx;

                // get the final velocities
                // particle i --> v1f = v2i,n * n^ + v1i,t * t^ 
                // particle j --> v2f = v1i,n * n^ + v2i,t * t^
                //
                // v2in = p2.vx * nx + p2.vy * ny
                // v1in = p1.v1 * nx + p1.vy * ny
                float v1in = particles[i].speed.x * nx + particles[i].speed.y * ny;
                float v2in = particles[j].speed.x * nx + particles[j].speed.y * ny;

                float v1it = particles[i].speed.x * tx + particles[i].speed.y * ty;
                float v2it = particles[j].speed.x * tx + particles[j].speed.y * ty;	

                float v1fx = v2in * nx + v1it * tx;
                float v1fy = v2in * ny + v1it * ty;

                float v2fx = v1in * nx + v2it * tx;
                float v2fy = v1in * ny + v2it * ty;

                // update the speeds of the particles
                particles[i].speed.x = v1fx;
                particles[i].speed.y = v1fy;

                particles[j].speed.x = v2fx;
                particles[j].speed.y = v2fy;

                // apply dampening effect on the particles
                particles[i].speed.x -= (particles[i].speed.x * DAMPING_FORCE);               
                particles[i].speed.y -= (particles[i].speed.y * DAMPING_FORCE);                
                particles[j].speed.x -= (particles[j].speed.x * DAMPING_FORCE);               
                particles[j].speed.y -= (particles[j].speed.y * DAMPING_FORCE);
            }
        }
    }

    for (int i = 0; i < PARTICLE_COUNT; i++) {
        UpdateParticle(&particles[i]);
    }

}

// --------------------- RAYLIB FUNCTIONS ---------------------  //

int main(void) {
    InitWindow(WIDTH, HEIGHT, "Particle Collision Simulation");
    SetTargetFPS(60);
    SetRandomSeed(time(NULL));

    InitParticles();
    while (!WindowShouldClose()) {

        BeginDrawing();

        DrawFPS(30, 30);

        // black canvas
        ClearBackground(BLACK);

        // draw the particles on the canvas
        DrawParticles();

        // update particles
        UpdateParticles();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
