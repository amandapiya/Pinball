#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>
#include "sdl_wrapper.h"
#include "swinger.h"
#include "key_handler_aux.h"
#include "body_aux.h"

#define WINDOW_TITLE "CS 3"
#define WINDOW_WIDTH 1300
#define WINDOW_HEIGHT 650
#define MS_PER_S 1e3

/**
 * The coordinate at the center of the screen.
 */
vector_t center;
/**
 * The coordinate difference from the center to the top right corner.
 */
vector_t max_diff;
/**
 * The SDL window where the scene is rendered.
 */
SDL_Window *window;
/**
 * The renderer used to draw the scene.
 */
SDL_Renderer *renderer;
/**
 * The keypress handler, or NULL if none has been configured.
 */
key_handler_t key_handler = NULL;
/**
 * SDL's timestamp when a key was last pressed or released.
 * Used to mesasure how long a key has been held.
 */
uint32_t key_start_timestamp;
/**
 * The value of clock() when time_since_last_tick() was last called.
 * Initially 0.
 */
clock_t last_clock = 0;

/**
 * Global variable to store the scene
 */
scene_t *curr_scene;
swinger_t *curr_swing1;
swinger_t *curr_swing2;
body_t *curr_spring;

/** Computes the center of the window in pixel coordinates */
vector_t get_window_center(void){
    int *width = malloc(sizeof(*width)), *height = malloc(sizeof(*height));
    assert(width != NULL);
    assert(height != NULL);
    SDL_GetWindowSize(window, width, height);
    vector_t dimensions = {.x = *width, .y = *height};
    free(width);
    free(height);
    return vec_multiply(0.5, dimensions);
}

/**
 * Computes the scaling factor between scene coordinates and pixel coordinates.
 * The scene is scaled by the same factor in the x and y dimensions,
 * chosen to maximize the size of the scene while keeping it in the window.
 */
double get_scene_scale(vector_t window_center){
    // Scale scene so it fits entirely in the window
    double x_scale = window_center.x / max_diff.x, y_scale = window_center.y / max_diff.y;
    return x_scale < y_scale ? x_scale : y_scale;
}

/** Maps a scene coordinate to a window coordinate */
vector_t get_window_position(vector_t scene_pos, vector_t window_center){
    // Scale scene coordinates by the scaling factor
    vector_t scene_center_offset = vec_subtract(scene_pos, center);
    double scale = get_scene_scale(window_center);
    vector_t pixel_center_offset = vec_multiply(scale, scene_center_offset);
    vector_t pixel = {
        .x = round(window_center.x + pixel_center_offset.x),
        // Flip y axis since positive y is down on the screen
        .y = round(window_center.y - pixel_center_offset.y)
    };
    return pixel;
}

/**
 * Converts an SDL key code to a char.
 * 7-bit ASCII characters are just returned
 * and arrow keys are given special character codes.
 */
char get_keycode(SDL_Keycode key){
    switch (key) {
        case SDLK_LEFT:
            return LEFT_ARROW;
        case SDLK_UP:
            return UP_ARROW;
        case SDLK_RIGHT:
            return RIGHT_ARROW;
        case SDLK_DOWN:
            return DOWN_ARROW;
        default:
            // Only process 7-bit ASCII characters
            return key == (SDL_Keycode) (char) key ? key : '\0';
    }
}

void sdl_init(vector_t min, vector_t max){
    // Check parameters
    assert(min.x < max.x);
    assert(min.y < max.y);

    center = vec_multiply(0.5, vec_add(min, max));
    max_diff = vec_subtract(max, center);
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_RESIZABLE
    );
    renderer = SDL_CreateRenderer(window, -1, 0);
}

bool sdl_is_done(void){
    SDL_Event *event = malloc(sizeof(*event));
    assert(event != NULL);
    while (SDL_PollEvent(event)) {
        switch (event->type) {
            case SDL_QUIT:
                TTF_Quit();
                free(event);
                return true;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                // Skip the keypress if no handler is configured
                // or an unrecognized key was pressed
                if (key_handler == NULL) break;
                char key = get_keycode(event->key.keysym.sym);
                if (key == '\0') break;

                uint32_t timestamp = event->key.timestamp;
                if (!event->key.repeat) {
                    key_start_timestamp = timestamp;
                }
                key_event_type_t type = event->type == SDL_KEYDOWN ? KEY_PRESSED : KEY_RELEASED;
                double held_time = (timestamp - key_start_timestamp) / MS_PER_S;
                assert(curr_scene != NULL);
//                body_t *player = scene_get_body(curr_scene, 0);
//                assert(player != NULL);
                key_handler_aux_t *key_aux = key_handler_aux_init(curr_scene, curr_swing1, curr_swing2, curr_spring);
                key_handler(key, type, held_time, key_aux);
                break;
        }
    }
    free(event);
    return false;
}

