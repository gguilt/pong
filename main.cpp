/** Pong the Game | Started on October 8th 2014 - 05:11
                        Finished on December 20th 2014 - 05:06 **/

/// Header files:
#include <stdio.h>  // Used for terminal i/o
#include <cmath>    // Used for geometry rendering
#include <random>   // Used for ball auto-movement

#include <SDL2/SDL.h>       // Used for basics of SDL
#include <SDL2/SDL_image.h> // Used for font header & ball
#include <SDL2/SDL_ttf.h>   // Used for scoreboard

/// Variables & settings:
#define WINDOW_TITLE	"Pong the Game"
#define WINDOW_WIDTH	(900)
#define WINDOW_HEIGHT	(500)

bool isRestartTime = false;
int leftScore = 0, rightScore = 0;
std::string whoWin_str = " ";
std::string pressSpace_str = "Press <SPACE> for restart the game.";

struct Circle
{
    int x, y;
    int r;
};

bool game_Init(); // SDL starts up & creates window
bool game_loadMedia(); // Loads media
void game_Cleanup(); // Frees media & shuts down SDL

SDL_Window* g_Window = NULL; //  The window we'll be rendering to
SDL_Renderer* g_Renderer = NULL; // Window renderer
SDL_Texture* game_loadTexture(std::string path); // Loads individual image as texture
TTF_Font* g_Font = NULL;

bool checkCollision(Circle& a, SDL_Rect& b); // Circle - box collision detector
double distanceSquared(int x1, int y1, int x2, int y2); // Calculates distance squared between two points

/// Game classes:
// Texture class & functions
class Texture
{
	public:
		Texture(); // Initializes variables
		~Texture(); // Deallocates memory
		bool loadFromFile(std::string path); // Loads image at specified path
		bool loadFromRenderedText(std::string textureText, SDL_Color textColor); // Creates image from font string

		void free(); // Deallocates texture
		void setColor(Uint8 red, Uint8 green, Uint8 blue); // Set color modulation
		void setBlendMode(SDL_BlendMode blending); // Set blending
		void setAlpha(Uint8 alpha); // Set alpha modulation
		void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE); //Renders texture at given point

		// Gets image dimensions
		int getWidth();
		int getHeight();

	private:
		SDL_Texture* mTexture; // The actual hardware texture
		int mWidth, mHeight; // Image dimensions
};

// Scene textures
Texture gDotTexture;

Texture::Texture()
{
	// Initialize
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

Texture::~Texture()
{
	free(); // Deallocate
}

bool Texture::loadFromFile(std::string path)
{
	free(); // Get rid of preexisting texture
	SDL_Texture* newTexture = NULL; // The final texture
	SDL_Surface* loadedSurface = IMG_Load(path.c_str()); // Load image at specified path

	if(loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF)); // Color key image
        newTexture = SDL_CreateTextureFromSurface(g_Renderer, loadedSurface); // Create texture from surface pixels
		if( newTexture == NULL )
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			// Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}
		SDL_FreeSurface(loadedSurface); // Get rid of old loaded surface
	}
	mTexture = newTexture; // Return success
	return mTexture != NULL;
}

bool Texture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
	free(); // Get rid of preexisting texture
	SDL_Surface* textSurface = TTF_RenderText_Solid(g_Font, textureText.c_str(), textColor); // Render text surface

	if(textSurface != NULL)
	{
        mTexture = SDL_CreateTextureFromSurface(g_Renderer, textSurface); // Create texture from surface pixels

		if(mTexture == NULL)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			// Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}
		SDL_FreeSurface(textSurface); // Get rid of old surface
	}
	else
	{
		printf("Unable to render text surface! SDL_TTF Error: %s\n", TTF_GetError());
	}

	return mTexture != NULL; // Return success
}

void Texture::free()
{
	if(mTexture != NULL) // Free texture if it exists
	{
		SDL_DestroyTexture(mTexture);
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void Texture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	SDL_SetTextureColorMod(mTexture, red, green, blue); // Modulate texture RGB
}

void Texture::setBlendMode(SDL_BlendMode blending)
{
	SDL_SetTextureBlendMode(mTexture, blending); // Set blending function
}

void Texture::setAlpha(Uint8 alpha)
{
	SDL_SetTextureAlphaMod(mTexture, alpha); // Modulate texture alpha
}

void Texture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip)
{
	SDL_Rect renderQuad = { x, y, mWidth, mHeight }; // Set rendering space and render to screen

	if(clip != NULL) // Set clip rendering dimensions
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopyEx(g_Renderer, mTexture, clip, &renderQuad, angle, center, flip); // Render to screen
}

