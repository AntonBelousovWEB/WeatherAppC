#include <stdio.h>
#include "raylib.h"
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "env.h"
#include "cJSON.h"
#define MAX_INPUT_CHARS 11

typedef struct {
    char* string;
    size_t size;
} Response;

size_t write_chunk(void* data, size_t size, size_t nmemb, void* userdata);

int main() {
    const int screenWidth = 800;
    const int screenHeight = 450;
    int framesCounter = 0;

    InitWindow(screenWidth, screenHeight, "Weather App C");

    char cityName[MAX_INPUT_CHARS + 1] = "\0";
    int letterCount = 0;

    Rectangle textBox = { screenWidth / 2.0f - 100, 180, 225, 50 };
    bool mouseOnText = false;

    CURL* curl;
    CURLcode response;

    Response result;
    result.string = malloc(1);
    result.size = 0;

    while (!WindowShouldClose()) {
        if (CheckCollisionPointRec(GetMousePosition(), textBox)) mouseOnText = true;
        else mouseOnText = false;

        if (mouseOnText) {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);

            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && (letterCount < MAX_INPUT_CHARS)) {
                    cityName[letterCount] = (char)key;
                    cityName[letterCount + 1] = '\0';
                    letterCount++;
                }

                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                letterCount--;
                if (letterCount < 0) letterCount = 0;
                cityName[letterCount] = '\0';
            }

            if (IsKeyPressed(KEY_ENTER)) {
                char buffer[256];
                sprintf_s(
                    buffer,
                    sizeof(buffer),
                    "https://api.openweathermap.org/data/2.5/weather?q=%s&appid=%s",
                    cityName,
                    WEATHER_API_KEY
                );

                curl = curl_easy_init();
                result.string = malloc(1);
                result.size = 0;

                curl_easy_setopt(curl, CURLOPT_URL, buffer);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_chunk);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&result);

                response = curl_easy_perform(curl);

                cJSON* json = cJSON_Parse(result.string);

                if (json != NULL) {
                    cJSON* main = cJSON_GetObjectItem(json, "main");
                    cJSON* temp = cJSON_GetObjectItem(main, "temp");

                    int TempValue = temp->valueint;
                    printf("\n\nTemp: %d K\n", TempValue); // Кельвин

                    cJSON_Delete(json);
                }
                else {
                    printf("Failed to parse JSON.\n");
                }

                curl_easy_cleanup(curl);
            }
        }
        else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Type the city name and press ENTER to get the temperature", 150, 140, 20, GRAY);

        DrawRectangleRec(textBox, LIGHTGRAY);
        if (mouseOnText) DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, RED);
        else DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, DARKGRAY);

        DrawText(cityName, (int)textBox.x + 5, (int)textBox.y + 8, 40, MAROON);

        DrawText(TextFormat("INPUT CHARS: %i/%i", letterCount, MAX_INPUT_CHARS), 315, 250, 20, DARKGRAY);

        if (mouseOnText) {
            if (letterCount < MAX_INPUT_CHARS) {
                if (((framesCounter / 20) % 2) == 0) DrawText("_", (int)textBox.x + 8 + MeasureText(cityName, 40), (int)textBox.y + 12, 40, MAROON);
            }
            else DrawText("Press BACKSPACE to delete chars...", 230, 300, 20, GRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

size_t write_chunk(void* data, size_t size, size_t nmemb, void* userdata) {
    size_t real_size = size * nmemb;
    Response* response = (Response*)userdata;

    char* ptr = realloc(response->string, response->size + real_size + 1);
    if (ptr == NULL) {
        return CURL_WRITEFUNC_ERROR;
    }

    response->string = ptr;
    memcpy(&(response->string[response->size]), data, real_size);
    response->size += real_size;
    response->string[response->size] = 0;

    return real_size;
}