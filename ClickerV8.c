#include <raylib.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>

#define NUM_FRAMES 3
#define NUM_BUILDINGS 6

typedef enum {
    NONE = 0,
    CLICKER,
    GRANDMA,
    BAKERY,
    FACTORY,
    MAXPOWER
    } BuildingSetting;
    
static const char *buildingText[] = {
    "none",
    "clicker: 10",
    "grandma: 100",
    "bakery: 500",
    "factory: 1,000",
    "MAX POWER: 2,000"
};

typedef struct {
    int cookies;
    int clickers;
    int clickers_value;
    int grandmas;
    int grandmas_value;
    int bakeries;
    int bakeries_value;
    int factories;
    int factories_value;
    int increment;
    int max_power;
    int max_power_value;
} GameState;

GameState gameState = {0, 0, 1, 0, 5, 0, 25, 0, 100, 1, 0, 1000000};
pthread_mutex_t mutex_clickers, mutex_grandmas, mutex_bakeries, mutex_factories, mutex_max_power = PTHREAD_MUTEX_INITIALIZER;

void* clicker_increase(void* arg)
{
    while (true) {
        while (gameState.clickers >= 1){ 
            for (int i = 0; i < 10; i++) {
                usleep(6000000 / gameState.clickers);
            }        
            pthread_mutex_lock(&mutex_clickers);
            gameState.cookies += gameState.increment;
            pthread_mutex_unlock(&mutex_clickers);       
        }
    }
    return NULL;
}

void* grandmas_increase(void* arg)
{
    while (true) {
        while (gameState.grandmas >= 1) {
            for (int i = 0; i < 10; i++) {
                usleep(6000000 / (gameState.grandmas * gameState.grandmas_value));
            }            
            pthread_mutex_lock(&mutex_grandmas);
            gameState.cookies++;
            pthread_mutex_unlock(&mutex_grandmas);        
        }
    }
    return NULL;
}

void* bakeries_increase(void* arg)
{
    while (true) {
        while (gameState.bakeries >= 1) {
            for (int i = 0; i < 10; i++) {
                usleep(6000000 / (gameState.bakeries * gameState.bakeries_value));
            }
            pthread_mutex_lock(&mutex_bakeries);
            gameState.cookies += gameState.increment;
            pthread_mutex_unlock(&mutex_bakeries);        
        }
    }
    return NULL;
}

void* factories_increase(void* arg)
{
    while (true) {
        while (gameState.factories >= 1) {
            for (int i = 0; i < 10; i++) {
                usleep(6000000 / (gameState.factories * gameState.factories_value));
            }
            pthread_mutex_lock(&mutex_factories);
            gameState.cookies++ ;
            pthread_mutex_unlock(&mutex_factories);      
        }            
    }
    return NULL;
}

void* max_power_increase(void* arg)
{
    while (true) {
        while (gameState.max_power >= 1) {
            for (int i = 0; i < 10; i++) {
                usleep(6000000 / (gameState.max_power * gameState.max_power_value));
            }
            pthread_mutex_lock(&mutex_max_power);
            gameState.cookies++ ;
            pthread_mutex_unlock(&mutex_max_power);      
        }            
    }
    return NULL;
}

