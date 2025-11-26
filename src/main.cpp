#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>

#include <vector>

struct Vector2D { float x, y; };

const int DEFAULT_SCREEN_WIDTH = 1920;
const int DEFAULT_SCREEN_HEIGHT = 1080;
const Uint8 SCROLL_SCALE_FACTOR = 8;

struct Rect {SDL_FRect rect; Uint8 greyScale;};

int main(int, char**)
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("SDL3 Test", DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT, 0);
    if (window == nullptr)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return 1;
    }
    SDL_Log("Window initialized");

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return 1;
    }
    SDL_Log("Game initialized");

    Vector2D start{DEFAULT_SCREEN_WIDTH / 2, DEFAULT_SCREEN_WIDTH / 2};
    Vector2D end{DEFAULT_SCREEN_WIDTH / 2, DEFAULT_SCREEN_WIDTH / 2};
    Uint8 colorGreyScale = 0xff / 2;

    bool shouldQuit = false;
    SDL_Event event;

    std::vector<Rect> rects;

    while (!shouldQuit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                shouldQuit = true;
                break;
            
            // KEY BOARD
            case SDL_EVENT_KEY_DOWN:
                if (event.key.key == SDLK_ESCAPE)
                {
                    rects.clear();
                }
                break;

            // MOUSE MOTION
            case SDL_EVENT_MOUSE_MOTION:
                if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LEFT)
                {
                    end.x = event.motion.x;
                    end.y = event.motion.y;
                }
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                colorGreyScale = colorGreyScale + event.wheel.integer_y * SCROLL_SCALE_FACTOR;
                break;
            
            // MOUSE BUTTONS
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    start.x = event.button.x;
                    start.y = event.button.y;
                    end.x = event.button.x;
                    end.y = event.button.y;
                }
                else if (
                    event.button.button == SDL_BUTTON_RIGHT &&
                    rects.size() > 0
                ) {
                    rects.pop_back();
                }
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    end.x = start.x;
                    end.y = start.y;

                    Rect rect = {
                        {start.x, start.y, event.button.x - start.x, event.button.y - start.y},
                        colorGreyScale
                    };
                    rects.push_back(rect);
                }
                break;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        for (auto rect : rects)
        {
            SDL_SetRenderDrawColor(
                renderer,
                rect.greyScale,
                rect.greyScale,
                rect.greyScale,
                SDL_ALPHA_OPAQUE
            );
            SDL_RenderFillRect(renderer, &rect.rect);
        }

        SDL_SetRenderDrawColor(renderer, colorGreyScale, colorGreyScale, colorGreyScale, SDL_ALPHA_OPAQUE);
        SDL_FRect rect = {start.x, start.y, end.x - start.x, end.y - start.y};
        SDL_RenderFillRect(renderer, &rect);
        
        SDL_RenderPresent(renderer);

        SDL_Delay(10);
    }

    SDL_Quit();
    return 0;
}