void sdl_clear(void){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}

void sdl_draw_polygon(list_t *points, rgb_color_t color){
    // Check parameters
    size_t n = list_size(points);
    assert(n >= 3);
    assert(0 <= color.r && color.r <= 1);
    assert(0 <= color.g && color.g <= 1);
    assert(0 <= color.b && color.b <= 1);

    vector_t window_center = get_window_center();

    // Convert each vertex to a point on screen
    int16_t *x_points = malloc(sizeof(*x_points) * n), *y_points = malloc(sizeof(*y_points) * n);
    assert(x_points != NULL);
    assert(y_points != NULL);
    for (size_t i = 0; i < n; i++) {
        vector_t *vertex = list_get(points, i);
        vector_t pixel = get_window_position(*vertex, window_center);
        x_points[i] = pixel.x;
        y_points[i] = pixel.y;
    }

    // Draw polygon with the given color
    filledPolygonRGBA(
        renderer,
        x_points, y_points, n,
        color.r * 255, color.g * 255, color.b * 255, 255
    );
    free(x_points);
    free(y_points);
}

// might need to double check clearing stuff idk yet
void sdl_render_text(vector_t position, int font_size, char* text, rgb_color_t color){
    assert(0 <= color.r && color.r <= 1);
    assert(0 <= color.g && color.g <= 1);
    assert(0 <= color.b && color.b <= 1);
    assert(position.x >= 0 && position.x <= WINDOW_WIDTH);
    assert(position.y >= 0 && position.y <= WINDOW_HEIGHT);

    vector_t window_center = get_window_center();

    TTF_Font* font = TTF_OpenFont("fonts/Minecraft.ttf", font_size);
    SDL_Color textColor = {color.r * 255, color.g * 255, color.b *255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect* message_rect = malloc(sizeof(SDL_Rect));
    position =  get_window_position(position, window_center);
    message_rect->x = position.x;
    message_rect->y = WINDOW_HEIGHT - position.y;
    message_rect->w = surface->w;
    message_rect->h = surface->h;


    SDL_RenderCopy(renderer, texture, NULL, message_rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
    
    TTF_CloseFont(font);
}

void sdl_render_image(vector_t position){
    assert(position.x >= 0 && position.x <= WINDOW_WIDTH);
    assert(position.y >= 0 && position.y <= WINDOW_HEIGHT);

    vector_t window_center = get_window_center();

    SDL_Surface *image = SDL_LoadBMP("fonts/blank-original.bmp");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);

    position =  get_window_position(position, window_center);
    SDL_Rect dstrect = {position.x, WINDOW_HEIGHT - position.y, image->w / 2, image->h / 2};

    SDL_RenderCopy(renderer, texture, NULL, &dstrect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(image);
}

void sdl_show(void) {
    SDL_RenderPresent(renderer);
}

void sdl_render_scene(scene_t *scene, list_t *swlist){
//    sdl_clear();
    size_t body_count = scene_bodies(scene);
    for (size_t i = 0; i < body_count; i++) {
        body_t *body = scene_get_body(scene, i);
        list_t *shape = body_get_shape(body);
        sdl_draw_polygon(shape, body_get_color(body));
        list_free(shape);
    }
    
    for (size_t j = 0; j < list_size(swlist); j++){
        swinger_t *s = list_get(swlist, j);
        list_t *shape = swinger_get_shape(s);
        sdl_draw_polygon(shape, swinger_get_color(s));
        list_free(shape);
    }
    curr_scene = scene;
    curr_swing1 = list_get(swlist, 0);
    curr_swing2 = list_get(swlist, 1);
    
    for (size_t i = 0; i < scene_bodies(scene); i++){
        body_t *b = scene_get_body(scene, i);
        if(((body_aux_t*) body_get_info(b))->is_spring) {
            curr_spring = b;
        }
    }

    sdl_show();
}

void sdl_on_key(key_handler_t handler){
    key_handler = handler;
}

double time_since_last_tick(void){
    clock_t now = clock();
    double difference = last_clock
        ? (double) (now - last_clock) / CLOCKS_PER_SEC
        : 0.0; // return 0 the first time this is called
    last_clock = now;
    return difference;
}
