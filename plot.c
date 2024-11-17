#include "function.h"

#include <stdio.h>
#include <stdlib.h>

#include <raylib.h>
#include <raymath.h>

void plot3d(function_type_t func, int count);
void plot2d(function_type_t func, int count) { }

void plot(function_type_t func, int count)
{
	if(count ==  2)
	{
		plot3d(func, count);
	}
	else if(count == 1)
	{
		plot2d(func, count);
	}
	else
	{
		fprintf(stderr, "Can't plot this type of function right now sorry. %d\n", count);
		return;
	}
}

// FIXME: hardcoding the value 20 not good for health
void draw_grid(int samples, float spacing)
{
	Vector3 start, end;
	Color color;

	int hlfsam = samples/2;

	for(int i = -hlfsam; i <= hlfsam; i++)
	{
		if(i == 0) color = PURPLE;
		else       color = DARKPURPLE;

		color.r /= 2;
		color.g /= 2;
		color.b /= 2;

		start = (Vector3){i * spacing, 0, -hlfsam * spacing};
		end   = (Vector3){i * spacing, 0, hlfsam * spacing};
		DrawLine3D(start, end, color);

		start = (Vector3){-hlfsam * spacing, 0, i * spacing};
		end   = (Vector3){hlfsam * spacing, 0, i * spacing};
		DrawLine3D(start, end, color);
	}
}

void draw_points(function_type_t func, Vector3 Origin, Vector3 Zoom, int samples, float spacing)
{
	int hlfsam = samples/2;

	for(int i = -hlfsam; i <= hlfsam; i++)
	{
		for(int j = -hlfsam; j <= hlfsam; j++)
		{
			vec_t in = {.it = {
				Lerp(Origin.x - Zoom.x, Origin.x + Zoom.x, Normalize(j, -hlfsam, hlfsam)),
				Lerp(Origin.y - Zoom.y, Origin.y + Zoom.y, Normalize(i, -hlfsam, hlfsam)),
				0, 0
			}};

			double out = func(in).it[0] * Zoom.z;

			if(out > hlfsam || out < -hlfsam)
				continue;

			Vector3 p = { j * spacing, out, i * spacing };

			DrawPoint3D(p, MAROON);
		}
	}
}

void plot3d(function_type_t func, int count)
{
	Camera cam = {
		.position = {0, 30.0f, 30.0f},
		.target = {0, 0.0f, 0.0f},
		.up = {0, 1., 0},
		.fovy = 45,
		.projection = CAMERA_PERSPECTIVE
	};

	Vector3 cubePosition = {0, 0, 0};

	SetTraceLogLevel(LOG_ERROR);

	InitWindow(800, 800, "...");

	Vector3 rotation = {0};
	Vector3 Y = {0, 1, 0};

	static char textbuffer[1024];

	Vector3 Origin = {0};
	// Zoom is weird, x and y inreasing means to zoom out, but z its the oppsite
	Vector3 Zoom = {1, 1, 1};
	int samples = 20;
	float spacing = 1.f;

	while(!WindowShouldClose())
	{
		float dt = GetFrameTime();

		// Camera Rotation and things
		if(IsKeyPressed(KEY_UP))		rotation.z = 0.5f;
		else if(IsKeyPressed(KEY_DOWN))	        rotation.z = -0.5f;

		if(IsKeyPressed(KEY_LEFT))		rotation.y = -0.5f;
		else if(IsKeyPressed(KEY_RIGHT))	rotation.y = 0.5f;

		if(IsKeyReleased(KEY_UP) || IsKeyReleased(KEY_DOWN))
			rotation.z = 0.f;

		if(IsKeyReleased(KEY_LEFT) || IsKeyReleased(KEY_RIGHT))
			rotation.y = 0.f;

		cam.position = Vector3Transform(cam.position, MatrixRotateY(rotation.y * dt));
		cam.position = Vector3Transform(cam.position,
				MatrixRotate(Vector3CrossProduct(cam.position, Y), rotation.z * dt));

		if(IsKeyPressed(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_U)) samples -= 2;

		// Increasing samples
		if(IsKeyPressed(KEY_U)) samples += 2;

		spacing = 20.f / samples;

		if(IsKeyPressed(KEY_F)) Zoom.x += 0.5;
		if(IsKeyPressed(KEY_G)) Zoom.y += 0.5;
		if(IsKeyPressed(KEY_H)) Zoom.z += 0.5;
	
		BeginDrawing();
			ClearBackground(BLACK);
			BeginMode3D(cam);
				DrawCubeWires(Vector3Zero(), samples * spacing,
						samples * spacing, samples * spacing, ORANGE);
				draw_grid(10, 2.f);
				draw_points(func, Origin, Zoom, samples, spacing);
			EndMode3D();

			// Writing out information
			sprintf(textbuffer, "Origin = (%f %f %f)", Origin.x, Origin.y, Origin.z);
			DrawText(textbuffer, 5, 5, 20, WHITE);

			sprintf(textbuffer, "Zoom = (%f %f %f)", Zoom.x, Zoom.y, Zoom.z);
			DrawText(textbuffer, 5, 23, 20, WHITE);

		EndDrawing();
	}
	CloseWindow();
}
