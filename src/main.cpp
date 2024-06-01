#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <raylib.h>

Image target_image = LoadImage("./res/macaco.png");
Color* colors = LoadImageColors(target_image);

const int screen_width = target_image.width;
const int screen_height = target_image.height;

int num_of_tons = 2;
Color get_black_white(int x, int y);
void box_blur(Color* pixels, int width, int height, int size = 1);
void dumb_edge_filter(Color* pixels, int width, int height, float limit = 0.1f);
void not_so_dumb_edge_filter(Color* pixels, int width, int height, float limit = 0.1f);
void rancolorize_image(Color* pixels, int width, int height);

int main() {
    SetTraceLogLevel(LOG_WARNING);
    InitWindow(screen_width*2, screen_height, "EdgeFilter");
    Texture target_texture = LoadTextureFromImage(target_image);
    SetTargetFPS(30);

    //box_blur(colors, screen_width, screen_height);
    not_so_dumb_edge_filter(colors, screen_width, screen_height, 0.01f);
    //rancolorize_image(colors, screen_width, screen_height);
    //box_blur(colors, screen_width, screen_height, 5);
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_UP)) num_of_tons += 1;
        if (IsKeyDown(KEY_DOWN)) num_of_tons -= 1;

        BeginDrawing();
        ClearBackground(BLACK);
        for (int x = 0; x < screen_width; x++) {
            for (int y = 0; y < screen_height; y++) {
                DrawPixel(x, y, colors[x + (y * screen_width)]);
                //DrawPixel(x, y, get_black_white(x, y));
            }
        }
        DrawTexture(target_texture, target_image.width, 0, WHITE);
        EndDrawing();
    }

    UnloadTexture(target_texture);
    UnloadImage(target_image);
    CloseWindow();
}

Color get_black_white(int x, int y) {
    int i = x + (y * screen_width);
    Color color = colors[i];
    int brightness = (color.r + color.g + color.b) / 3;

    //int num_of_tons = 255;
    int ton = num_of_tons <= 0 ? 0 :  brightness % num_of_tons;
    brightness = (255 * ton) % 256;

    color.r = (color.r * brightness) / 255;
    color.g = (color.g * brightness) / 255;
    color.b = (color.b * brightness) / 255;
    return color;
}

void box_blur(Color* pixels, int width, int height, int size) {
    Color npixels[width*height];

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int i = x + (y*width);
            int r=0, g=0, b=0, a=0, d=0;


            for (int dx = -size; dx <= size; dx++) {
                for (int dy = -size; dy <= size; dy++) {
                    int ci = (x + dx) + ((y + dy) * width);
                    if (ci >= 0 && ci < width*height) {
                        Color pixel = pixels[ci];
                        r += pixel.r;
                        g += pixel.g;
                        b += pixel.b;
                        a += pixel.a;
                        d++;
                    }
                }
            } 

            r /= d;
            g /= d;
            b /= d;
            a = 255;
            npixels[i] = Color {(unsigned char)r, (unsigned char)(g), (unsigned char)(b), (unsigned char)(a)};
        }
    }

    for (int i = 0; i < width*height; i++) pixels[i] = npixels[i];
}

void dumb_edge_filter(Color* pixels, int width, int height, float limit) {
    Color npixels[width*height];
    for (int i = 0; i < width*height; i++) npixels[i] = BLACK;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < width; y++) {
            int curr_i = x + (y*width);
            Color curr_pixel = pixels[curr_i];
            float curr_bright = (curr_pixel.r+curr_pixel.g+curr_pixel.b)/3.0f;

            int top_i = x + ((y-1) * width);
            int bottom_i = x + ((y+1) * width);
            int left_i = (x-1) + (y * width);
            int right_i = (x+1) + (y * width);
            int sides[] = {top_i, bottom_i, left_i, right_i};

            for (int i = 0; i < 4; i++) {
                int side_i = sides[i];
                if (side_i >= 0 && side_i < width*height) {
                    Color other_pixel = pixels[side_i];
                    float other_bright = (other_pixel.r+other_pixel.g+other_pixel.b)/3.0f;
                    float delta_bright = fabsf(other_bright - curr_bright);

                    if (delta_bright/255.0f >= limit) {
                        unsigned char bright = (unsigned char) delta_bright;
                        npixels[curr_i].r += bright/2;
                        npixels[curr_i].g += bright/2;
                        npixels[curr_i].b += bright/2;
                        npixels[curr_i].a = 255;
                    }
                }
            }
        }
    }

    for (int i = 0; i < width*height; i++) pixels[i] = npixels[i];
}

void not_so_dumb_edge_filter(Color* pixels, int width, int height, float limit) {
    Color npixels[width*height];
    for (int i = 0; i < width*height; i++) npixels[i] = BLACK;

    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            int curr_i = x + (y*width);
            Color curr_pixel = pixels[curr_i];
            float curr_bright = (curr_pixel.r+curr_pixel.g+curr_pixel.b)/3.0f;

            int top_i = x + ((y-1) * width);
            int bottom_i = x + ((y+1) * width);
            int left_i = (x-1) + (y * width);
            int right_i = (x+1) + (y * width);
            int sides[] = {top_i, bottom_i, left_i, right_i};

            for (int i = 0; i < 4; i++) {
                int side_i = sides[i];
                if (side_i >= 0 && side_i < width*height) {
                    Color other_pixel = pixels[side_i];
                    float other_bright = (other_pixel.r+other_pixel.g+other_pixel.b)/3.0f;
                    float delta_bright = fabsf(other_bright - curr_bright);

                    if (delta_bright/255.0f >= limit) {
                        unsigned char bright = (unsigned char) (curr_bright + delta_bright)/2;
                        npixels[curr_i].r += bright/2;
                        npixels[curr_i].g += bright/2;
                        npixels[curr_i].b += bright/2;
                        npixels[curr_i].a = 255;
                    }
                }
            }
        }
    }

    for (int i = 0; i < width*height; i++) pixels[i] = npixels[i];
}

void rancolorize_image(Color* pixels, int width, int height) {
    int hue = 0;
    for (int i = 0; i < width*height; i++) {
        Vector3 hsv = ColorToHSV(pixels[i]);
        hsv.x = hue;
        if (i%width/2 == 0) hue = (hue+(rand()%40))%360;
        if (i%width==0) hue = rand() % 360;
        pixels[i] = ColorFromHSV(hsv.x, 1, hsv.z);
    }
}