int main(void) {
    const int screenWidth = 600;
    const int screenHeight = 800;
    
    int currentBuilding = NONE;
    
    InitWindow(screenWidth, screenHeight, "Cookie Clicker Ripoff V7");
    Image icon = LoadImage("textures/ShidIcon.png");
    SetWindowIcon(icon);
    
    Texture2D texture = LoadTexture("textures/ShidCookieStack.png");
    
    Vector2 mousePoint = { 0.0f, 0.0f };
    
    float frameHeight = (float)texture.height/NUM_FRAMES;
    
    Rectangle toggleRecs[NUM_BUILDINGS] = { 0 };
    int mouseHoverRec = -1;
    Rectangle sourceRec = { 0, 0, (float)texture.width, frameHeight };
    Rectangle theCookie = { screenWidth/2 - texture.width/2, screenHeight/2 - texture.height/NUM_FRAMES/2, (float)texture.width, frameHeight };
    
        for (int i = 0; i < NUM_BUILDINGS; i++) toggleRecs[i] = (Rectangle){ 425.0f, (float)(75 +32*i), 150.0f, 30.0f };

    pthread_t clickerThread, grandmaThread, bakeryThread, factoryThread, maxpowerThread;
    pthread_create(&clickerThread, NULL, clicker_increase, NULL);
    pthread_create(&grandmaThread, NULL, grandmas_increase, NULL);
    pthread_create(&bakeryThread, NULL, bakeries_increase, NULL);
    pthread_create(&factoryThread, NULL, factories_increase, NULL);
    pthread_create(&maxpowerThread, NULL, max_power_increase, NULL);
    
    int btnState = 0;
    bool btnAction = false;
    bool cookieBuilding = false;
    
    SetTargetFPS(6000);
    
    struct Building {
        char name[20];
        int price;
        int value;
        int quantity;
    };
    
        struct Building none, clicker, grandma, bakery, factory, maxpower;
        
            strcpy( none.name, "none");
            none.price = 0;
            none.value = 0;
            none.quantity = 0;
    
            strcpy( clicker.name, "Clicker" );
            clicker.price = 10;
            clicker.value = 1;
            clicker.quantity = 0;
    
            strcpy( grandma.name, "Grandma" );
            grandma.price = 100;
            grandma.value = 5;
            grandma.quantity = 0;
    
            strcpy( bakery.name, "Bakery" );
            bakery.price = 500;
            bakery.value = 25;
            bakery.quantity = 0;
    
            strcpy( factory.name, "Factory");
            factory.price = 1000;
            factory.value = 100;
            factory.quantity = 0;
            
            strcpy( maxpower.name, "MAX POWER");
            maxpower.price = 2000;
            maxpower.value = 1000000;
            maxpower.quantity = 0;
            
        while (!WindowShouldClose())
        {
            mousePoint = GetMousePosition();
            
            for (int i = 0; i < NUM_BUILDINGS; i++)
            {
                if (CheckCollisionPointRec(mousePoint, toggleRecs[i]))
                {
                    mouseHoverRec = i;
                    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
                    {
                        currentBuilding = i;
                        cookieBuilding = true;
                    } break;
                } else mouseHoverRec = -1;
            }
            
            if (cookieBuilding && currentBuilding == CLICKER && gameState.cookies >= clicker.price)
            {
                switch (currentBuilding)
                {
                    case CLICKER: pthread_mutex_lock(&mutex_clickers); gameState.clickers++; gameState.cookies -= clicker.price; pthread_mutex_unlock(&mutex_clickers); break;
                    default: break;
                }
                cookieBuilding = false;
            } else if (cookieBuilding && currentBuilding == GRANDMA && gameState.cookies >= grandma.price)
            {
                switch (currentBuilding)
                {
                    case GRANDMA: pthread_mutex_lock(&mutex_grandmas); gameState.grandmas++; gameState.cookies -= grandma.price; pthread_mutex_unlock(&mutex_grandmas); break;
                    default: break;
                }
                cookieBuilding = false;
            } else if (cookieBuilding && currentBuilding == BAKERY && gameState.cookies >= bakery.price)
            {
                switch (currentBuilding)
                {
                    case BAKERY: pthread_mutex_lock(&mutex_bakeries); gameState.bakeries++; gameState.cookies -= bakery.price; pthread_mutex_unlock(&mutex_bakeries); break;
                    default: break;
                }
                cookieBuilding = false;
            } else if (cookieBuilding && currentBuilding == FACTORY && gameState.cookies >= factory.price)
            {
                switch (currentBuilding)
                {
                    case FACTORY: pthread_mutex_lock(&mutex_factories); gameState.factories++; gameState.cookies -= factory.price; pthread_mutex_unlock(&mutex_factories); break;
                    default: break;
                }
                cookieBuilding = false;
            } else if (cookieBuilding && currentBuilding == MAXPOWER && gameState.cookies >= maxpower.price)
            {
                switch (currentBuilding)
                {
                    case MAXPOWER: pthread_mutex_lock(&mutex_max_power); gameState.max_power++; gameState.cookies -= maxpower.price; pthread_mutex_unlock(&mutex_max_power); break;
                    default: break;
                }
                cookieBuilding = false;
            }
            
        BeginDrawing();
            ClearBackground(WHITE);
            
            for (int i = 0; i < NUM_BUILDINGS; i++)
            {
                DrawRectangleRec(toggleRecs[i], ((i == currentBuilding) || (i == mouseHoverRec)) ? SKYBLUE : LIGHTGRAY);
                DrawRectangleLines((int)toggleRecs[i].x, (int)toggleRecs[i].y, (int)toggleRecs[i].width, (int)toggleRecs[i].height, ((i == currentBuilding) || (i == mouseHoverRec)) ? BLUE: GRAY);
                DrawText( buildingText[i], (int)(toggleRecs[i].x + toggleRecs[i].width/2 - MeasureText(buildingText[i], 10)/2), (int) toggleRecs[i].y + 11, 10, ((i == currentBuilding) || (i == mouseHoverRec)) ? DARKBLUE: DARKGRAY);
            }
            
            DrawTextureRec(texture, sourceRec, (Vector2){ theCookie.x, theCookie.y }, WHITE);
            
            if (CheckCollisionPointRec(mousePoint, theCookie))
            {
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                    btnState = 2;
                    btnAction = false;
                } else {
                    btnAction = false;
                    btnState = 1;
                }
                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
                    btnAction = true;
                } 
            } else {
                    btnAction = false;
                    btnState = 0;
                }
                if (btnAction){
                    gameState.cookies++;
                }
                
                sourceRec.y = btnState*frameHeight;
                
                int a = gameState.clickers * gameState.clickers_value;
                int b = gameState.grandmas * gameState.grandmas_value;
                int c = gameState.bakeries * gameState.bakeries_value;
                int d = gameState.factories * gameState.factories_value;
                int e = gameState.max_power * gameState.max_power_value;
                int result = a + b + c + d + e;
                double cookiesPerSec = (double)result / 60;
                DrawText(TextFormat("Cookies: %i", gameState.cookies), screenWidth/2 - 60, 20, 20, BLACK);
                DrawText(TextFormat("CPS: %.2f", cookiesPerSec), screenWidth/2 - 30, 700, 20, BLACK);
                DrawText(TextFormat("Clickers: %i", gameState.clickers), 30, 550, 20, BLACK);              
                DrawText(TextFormat("Grandmas: %i", gameState.grandmas), 30, 600, 20, BLACK);  
                DrawText(TextFormat("Bakeries: %i", gameState.bakeries), 30, 650, 20, BLACK);  
                DrawText(TextFormat("Factories: %i", gameState.factories), 30, 700, 20, BLACK);  
                DrawText(TextFormat("MAX POWER: %i", gameState.max_power), 30, 750, 20, BLACK); 
                
                DrawText(TextFormat("Clickers give 1 CPM"), 30, 100, 20, BLACK);  
                DrawText(TextFormat("Grandmas give 5 CPM"), 30, 125, 20, BLACK);  
                DrawText(TextFormat("Bakeries give 25 CPM"), 30, 150, 20, BLACK);  
                DrawText(TextFormat("Factories give 100 CPM"), 30, 175, 20, BLACK);  
                DrawText(TextFormat("MAX POWER give 10000000 CPM"), 30, 200, 20, BLACK); 
                
                DrawFPS(10, 10);
                EndDrawing();
            }
            UnloadTexture(texture);
            CloseWindow();
            return 0;
        }