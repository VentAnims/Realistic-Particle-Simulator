#include "raylib.h"
#include "raymath.h"
#include <math.h>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#define SCREEN_WIDTH 1000
#define SCREEN_HEIGHT 700
#define MAX_PARTICLES 3000

typedef struct {
    Vector2 pos;
    Vector2 vel;
    float radius;
    float mass;
    Color color;
} Particle;

Particle particles[MAX_PARTICLES];

float particleCountF = 100;
int particleCount = 100;

float speedMultiplier = 1.0f;
float speedMultiplierF = 1.0f;

float restitution = 1.0f;     // bounce strength | perfectly elastic
float damping = 1.0f;         // air resistance | no energy loss

bool layeringEnabled = false;

float minLayerRadius = 80.0f;
float maxLayerRadius = 300.0f;

float layeringStrength = 200.0f; // how strong the organizing force is

// Initialize particles
void InitParticles() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].radius = 6;
        particles[i].mass = particles[i].radius;

        particles[i].pos = (Vector2){ GetRandomValue(50, SCREEN_WIDTH - 50), GetRandomValue(50, SCREEN_HEIGHT - 50) };

        particles[i].vel = (Vector2){ GetRandomValue(-250, 250), GetRandomValue(-250, 250) };

        particles[i].color = BLUE;
    }
}

void DrawParticleGlow(Vector2 pos, float radius, Color color)
{
    // Draw halo layers
    int layers = 2;
    for (int i = layers; i > 0; i--) {
        float r = radius + i * 4;              // bigger radius each layer
        float alpha = (float)i / layers * 0.3f; // transparency
        Color c = color;
        c.a = (unsigned char)(alpha * 255);
        DrawCircleV(pos, r, c);
    }

    // Draw the main solid particle on top
    DrawCircleV(pos, radius, color);
}

// Resolve elastic collision
void ResolveCollision(Particle* p1, Particle* p2) {
    Vector2 normal = Vector2Subtract(p2->pos, p1->pos);
    float dist = Vector2Length(normal);

    if (dist == 0.0f) return;

    normal = Vector2Scale(normal, 1.0f / dist);

    Vector2 tangent = (Vector2){ -normal.y, normal.x };

    float v1n = Vector2DotProduct(normal, p1->vel);
    float v1t = Vector2DotProduct(tangent, p1->vel);

    float v2n = Vector2DotProduct(normal, p2->vel);
    float v2t = Vector2DotProduct(tangent, p2->vel);

    // Swap normal velocities (equal mass)
    float temp = v1n;
    v1n = v2n;
    v2n = temp;

    Vector2 v1nVec = Vector2Scale(normal, v1n);
    Vector2 v1tVec = Vector2Scale(tangent, v1t);

    Vector2 v2nVec = Vector2Scale(normal, v2n);
    Vector2 v2tVec = Vector2Scale(tangent, v2t);

    p1->vel = Vector2Add(v1nVec, v1tVec);
    p2->vel = Vector2Add(v2nVec, v2tVec);

    // Separate them to prevent sticking
    float overlap = (p1->radius + p2->radius) - dist;
    Vector2 correction = Vector2Scale(normal, overlap / 2.0f);

    p1->pos = Vector2Subtract(p1->pos, correction);
    p2->pos = Vector2Add(p2->pos, correction);
}