int Texture::getWidth()
{
	return mWidth;
}

int Texture::getHeight()
{
	return mHeight;
}

// Score board
Texture g_whoWin, g_pressSpace;
Texture g_leftScore, g_rightScore;

// Dot class & functions
class Dot {
    public:
        static const int DOT_WIDTH = 10;
        static const int DOT_HEIGHT = 10;
        static const int DOT_VEL = 1; // Maximum axis velocity of the dot

        Dot(int x, int y); // Initializes the variables

        void move(SDL_Rect& square, SDL_Rect& square2); // Moves the dot and checks collision
        void render(); // Shows the dot on the screen
        Circle& getCollider(); // Gets collision circle

    private:
        float mPosX, mPosY; // The X and Y offsets of the dot
        float mVelX, mVelY; // The velocity of the dot
        Circle mCollider; // Dot's collision circle
        void shiftColliders(); // Moves the collision circle relative to the dot's offset
};

int getRandomNumber(int higher, int lower)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<int> dist(lower, higher);
    return dist(generator);
}

Dot::Dot(int x, int y)
{
    // Initialize the offsets
    mPosX = x;
    mPosY = y;
    mCollider.r = DOT_WIDTH / 2; // Set collision circle size

    // Initialize the velocity
    mVelX = cos(getRandomNumber(2, -2));
    mVelY = sin(getRandomNumber(2, -2));

    shiftColliders(); // Move collider relative to the circle
}

