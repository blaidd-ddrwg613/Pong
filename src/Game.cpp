#include "Game.h"
#include "iostream"

Game::Game()
{
    window = nullptr;
    renderer = nullptr;
    isRunning = true;
    tickCount = 0;
    paddleDir = 0;
    bVelocity = {200.0f, -235.0f};

    // Set the Ball to the center of the screen, center pos
    ballPos = {1080 / 2.0, 720 / 2.0};
    // Set the Paddle to the left side of the screen, center pos
    paddlePos = { 10.0f, (720 / 2.0) };
}

bool Game::Init()
{
    int sdlResult = SDL_Init(SDL_INIT_VIDEO);
    if (sdlResult != 0)
    {
        SDL_Log("Failed to init SDL: %s", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow("Game Programing in C++", 100, 100, 1080, 720, 0);
    if (!window)
    {
        SDL_Log("Failed to create SDL window: %s", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer)
    {
        SDL_Log("Failed to create SDL Renderer: %s", SDL_GetError());
        return false;
    }
    return true;
}

void Game::RunLoop()
{
    while(isRunning)
    {
        ProcessInput();
        UpdateGame();
        GenerateOutput();
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            isRunning = false;
        }
    }

    // Get and store the state of the keyboard and poll for keys
    const Uint8* state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_ESCAPE])
    {
        isRunning = false;
    }

    paddleDir = 0;
    if (state[SDL_SCANCODE_W])
    {
        paddleDir = -1;
    }
    if (state[SDL_SCANCODE_S])
    {
        paddleDir = 1;
    }
}

void Game::UpdateGame()
{
    // Wait 16ms
    while (!SDL_TICKS_PASSED(SDL_GetTicks(), tickCount + 16))
        ;

        float deltaTime = (SDL_GetTicks() - tickCount) / 1000.0f;
        tickCount = SDL_GetTicks();

        // Clamp Delta time
        if (deltaTime > 0.05f)
        {
            deltaTime = 0.05f;
        }

        // TODO Update objects in world
        if (paddleDir != 0)
        {
            paddlePos.y += paddleDir * paddleSpeed * deltaTime;
            // Clamp paddle to screen bounds
            if (paddlePos.y < (pHeight/2.0f + thickness))
            {
                paddlePos.y = pHeight/2.0f + thickness;
            }
            if (paddlePos.y > (720.0f - pHeight/2.0f - thickness + 10))
            {
                paddlePos.y = 720.0f - pHeight/2.0f - thickness + 10;
            }
        }

        // Update Ball pos
        ballPos.x += bVelocity.x * deltaTime;
        ballPos.y += bVelocity.y * deltaTime;

    // Bounce if needed
    // Did we intersect with the paddle?
    float diff = paddlePos.y - ballPos.y;
    // Take absolute value of difference
    diff = (diff > 0.0f) ? diff : -diff;
    if (
        // Our y-difference is small enough
            diff <= pHeight / 2.0f &&
            // We are in the correct x-position
            ballPos.x <= 25.0f && ballPos.x >= 20.0f &&
            // The ball is moving to the left
            bVelocity.x < 0.0f)
    {
        bVelocity.x *= -1.0f;
    }

        if (ballPos.x <= 0.0f)
        {
            isRunning = false;
        }

        if (ballPos.x >= (1080 - thickness) && bVelocity.x > 0.0f)
        {
            bVelocity.x *= -1;
        }

        // Top Wall Collision
        if (ballPos.y <= thickness && bVelocity.y < 0.0f)
        {
            bVelocity.y *= -1;
        }
        // Bottom Wall Collision
        if (ballPos.y >= (720 - thickness) && bVelocity.y > 0.0f)
        {
            bVelocity.y *= -1;
        }
}

void Game::GenerateOutput()
{
    // Set the color for the buffer
    SDL_SetRenderDrawColor(renderer,Purple);
    // Clear the buffer and use our set color
    SDL_RenderClear(renderer);

    // Draw
    SDL_SetRenderDrawColor(renderer, WHITE);

    // Draw Walls
    SDL_Rect wall {0, 0, 1080, thickness};
    SDL_RenderFillRect(renderer, &wall);

    wall.y = 720 - thickness;
    SDL_RenderFillRect(renderer,&wall);

    wall.x = 1080 - thickness;
    wall.y = 0;
    wall.w = thickness;
    wall.h = 720;

    // Draw Paddle
    int height = thickness * 7.5;
    SDL_Rect paddle {
        static_cast<int>(paddlePos.x - thickness/2.0f),
        static_cast<int>(paddlePos.y - height /2.0f),
        thickness,
        pHeight
    };


    // Draw Ball
    SDL_Rect ball {
        static_cast<int>(ballPos.x - thickness/2.0f),
        static_cast<int>(ballPos.y - thickness/2.0f),
        thickness,
        thickness};

    // Render all the Things
    SDL_RenderFillRect(renderer, &wall);
    SDL_RenderFillRect(renderer, &paddle);
    SDL_RenderFillRect(renderer, &ball);

    // Swap the buffers
    SDL_RenderPresent(renderer);
}

void Game::Shutdown()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
}