void UpdateParticles(float dt) {
    Vector2 center = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };

    for (int i = 0; i < particleCount; i++) {
        // Layering logic
        if (layeringEnabled) {
            Vector2 toParticle = Vector2Subtract(particles[i].pos, center);
            float currentRadius = Vector2Length(toParticle);
            
            // Map energy to target radius
            float speed = Vector2Length(particles[i].vel);
            float energy = 0.5f * particles[i].mass * speed * speed;
            float normalized = energy / 40000.0f;
            if (normalized > 1.0f) normalized = 1.0f;
            float targetRadius = minLayerRadius + normalized * (maxLayerRadius - minLayerRadius);
            
            // --- smooth force toward target shell ---
            Vector2 desiredPos = Vector2Add(center, Vector2Scale(Vector2Normalize(toParticle), targetRadius));
            Vector2 correction = Vector2Subtract(desiredPos, particles[i].pos);
            Vector2 force = Vector2Scale(correction, layeringStrength * dt);
            
            float maxForce = 50.0f;  // limit so it doesn't explode
            if (Vector2Length(force) > maxForce) force = Vector2Scale(Vector2Normalize(force), maxForce);
            
            particles[i].vel = Vector2Add(particles[i].vel, force);
        }

        // Damping
        if (layeringEnabled) {
            particles[i].vel = Vector2Scale(particles[i].vel, 0.99f);
        }

        // Move particle
        particles[i].pos.x += particles[i].vel.x * dt * speedMultiplier;
        particles[i].pos.y += particles[i].vel.y * dt * speedMultiplier;

        // Wall collisions
        if (particles[i].pos.x < particles[i].radius) {
            particles[i].pos.x = particles[i].radius;
            particles[i].vel.x *= -1.0f;
        }
        if (particles[i].pos.x > SCREEN_WIDTH - particles[i].radius) {
            particles[i].pos.x = SCREEN_WIDTH - particles[i].radius;
            particles[i].vel.x *= -1.0f;
        }
        if (particles[i].pos.y < particles[i].radius) {
            particles[i].pos.y = particles[i].radius;
            particles[i].vel.y *= -1.0f;
        }
        if (particles[i].pos.y > SCREEN_HEIGHT - particles[i].radius) {
            particles[i].pos.y = SCREEN_HEIGHT - particles[i].radius;
            particles[i].vel.y *= -1.0f;
        }

        // Color based on velocity
        float speed = Vector2Length(particles[i].vel);
        float t = speed / 400.0f;
        if (t > 1.0f) t = 1.0f;

        particles[i].color = ColorLerp(BLUE, RED, t);
    }

    // Particle collisions
    for (int i = 0; i < particleCount; i++) {
        for (int j = i + 1; j < particleCount; j++) {
            float dist = Vector2Distance(particles[i].pos, particles[j].pos);

            if (dist < particles[i].radius + particles[j].radius) {
                ResolveCollision(&particles[i], &particles[j]);
            }
        }
    }
}

void DrawParticles() {
    for (int i = 0; i < particleCount; i++) {
        DrawParticleGlow(particles[i].pos, particles[i].radius, particles[i].color);
    }
}

void DrawGUI() {

    // Particle count
    GuiLabel((Rectangle){20, 20, 120, 20}, "Particles:");
    GuiSlider((Rectangle){150, 20, 200, 20}, NULL, NULL, &particleCountF, 10, MAX_PARTICLES);
    particleCount = (int)particleCountF;

    DrawText(TextFormat("%d", particleCount), 370, 20, 20, BLACK);

    // Speed multiplier
    GuiLabel((Rectangle){20, 60, 120, 20}, "Speed:");
    GuiSlider((Rectangle){150, 60, 200, 20}, NULL, NULL, &speedMultiplierF, 0.1f, 3.0f);
    speedMultiplier = speedMultiplierF;

    DrawText(TextFormat("%.2f", speedMultiplier), 370, 60, 20, BLACK);

    DrawText(layeringEnabled ? "Layering: ON (Press F)" : "Layering: OFF (Press F)", 20, 100, 20, DARKGRAY);
}

int main() {

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Realistic Particle Simulation");
    SetTargetFPS(60);

    InitParticles();

    while (!WindowShouldClose()) {

        float dt = GetFrameTime();

        if (IsKeyPressed(KEY_F)) {
            layeringEnabled = !layeringEnabled;
        }

        UpdateParticles(dt);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawParticles();
        DrawGUI();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}