void Dot::move(SDL_Rect& square, SDL_Rect& square2)
{
    SDL_Color textColor = {00, 66, 255};

    // Speed reducer
    if(mVelX > 1 || mVelX < -1)
    {
        if(mVelX > 1)
        {
            mVelX = cos(getRandomNumber(2, 0));
        }
        else
        {
            mVelX = cos(getRandomNumber(2, 0));
        }
    }

    if(mVelY > 1 || mVelY < -1)
    {
        if(mVelY > 1)
        {
            mVelY = sin(getRandomNumber(0, -2));
        }
        else
        {
            mVelY = sin(getRandomNumber(0, -2));
        }
    }

    // Move the dot left or right
    mPosX += mVelX;
	shiftColliders();

    // If the dot collided or went too far to the left or right
    if((mPosX <= -20) || (mPosX + DOT_WIDTH >= WINDOW_WIDTH + 20))
    {
        if(mPosX <= -20)
        {
            if(rightScore >= 30 && rightScore >= leftScore)
            {
                whoWin_str = "Left player is winner!!";
                g_whoWin.loadFromRenderedText(whoWin_str, textColor);

                g_whoWin.render((WINDOW_WIDTH / 2) - g_whoWin.getWidth(), WINDOW_HEIGHT / 2);
                g_pressSpace.render(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 28);
                isRestartTime = true;
            }
            else
            {
                rightScore++;
                g_rightScore.loadFromRenderedText(std::to_string(rightScore), textColor);
                mPosX = WINDOW_WIDTH / 2, mPosY = WINDOW_HEIGHT / 2;
                mVelX = -mVelX;
                mVelY = -mVelY;
            }
        }
        else if(mPosX + DOT_WIDTH >= WINDOW_WIDTH + 20)
        {
            if(leftScore >= 30 && leftScore >= rightScore)
            {
                whoWin_str = "Right player is winner!!";
                g_whoWin.loadFromRenderedText(whoWin_str, textColor);

                g_whoWin.render(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
                g_pressSpace.render((WINDOW_WIDTH / 2) - g_pressSpace.getWidth(), WINDOW_HEIGHT / 2 + 28);
                isRestartTime = true;
            }
            else
            {
                leftScore++;
                g_leftScore.loadFromRenderedText(std::to_string(leftScore), textColor);
                mPosX = WINDOW_WIDTH / 2, mPosY = WINDOW_HEIGHT / 2;
                mVelX = -mVelX;
                mVelY = -mVelY;
            }
        }
    }

    // Move the dot up or down
    mPosY += mVelY;
	shiftColliders();

    // If the dot collided or went too far up or down
    if((mPosY + mCollider.r <= 0) || (mPosY + DOT_HEIGHT > WINDOW_HEIGHT))
    {
        mVelY = -mVelY;
    }

    if(checkCollision(mCollider, square) || checkCollision(mCollider, square2))
    {
        mVelX = -mVelX;
        mVelY += 0.306; // Now humans can't detect the ball movement
        mVelY = -mVelY;
    }
}

void Dot::render()
{
    gDotTexture.render(mPosX - mCollider.r, mPosY - mCollider.r); // Show the dot
}

Circle& Dot::getCollider()
{
	return mCollider;
}

void Dot::shiftColliders()
{
	//Align collider to center of dot
	mCollider.x = mPosX;
	mCollider.y = mPosY;
}

bool checkCollision(Circle& a, SDL_Rect& b)
{
    int cX, cY; // Closest point on collision box

    // Find closest x offset
    if(a.x <= b.x)
    {
        cX = b.x;
    }
    else if(a.x >= b.x + b.w)
    {
        cX = b.x + b.w;
    }
    else
    {
        cX = a.x;
    }

    // Find closest y offset
    if(a.y <= b.y)
    {
        cY = b.y;
    }
    else if(a.y >= b.y + b.h)
    {
        cY = b.y + b.h;
    }
    else
    {
        cY = a.y;
    }

    // If the closest point is inside the circle
    if(distanceSquared(a.x, a.y, cX, cY) < a.r * a.r)
    {
        return true; // This box and the circle have collided
    }

    return false; // If the shapes have not collided
}

double distanceSquared(int x1, int y1, int x2, int y2)
{
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    return deltaX*deltaX + deltaY*deltaY;
}

/// Game functions:
bool game_Init()
{
	bool success = true; // Control flag initialization

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("[error] SDL could not initialized! (%s)\n", SDL_GetError());
		success = false;
	}
	else
	{
		if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("[warning] Linear texture filtering not enabled!\n");
		}

		g_Window = SDL_CreateWindow(WINDOW_TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
		if(g_Window == NULL)
		{
			printf("[error] Window could not be created! (%s)\n", SDL_GetError());
			success = false;
		}
		else
		{
			g_Renderer = SDL_CreateRenderer(g_Window, -1, SDL_RENDERER_ACCELERATED);

			if(g_Renderer == NULL)
			{
				printf("[error] Renderer could not be created! (%s)\n", SDL_GetError());
				success = false;
			}
			else
			{
				SDL_SetRenderDrawColor(g_Renderer, 0x00, 0x00, 0x00, 0xFF);

				// Initialize PNG loading:
				int imgFlags = IMG_INIT_PNG;
				if(!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("[error] SDL_image could not initialize! (%s)\n", IMG_GetError());
                    success = false;
                }

                // Initialize SDL_TTF
                if(TTF_Init() == -1)
                {
                    printf("[error] SDL_ttf could not initialize! (%s)\n", TTF_GetError());
                    success = false;
                }
			}
		}
	}

	return success;
}

bool game_loadMedia()
{
	bool success = true;

    // Loading the dot:
	if(!gDotTexture.loadFromFile("dot.bmp"))
    {
        printf("Failed to load dot texture!\n");
        success = false;
    }

    // Loading the scoreboard:
    g_Font = TTF_OpenFont("BebasNeue.otf", 28); // Font name, font size
    if(g_Font == NULL)
    {
        printf("[error] Failed to load font! (%s)\n", TTF_GetError());
        success = false;
    }
    else
    {
        SDL_Color textColor = {00, 66, 255};
        if(!g_leftScore.loadFromRenderedText(std::to_string(leftScore), textColor))
        {
            printf("[error] Failed to render text texture! (leftScore)\n");
            success = false;
        }
        if(!g_rightScore.loadFromRenderedText(std::to_string(rightScore), textColor))
        {
            printf("[error] Failed to render text texture! (rightScore)\n");
            success = false;
        }
        if(!g_whoWin.loadFromRenderedText(whoWin_str, textColor))
        {
            printf("[error] Failed to render text texture! (whoWin)\n");
            success = false;
        }

        textColor = { 106, 66, 255 };
        if(!g_pressSpace.loadFromRenderedText(pressSpace_str, textColor))
        {
            printf("[error] Failed to render text texture! (pressSpace)\n");
            success = false;
        }
    }

	return success;
}

