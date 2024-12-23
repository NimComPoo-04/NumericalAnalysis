#include "function.h"

#include <stdio.h>
#include <stdlib.h>

#include <raylib.h>
#include <raymath.h>

static Vector3 Y = {0, 1, 0};

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

			double out = func(in).it[0] * Zoom.z + Origin.z;

			if(out > hlfsam * spacing || out < -hlfsam * spacing)
				continue;

			Vector3 p = { j * spacing, out, i * spacing };

			DrawPoint3D(p, MAROON);
		}
	}
}

void draw_triangles(function_type_t func, Vector3 Origin, Vector3 Zoom, int samples, float spacing)
{
	int hlfsam = samples/2;

	for(int i = -hlfsam; i <= hlfsam; i++)
	{
		for(int j = -hlfsam; j <= hlfsam; j++)
		{
			Vector3 p[4] = {0};
			Vector3 mid = {0};

			for(int k = 0; k < 4; k++)
			{
				vec_t in = {.it = {
					Lerp(Origin.x - Zoom.x, Origin.x + Zoom.x, Normalize(j + (k == 1 || k == 2), -hlfsam, hlfsam)),
					Lerp(Origin.y - Zoom.y, Origin.y + Zoom.y, Normalize(i + (k == 0 || k == 1), -hlfsam, hlfsam)),
					0, 0
				}};

				double out = func(in).it[0] * Zoom.z + Origin.z;
				if(isnan(out) || out < -hlfsam * spacing - 0.1 || out > hlfsam * spacing + 0.1)
					goto SKIP;
	//			out = Clamp(out, -hlfsam * spacing - 0.01, hlfsam * spacing + 0.01);

				p[k] = (Vector3){ (j + (k == 1 || k == 2)) * spacing, out, (i + (k == 0 || k == 1)) * spacing };

				mid = Vector3Add(mid, p[k]);
			}

			mid = Vector3Scale(mid, 4);
			float f = Normalize(mid.y, -hlfsam * spacing, hlfsam * spacing);

			Vector3 normal = Vector3Normalize(Vector3CrossProduct(Vector3Subtract(p[0], p[2]), Vector3Subtract(p[0], p[3])));
			float t = Vector3DotProduct(normal, Y);

			Color col1 = ColorBrightness(DARKBLUE, t);
			Color col2 = ColorBrightness(DARKGRAY, t);

			int count = 0;

			count += p[0].y < -hlfsam * spacing || p[0].y > hlfsam * spacing;
			count += p[1].y < -hlfsam * spacing || p[1].y > hlfsam * spacing;
			count += p[2].y < -hlfsam * spacing || p[2].y > hlfsam * spacing;

			if(count < 4)
			{
				DrawTriangle3D(p[0], p[1], p[2], col1);
				DrawTriangle3D(p[2], p[1], p[0], col2);
			}

			count = 0;

			count += p[0].y < -hlfsam * spacing || p[0].y > hlfsam * spacing;
			count += p[3].y < -hlfsam * spacing || p[3].y > hlfsam * spacing;
			count += p[2].y < -hlfsam * spacing || p[2].y > hlfsam * spacing;

			if(count < 4)
			{
				DrawTriangle3D(p[0], p[2], p[3], col1);
				DrawTriangle3D(p[3], p[2], p[0], col2);
			}
SKIP:
		}
	}
}

void plot(function_type_t func)
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
	SetTargetFPS(60);

	InitWindow(1000, 1000, "...");

	Vector2 rotation = {0};

	static char textbuffer[1024];

	Vector3 Origin = {0};
	// Zoom is weird, x and y inreasing means to zoom out, but z its the oppsite
	Vector3 Zoom = {1, 1, 1};

	int sides = 20;
	int samples = 20;

	while(!WindowShouldClose())
	{
		float dt = GetFrameTime();

		// Rotation of the visibility cube with mouse
		if(IsMouseButtonDown(MOUSE_LEFT_BUTTON))
		{
			rotation = Vector2Scale(Vector2Normalize(GetMouseDelta()), 0.05);
			cam.position = Vector3Transform(cam.position, MatrixRotateY(-rotation.x));

			Vector3 axis = Vector3CrossProduct(cam.position, Y);
			cam.position = Vector3Transform(cam.position, MatrixRotate(axis, rotation.y));
		}

		// Increasing samples
	//	if(IsKeyPressed(KEY_U)) samples += 2;

		float spacing = 1. * sides / samples;

		float scroll = GetMouseWheelMove();
		if(scroll != 0)
		{
			if(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))
			{
				samples += scroll;
				if(samples < 20) samples = 20;
			}
			else
			{
				if(IsKeyDown(KEY_X))      Zoom.x += scroll * 0.05;
				else if(IsKeyDown(KEY_Y)) Zoom.y += scroll * 0.05;
				else if(IsKeyDown(KEY_Z)) Zoom.z += scroll * 0.05;
				else Zoom = Vector3AddValue(Zoom, scroll * 0.05);

	//			Zoom = Vector3Max(Zoom, Vector3One());
			}
		}

		// Changing origin
		if(IsKeyDown(KEY_A)) Origin.x += 0.1;
		if(IsKeyDown(KEY_D)) Origin.x -= 0.1;
		if(IsKeyDown(KEY_W)) Origin.y += 0.1;
		if(IsKeyDown(KEY_S)) Origin.y -= 0.1;
		if(IsKeyDown(KEY_Q)) Origin.z += 0.1;
		if(IsKeyDown(KEY_E)) Origin.z -= 0.1;
	

		// Drawing out the graph
		BeginDrawing();
			ClearBackground(BLACK);
			BeginMode3D(cam);
				DrawCubeWires(Vector3Zero(), sides, sides, sides, ORANGE);
				draw_grid(sides/2.f, 2.f);
				draw_triangles(func, Origin, Zoom, samples, spacing);
			EndMode3D();

			// Writing out information
			sprintf(textbuffer, "Origin = (%f %f %f)", Origin.x, Origin.y, Origin.z);
			DrawText(textbuffer, 5, 5, 20, WHITE);

			sprintf(textbuffer, "Zoom = (%f %f %f)", Zoom.x, Zoom.y, Zoom.z);
			DrawText(textbuffer, 5, 23, 20, WHITE);

			sprintf(textbuffer, "Samples = %d", samples);
			DrawText(textbuffer, 5, 43, 20, WHITE);

			DrawFPS(5, 63);

		EndDrawing();
	}
	CloseWindow();
}