void game_Cleanup()
{
	SDL_DestroyRenderer(g_Renderer);
	SDL_DestroyWindow(g_Window);
	g_Window = NULL;
	g_Renderer = NULL;

	// Scoreboard:
	g_whoWin.free();
	g_pressSpace.free();
	g_leftScore.free();
	g_rightScore.free();
	TTF_CloseFont(g_Font);
	g_Font = NULL;

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* argv[]) // The main function
{
	if(!game_Init())
	{
		printf("[error] Failed to initialize!\n");
	}
	else
	{
		if(!game_loadMedia())
		{
			printf("[error] Failed to load media!\n");
		}
		else
		{
			bool quit = false;
			SDL_Event e; // Event handler

			// Drawing the player objects
			SDL_Rect left_fillRect = { WINDOW_WIDTH-40, WINDOW_HEIGHT/2-100, 20, 100 }; // Player left
			SDL_Rect right_fillRect = { 10, WINDOW_HEIGHT/2-100, 20, 100 }; // Player right

			Dot dot(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2); // The dot that will be moving around on the screen

			while(!quit)
			{
				// Clear the screen
				SDL_SetRenderDrawColor(g_Renderer, 0x11, 0x11, 0x19, 0xFF);
				SDL_RenderClear(g_Renderer);

				// Drawing a vertical line
				SDL_SetRenderDrawColor(g_Renderer, 0x00, 0x66, 0xFF, 0xFF);
				SDL_RenderDrawLine(g_Renderer, WINDOW_WIDTH/2, 0, WINDOW_WIDTH/2, WINDOW_HEIGHT);

				// Updating player objects
				SDL_SetRenderDrawColor(g_Renderer, 0x00, 0x66, 0xFF, 0xFF);
				SDL_RenderFillRect(g_Renderer, &left_fillRect);
				SDL_RenderFillRect(g_Renderer, &right_fillRect);

				while(SDL_PollEvent(&e) != 0) // Handle events on queue
                {
                    if(e.type == SDL_QUIT) // User requests quit
                    {
                        quit = true;
                    }
                }
                // Get keyboard state based keyboard handler
                const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
                if(currentKeyStates[SDL_SCANCODE_SPACE] && isRestartTime == true)
                {
                    SDL_Color textColor = { 00, 66, 255 };
                    leftScore = 0; rightScore = 0;
                    left_fillRect.y = WINDOW_HEIGHT/2-100;
                    right_fillRect.y = WINDOW_HEIGHT/2-100;
                    g_leftScore.loadFromRenderedText(std::to_string(leftScore), textColor);
                    g_rightScore.loadFromRenderedText(std::to_string(rightScore), textColor);
                    isRestartTime = false;
                }

                if(currentKeyStates[SDL_SCANCODE_UP])
                {
                    if(left_fillRect.y != 2)
                        left_fillRect.y--;
                }
                else if(currentKeyStates[SDL_SCANCODE_DOWN])
                {
                    if(left_fillRect.y != ((WINDOW_HEIGHT-left_fillRect.h)-2))
                        left_fillRect.y++;
                }

                if(currentKeyStates[SDL_SCANCODE_W])
                {
                    if(right_fillRect.y != 2)
                        right_fillRect.y--;
                }
                else if(currentKeyStates[SDL_SCANCODE_S])
                {
                    if(right_fillRect.y != ((WINDOW_HEIGHT-left_fillRect.h)-2))
                        right_fillRect.y++;
                }

                dot.move(right_fillRect, left_fillRect); // Dot movement & collision
                dot.render(); // Rendering the dot

                g_leftScore.render((WINDOW_WIDTH / 2) - g_leftScore.getWidth() - 10, WINDOW_HEIGHT-g_leftScore.getHeight());
                g_rightScore.render((WINDOW_WIDTH / 2)+10, WINDOW_HEIGHT-g_rightScore.getHeight());
				SDL_RenderPresent(g_Renderer); // Update the screen
			}
		}
	}

	game_Cleanup();
	return 0;
}

// The end of the file.
