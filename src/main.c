#include <timer.h>
#include <input.h>
#include <gpu.h>

#include <mes.h>
#include <gpu.h>
#include <rng.h>
#include <stdint.h>
#include <stdbool.h>
#include <timer.h>
#include "graphics.h"
#include "background.m3if.asset"
#include "warning.m3if.asset"
#include "flash.m3if.asset"
#include "blaster0.m3if.asset"
#include "blaster1.m3if.asset"
#include "blaster2.m3if.asset"
#include "blaster3.m3if.asset"
#include "blaster4.m3if.asset"
//#include "/home/vandi/code/menga/Memory_Manager/.vmes/src/vmes.c"

#define FPS 60
#define FRAMETIME ((1.0/FPS)*1000)


#define AIR 0
#define UNFLIPPED 1
#define FLIPPED 2
#define BLASTER 3
#define ERR_BLOCK 4

#define ROWS 7
#define COLUMNS 6

#define GRAVITY 8
#define GLIDE_SPEED 16
#define JUMP_HEIGHT 1800
#define STOMP_SPEED 300
#define HORIZONTAL_SPEED 60
#define NEW_BIT_DELAY 2000
#define WARNING_DURATION 1000
#define BLASTER_CHANCE 1
#define FLASH_DURATION 50
#define DESTROIED_BIT_ANIMATION_DURATION 500
#define LASER_SHOT_DURATION 750
#define LASER_SHOT_DELAY 6000

#define CONTROLLER 3

#define BLACK 0b000
#define WHITE 0b001
#define YELLOW 0b010
#define RED 0b011
#define DARK_RED 0b100
#define GREEN 0b101
#define BLUE 0b110
#define GRAY 0b111

#define MAX(a, b) (a > b ? a : b)

// palette
uint16_t palette[8] = {COLOR_TO_GPIO(0b000, 0b000, 0b000), \
                       COLOR_TO_GPIO(0b111, 0b111, 0b111), \
                       COLOR_TO_GPIO(0b111, 0b111, 0b000), \
                       COLOR_TO_GPIO(0b111, 0b000, 0b000), \
                       COLOR_TO_GPIO(0b110, 0b001, 0b001), \
                       COLOR_TO_GPIO(0b000, 0b111, 0b000), \
                       COLOR_TO_GPIO(0b000, 0000, 0b111), \
                       COLOR_TO_GPIO(0b011, 0b011, 0b011)};

struct point {
    int32_t x;
    int32_t y;
};

struct fpoint {
    double x;
    double y;
};

struct line {
    int32_t x;
    int32_t y;
    int32_t a;
    int32_t b;
};


struct line vertical_mesh[7];
int32_t vertical_mesh_count = 0;

struct line horizontal_mesh[7];
int32_t horizontal_mesh_count = 0;

struct player {
    int32_t x;
    int32_t y;
    int32_t velx;
    int32_t vely;
    bool last_up_keystate;
    bool last_down_keystate;
    bool in_air;
    bool gliding;
    bool stomping;
};

struct bit_descturction_animation {
    uint32_t animation_start;
    int32_t row;
    int32_t column;
};

struct laser_shot {
    uint32_t last_shot;
    int32_t row;
    int32_t column;
    int32_t texture;
    bool active;
    int32_t right_expansion;
    int32_t left_expansion;
    bool destroyed;
};

bool warning = false;
bool flash = false;
uint32_t last_warning_start = 0;
uint32_t last_flash_start = 0;
uint32_t interest_column = 0;
int32_t cycles = 0;

struct bit_descturction_animation bit_descturction_animations[ROWS * COLUMNS + 1];
int32_t bit_destruction_animation_array_pointer = 0;

struct laser_shot laser_shots[ROWS * COLUMNS * 8 + 1];
int32_t laser_shots_poiter = 0;

int32_t bit_blocks[ROWS][COLUMNS] = {
        {AIR, AIR, AIR, AIR, AIR, AIR},
        {AIR, AIR, AIR, AIR, AIR, AIR},
        {AIR, AIR, AIR, AIR, AIR, AIR},
        {AIR, AIR, AIR, AIR, AIR, AIR},
        {AIR, AIR, AIR, AIR, AIR, AIR},
        {AIR, AIR, AIR, AIR, AIR, AIR},
        {AIR, AIR, AIR, AIR, AIR, AIR},
};
uint32_t delta;

Surface warning_texture;
Surface flash_texture;
Surface background_texture;
Surface blaster0_texture;
Surface blaster1_texture;
Surface blaster2_texture;
Surface blaster3_texture;
Surface blaster4_texture;
struct player p1;
Surface *box;

void handle_input() {

    if (input_get_button(CONTROLLER, BUTTON_DOWN) && p1.in_air && !p1.last_down_keystate) {
        p1.stomping = true;
    }
    if (input_get_button(CONTROLLER, BUTTON_LEFT) && !p1.stomping) {
        p1.velx += (int32_t) (-HORIZONTAL_SPEED * MAX(FRAMETIME, delta));
    }
    if (input_get_button(CONTROLLER, BUTTON_RIGHT) && !p1.stomping) {
        p1.velx += (int32_t) (HORIZONTAL_SPEED * MAX(FRAMETIME, delta));;
    }

//        if ((! p1.in_air or  p1.gliding) &&event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP) {
//             p1.vely = -1.3;
//        }
    if (input_get_button(CONTROLLER, BUTTON_UP) && (!p1.in_air || p1.gliding) && !p1.last_up_keystate) {
        p1.vely = -JUMP_HEIGHT;
//        p1.y -= 0.05;
    }
    p1.last_up_keystate = input_get_button(CONTROLLER, BUTTON_UP);
    p1.last_down_keystate = input_get_button(CONTROLLER, BUTTON_DOWN);
}

int32_t column_air_amount(uint32_t column) {
    for (int32_t i = 0; i < ROWS; ++i) {
        if (bit_blocks[i][column] != AIR) {
            return i;
        }
    }
    return ROWS;
}

void spawn_warning() {
    cycles++;
    last_warning_start = timer_get_ms();
    interest_column = rng_u32() % 6;
    warning = true;
    flash = false;
}

void spawn_flash() {
    last_flash_start = timer_get_ms();
    warning = false;
    flash = true;
}

void end_flash() {
    warning = false;
    flash = false;
}


bool check_for_limitreach(int32_t column) {
    return bit_blocks[0][column] != AIR;
}

void add_bit_descturction_animation(struct bit_descturction_animation ani) {
    bit_descturction_animations[bit_destruction_animation_array_pointer] = ani;
    bit_destruction_animation_array_pointer++;
    if (bit_destruction_animation_array_pointer >= ROWS * COLUMNS)
        bit_destruction_animation_array_pointer = 0;
}

void handle_new_laser(int32_t row, uint32_t column) {
    struct laser_shot shot;
    shot.row = row;
    shot.column = column;
    shot.last_shot = LASER_SHOT_DELAY + LASER_SHOT_DURATION;
    shot.destroyed = false;
    laser_shots[laser_shots_poiter] = shot;
    laser_shots_poiter++;
    if (laser_shots_poiter >= ROWS * COLUMNS * 8) {
        laser_shots_poiter = 0;
    }
}

void send_error_block(int32_t column) {
    bit_blocks[0][column] = AIR;
    for (int32_t i = 1; i < ROWS - 1; ++i) {
        struct bit_descturction_animation ani;
        ani.animation_start = timer_get_ms();
        ani.column = column;
        ani.row = i;
        if (bit_blocks[i + 1][column] == ERR_BLOCK) {
            bit_blocks[i][column] = ERR_BLOCK;
            return;
        }
        add_bit_descturction_animation(ani);
        bit_blocks[i][column] = AIR;
    }
    bit_blocks[6][column] = ERR_BLOCK;
}

void spawn_block() {
//    double rand = dis(gen);
    int32_t new_block_type = 0;
    if (rng_u32() % (int) (1 / BLASTER_CHANCE) == 0) { new_block_type = BLASTER; }
    else {
//        rand = dis(gen);
        new_block_type = UNFLIPPED;
    }

    for (int32_t i = 1; i < ROWS; ++i) {
        if (bit_blocks[i][interest_column] != AIR) {
            bit_blocks[i - 1][interest_column] = new_block_type;
            if (new_block_type == BLASTER) { handle_new_laser(i - 1, interest_column); }
            return;
        }
    }
    bit_blocks[6][interest_column] = new_block_type;
    if (new_block_type == BLASTER) { handle_new_laser(6, interest_column); }
}

void update_laser_shots() {

    for (int i = 0; i < ROWS * COLUMNS * 8; i++) {
        if (laser_shots[i].destroyed) {
            continue;
        }
        if (bit_blocks[laser_shots[i].row][laser_shots[i].column] != BLASTER) {
            laser_shots[i].destroyed = true;
            return;
        }
        if (timer_get_ms() - laser_shots[i].last_shot > LASER_SHOT_DELAY) {
            laser_shots[i].active = true;
            laser_shots[i].texture = 4;
        } else {
            laser_shots[i].active = false;
            laser_shots[i].texture = 0;
            if (timer_get_ms() - laser_shots[i].last_shot > LASER_SHOT_DELAY * 0.55) {
                laser_shots[i].texture = 1;
            }
            if (timer_get_ms() - laser_shots[i].last_shot > LASER_SHOT_DELAY * 0.7) {
                laser_shots[i].texture = 2;
            }
            if (timer_get_ms() - laser_shots[i].last_shot > LASER_SHOT_DELAY * 0.85) {
                laser_shots[i].texture = 3;
            }
        }
        if (timer_get_ms() - laser_shots[i].last_shot > LASER_SHOT_DELAY + LASER_SHOT_DURATION) {
            laser_shots[i].active = false;
            laser_shots[i].last_shot = timer_get_ms();
            laser_shots[i].texture = 4;
        }
        int right = 0;
        int left = 0;
        for (int j = laser_shots[i].column + 1; j < COLUMNS; ++j) {
            if (bit_blocks[laser_shots[i].row][j] == AIR) {
                right++;
            } else break;
        }
        for (int j = laser_shots[i].column - 1; j >= 0; --j) {
            if (bit_blocks[laser_shots[i].row][j] == AIR) {
                left++;
            } else break;
        }
        laser_shots[i].right_expansion = right;
        laser_shots[i].left_expansion = left;
    }
}

void handle_box_collision() {
    p1.gliding = false;
    if (p1.velx > 0) {
        if (p1.x + p1.velx + 7 * 1000 > 119 * 1000) {
            p1.velx = 0;
            p1.x = 119 * 1000 - 7 * 1000;
            p1.gliding = true;
        }
    } else if (p1.velx < 0) {
        if (p1.x + p1.velx < 43 * 1000) {
            p1.velx = 0;
            p1.x = 42 * 1000 + 2;
            p1.gliding = true;
        }
    }
    if (p1.vely >= 0 && p1.y >= 110000) {
        p1.vely = 0;
        p1.y = 119 * 1000 - 9 * 1000;
        p1.in_air = false;
    } else {
        p1.in_air = true;
    }
    if (p1.vely <= 0 && p1.y <= 32 * 1000) {
        p1.vely = 0;
    }
}

void correct_player_stomp_trajectory() {
//    if (p1.x / 1000 * 2 % 2) // TODO figure this outt
    int32_t x = ((int) round(((float) p1.x / 1000) + 4.5) - 42) % 13;
//    std::cout << x << std::endl;
    if (x > 13 - 2.5) {
        p1.velx = (int32_t) (-STOMP_SPEED / 2 * MAX(FRAMETIME, delta));
        p1.vely = (int32_t) (STOMP_SPEED / 20 * MAX(FRAMETIME, delta));
    } else if (x < 3.5) {
        p1.velx = (int32_t) (STOMP_SPEED / 2 * MAX(FRAMETIME, delta));
        p1.vely = (int32_t) (STOMP_SPEED / 20 * MAX(FRAMETIME, delta));
    }
}


bool isPointInsideRectangle(int32_t px, int32_t py, int32_t x, int32_t y, int32_t h, int32_t w) {
    if (px >= x && px <= x + w &&
        py >= y && py <= y + h) {
        return true;
    }
    return false;
}

void handle_stomp_obstacle() {
    if (p1.vely == 0) {
        return;
    }
    for (int32_t i = 0; i < ROWS; ++i) {
        for (int32_t j = 0; j < COLUMNS; ++j) {
            int32_t x = 42000 + (j * 13000);
            int32_t y = 35000 + (i * 12000);
            int32_t w = 12000;
            int32_t h = 12000;

            struct point p;
            p.y = p1.y + 9000 + p1.vely;
            p.x = p1.x + 3500;
            if (isPointInsideRectangle(p.x, p.y, x, y, h, w)) {
                if (bit_blocks[i][j] == UNFLIPPED) {
                    bit_blocks[i][j] = FLIPPED;
                    p1.vely = -JUMP_HEIGHT;
                    p1.in_air = true;
                    p1.stomping = false;
                    return;
                } else if (bit_blocks[i][j] == FLIPPED || bit_blocks[i][j] == BLASTER) {
                    bit_blocks[i][j] = AIR;
                    struct bit_descturction_animation ani;
                    ani.animation_start = timer_get_ms();
                    ani.column = j;
                    ani.row = i;
                    add_bit_descturction_animation(ani);

                } else if (bit_blocks[i][j] == ERR_BLOCK) {
                    p1.vely = -JUMP_HEIGHT;
                    p1.in_air = true;
                    p1.stomping = false;
                    return;
                }
            }
        }
    }

}

void flip_blocks() {
    if (p1.velx == 0) {
        return;
    }
    for (int32_t i = 0; i < ROWS; ++i) {
        for (int32_t j = 0; j < COLUMNS; ++j) {
            if (bit_blocks[i][j] != UNFLIPPED) {
                continue;
            }

            int32_t x = 42000 + (j * 13000);
            int32_t y = 35000 + (i * 12000);
            int32_t w = 12000;
            int32_t h = 12000;

            struct point p;
            p.y = p1.y + 4500;
            p.x = p1.x + p1.velx;
            if (p1.velx > 0)
                p.x += 7000;
            if (isPointInsideRectangle(p.x, p.y, x, y, h, w)) {
                bit_blocks[i][j] = FLIPPED;
            }
        }
    }
}

int32_t column_height(int32_t column) {
    int32_t counter = 0;
    for (int32_t i = ROWS - 1; i >= 0; --i) {
        if (bit_blocks[i][column] == AIR) {
            return counter;
        }
        counter++;
    }
    return ROWS;
}

bool linesIntersect(struct line a, struct line b) {
    int32_t x1 = a.x;
    int32_t y1 = a.y;
    int32_t x2 = a.a;
    int32_t y2 = a.b;
    int32_t x3 = b.x;
    int32_t y3 = b.y;
    int32_t x4 = b.a;
    int32_t y4 = b.b;

    double den = (y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1);
    double ua = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / den;
    double ub = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / den;

    if (ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1) {
        return true;
    }
    return false;
}


void handle_block_collision() {

//    struct line vertical_mesh[6];
    vertical_mesh_count = 0;

//    struct line horizontal_mesh[5];
    horizontal_mesh_count = 0;


    for (int32_t i = 0; i < COLUMNS - 1; ++i) {
        int32_t left_height = column_height(i);
        int32_t right_height = column_height(i + 1);

        if (left_height < right_height) {
            struct line new_line;
            new_line.x = (41 + ((i + 1) * 13) + 1) * 1000 - 3500;
            new_line.a = new_line.x;

            new_line.y = 120000;
            new_line.b = ((119 - (right_height * 12)) * 1000) - 4500 + 1;
            vertical_mesh[vertical_mesh_count] = new_line;
            vertical_mesh_count++;
        } else if (left_height > right_height) {
            struct line new_line;
            new_line.x = (41 + (i + 1) * 13) * 1000 + 3500;
            new_line.a = new_line.x;

            new_line.y = ((119 - (left_height * 12)) * 1000) - 4500 + 1;
            new_line.b = 120000;
            vertical_mesh[vertical_mesh_count] = new_line;
            vertical_mesh_count++;
        }

    }

    for (int32_t i = 0; i < COLUMNS; ++i) {
        int32_t left_height = column_height(i - 1);
        int32_t current_height = column_height(i);
        int32_t right_height = column_height(i + 1);
        if (current_height == 0) {
            continue;
        }
        if (i != 0 && horizontal_mesh_count != 0 && current_height == left_height) {
            horizontal_mesh[horizontal_mesh_count - 1].a += 13000;
        } else {
            struct line new_line;
            new_line.x = (42 + (i * 13)) * 1000 - 3500;
            new_line.a = new_line.x + 12000 + 7000;

            new_line.y = (119 - (current_height * 12)) * 1000 - 4500;
            new_line.b = new_line.y;
            horizontal_mesh[horizontal_mesh_count] = new_line;
            horizontal_mesh_count++;
        }
//        if ((i != COLUMNS - 1 && right_height > current_height)) {
//            horizontal_mesh[horizontal_mesh_count].a += 3000;
//        }
//        if (i == COLUMNS - 1 && horizontal_mesh_count != 0) {
//            horizontal_mesh[horizontal_mesh_count].a += 2000;
//        }
//
//        if ((i != 0 && left_height > current_height)) {
//            horizontal_mesh[horizontal_mesh_count].x -= 3000;
//        }
//        if (i == 0 && horizontal_mesh_count != 0) {
//            horizontal_mesh[horizontal_mesh_count].x -= 2000;
//        }
    }

    struct point center;
    center.x = p1.x + 3500;
    center.y = p1.y + 4500;

    struct point center_vel;
    center_vel.x = center.x + p1.velx;
    center_vel.y = center.y + p1.vely;

    struct line center_line;
    center_line.x = center.x;
    center_line.y = center.y;
    center_line.a = center_vel.x;
    center_line.b = center_vel.y;
    //redner mesh

    if (p1.vely >= -40) {

        for (int32_t i = 0; i < horizontal_mesh_count; i++) {
//            printf("1, %d\n", i);
            bool g = linesIntersect(horizontal_mesh[i], center_line);
//            printf("2, %d\n", i);
            if (g) {
                p1.vely = 0;
                p1.y = horizontal_mesh[i].b - 4500;
                p1.in_air = false;
            }
        }
    }
    if (p1.vely == 0) {
        struct point gravity_center_vel;
        gravity_center_vel.x = center.x + p1.velx;
        gravity_center_vel.y = center.y + (int32_t) (GRAVITY * MAX(FRAMETIME, delta));
        struct line gravity_center_line;
        gravity_center_line.x = center.x;
        gravity_center_line.y = center.y;
        gravity_center_line.a = gravity_center_vel.x;
        gravity_center_line.b = gravity_center_vel.y;
        for (int32_t i = 0; i < horizontal_mesh_count; i++) {
            if (linesIntersect(horizontal_mesh[i], gravity_center_line)) {
                p1.in_air = false;
            }
        }
    }

    for (int32_t i = 0; i < vertical_mesh_count; i++) {
        if (linesIntersect(vertical_mesh[i], center_line)) {
            p1.velx = 0;
            p1.gliding = true;
        }
    }
}

void handle_player() {

    p1.velx = 0;
    if (p1.in_air) {
        p1.vely += (int32_t) (GRAVITY * MAX(FRAMETIME, delta));;
    } else {
        p1.vely = 0;
    }
    handle_input();
    if (p1.stomping) {
        p1.vely = (int32_t) (STOMP_SPEED * MAX(FRAMETIME, delta));
        handle_box_collision();
        if (p1.vely == 0) {

            p1.vely = -JUMP_HEIGHT;
            p1.in_air = true;
            p1.stomping = false;
            return;
        }
        correct_player_stomp_trajectory();
        handle_stomp_obstacle();

    } else {
        flip_blocks();

        handle_box_collision();
        handle_block_collision();
        if (p1.gliding && p1.vely > (int32_t) (GLIDE_SPEED * MAX(FRAMETIME, delta))) {
            p1.vely = (int32_t) (GLIDE_SPEED * MAX(FRAMETIME, delta));
        }
    }
    p1.x += p1.velx;
    p1.y += p1.vely;
}


void render() {
    surf_fill(box, BLACK);
    // render Background
    surf_draw_masked_surf(box, &background_texture, 0, 0, BLACK);

    for (int32_t bit = 0; bit < ROWS * COLUMNS; bit++) {
        if (timer_get_ms() - bit_descturction_animations[bit].animation_start < DESTROIED_BIT_ANIMATION_DURATION &&
            timer_get_ms() > DESTROIED_BIT_ANIMATION_DURATION) {
            surf_set_pixel(box, 1 + (bit_descturction_animations[bit].column * 13) + 5,
                           1 + (bit_descturction_animations[bit].row * 12) + 5, WHITE);
            surf_set_pixel(box, 1 + (bit_descturction_animations[bit].column * 13) + 5,
                           1 + (bit_descturction_animations[bit].row * 12) + 6, WHITE);
            surf_set_pixel(box, 1 + (bit_descturction_animations[bit].column * 13) + 6,
                           1 + (bit_descturction_animations[bit].row * 12) + 5, WHITE);
            surf_set_pixel(box, 1 + (bit_descturction_animations[bit].column * 13) + 6,
                           1 + (bit_descturction_animations[bit].row * 12) + 6, WHITE);
        }
    }


    if (warning) {
        SDL_Rect destination_rect;
        uint32_t x = 1 + (interest_column * 13);
        int32_t y = 1;
//        int32_t w = 12;
//        int32_t h = 12 * (column_air_amount(interest_column));
        surf_draw_masked_surf(box, &warning_texture, x, y, BLACK);
    } else if (flash) {
        SDL_Rect destination_rect;
        uint32_t x = 1 + (interest_column * 13);
        int32_t y = 1 - (12 * (COLUMNS - column_air_amount(interest_column)));
        int32_t w = 12;
        int32_t h = 12 * (COLUMNS);

//        SDL_Rect source_rect;
//        source_rect.x = 0;
//        source_rect.y = 84 - (12 * (column_air_amount(interest_column)));
//        source_rect.w = 12;
//        source_rect.h = 12 * (column_air_amount(interest_column));
        surf_draw_masked_surf(box, &flash_texture, x, y, BLACK);
    }

    for (int i = 0; i < ROWS * COLUMNS * 8; i++) {
        if (laser_shots[i].active && !laser_shots[i].destroyed) {
            int32_t x = ((laser_shots[i].column - laser_shots[i].left_expansion) * 13) - 1;
            int32_t y = (laser_shots[i].row * 12) + 5;
            int32_t w = ((laser_shots[i].left_expansion + laser_shots[i].right_expansion + 1) * 13) + 1;
            for (int j = x; j < x + w; j++) {
                surf_set_pixel(box, j, y, DARK_RED);
                surf_set_pixel(box, j, y + 1, RED);
                surf_set_pixel(box, j, y + 2, RED);
                surf_set_pixel(box, j, y + 3, DARK_RED);
            }
        }
        if (!laser_shots[i].destroyed) {
            int32_t x = 1 + (laser_shots[i].column * 13);
            int32_t y = 1 + (laser_shots[i].row * 12);
            printf("%d\n", laser_shots[i].texture);
            if (laser_shots[i].texture == 0) {
                surf_draw_masked_surf(box, &blaster0_texture, x, y, BLACK);
            } else if (laser_shots[i].texture == 1) {
                surf_draw_masked_surf(box, &blaster1_texture, x, y, BLACK);
            } else if (laser_shots[i].texture == 2) {
                surf_draw_masked_surf(box, &blaster2_texture, x, y, BLACK);
            } else if (laser_shots[i].texture == 3) {
                surf_draw_masked_surf(box, &blaster3_texture, x, y, BLACK);
            } else if (laser_shots[i].texture == 4) {
                surf_draw_masked_surf(box, &blaster4_texture, x, y, BLACK);
            }
        }
    }


//     render blocks
    for (int32_t i = 0; i < ROWS; ++i) {
        for (int32_t j = 0; j < COLUMNS; ++j) {
            int32_t x = 1 + (j * 13);
            int32_t y = 1 + (i * 12);
            int32_t w = 12;
            int32_t h = 12;

//            std::cout << zero_texture.get_surface_rect(count, 0)->x << ", " << zero_texture.get_surface_rect(count, 0)->y << "\n";
            switch (bit_blocks[i][j]) {
                case UNFLIPPED:
                    surf_draw_filled_rectangle(box, x, y, w, h, RED);
                    break;
                case FLIPPED:
                    surf_draw_filled_rectangle(box, x, y, w, h, YELLOW);
                    break;
                case ERR_BLOCK:
                    surf_draw_filled_rectangle(box, x, y, w, h, GREEN);
                    break;
            }
        }
    }

    surf_draw_rectangle(box, (p1.x - 1) / 1000 - 41, (p1.y - 1) / 1000 - 34, 7, 9, WHITE);
//    printf("%d, %d\n", p1.x, p1.y);


//    for (int32_t i = 0; i < horizontal_mesh_count; i++) {
//        surf_draw_line(box, horizontal_mesh[i].x / 1000 - 41, horizontal_mesh[i].y / 1000 - 34,
//                       horizontal_mesh[i].a / 1000 - 41,
//                       horizontal_mesh[i].b / 1000 - 34, BLUE);
//    }
//    for (int32_t i = 0; i < vertical_mesh_count; i++) {
//        surf_draw_line(box, vertical_mesh[i].x / 1000 - 41, vertical_mesh[i].y / 1000 - 34,
//                       vertical_mesh[i].a / 1000 - 41, vertical_mesh[i].b / 1000 - 34, BLUE);
//    }

    gpu_send_buf(BACK_BUFFER, box->w, box->h, 41, 34, box->d);
    gpu_swap_buf();

//    SDL_SetRenderDrawColor(*renderer_pointer, 0, 255, 0, 255);
//    for (int32_t i = 0; i < vertical_mesh_count; i++) {
//        SDL_RenderDrawLine(*renderer_pointer, vertical_mesh[i].x * 9, vertical_mesh[i].y * 9, vertical_mesh[i].a * 9, vertical_mesh[i].b * 9);
//    }
//    SDL_SetRenderDrawColor(*renderer_pointer, 0, 255, 0, 255);
//    for (int32_t i = 0; i < horizontal_mesh_count; i++) {
//        SDL_RenderDrawLine(*renderer_pointer, horizontal_mesh[i].x * 9, horizontal_mesh[i].y * 9, horizontal_mesh[i].a * 9, horizontal_mesh[i].b * 9);
//    }
}

uint8_t start(void) {
    Surface temp = surf_create(1 + (COLUMNS * 13), 2 + (ROWS * 12));
    box = &temp;
    warning_texture = surf_create_from_memory(12, 84, ASSET_WARNING_M3IF);
    flash_texture = surf_create_from_memory(12, 84, ASSET_FLASH_M3IF);
    background_texture = surf_create_from_memory(1 + (COLUMNS * 13), 2 + (ROWS * 12), ASSET_BACKGROUND_M3IF);
    blaster0_texture = surf_create_from_memory(12, 12, ASSET_BLASTER0_M3IF);
    blaster1_texture = surf_create_from_memory(12, 12, ASSET_BLASTER1_M3IF);
    blaster2_texture = surf_create_from_memory(12, 12, ASSET_BLASTER2_M3IF);
    blaster3_texture = surf_create_from_memory(12, 12, ASSET_BLASTER3_M3IF);
    blaster4_texture = surf_create_from_memory(12, 12, ASSET_BLASTER4_M3IF);
    rng_init();
    gpu_update_palette(palette);
    p1.x = 96000;
    p1.y = 79000;
    p1.velx = 0;
    p1.vely = -100;

    uint32_t stop;
    uint32_t start;


    for (int i = 0; i < ROWS * COLUMNS * 8; i++) {
        struct laser_shot shot;
        shot.row = 0;
        shot.column = 0;
        shot.last_shot = LASER_SHOT_DELAY + LASER_SHOT_DURATION;
        shot.destroyed = true;
        laser_shots[i] = shot;
    }

    gpu_blank(FRONT_BUFFER, 0);
    gpu_blank(BACK_BUFFER, 0);

    while (1) {
        if (timer_get_ms() > NEW_BIT_DELAY * cycles && !warning && !flash) {
            spawn_warning();
        } else if (warning && timer_get_ms() - last_warning_start > WARNING_DURATION) {
            spawn_flash();
            spawn_block();
        } else if (flash && timer_get_ms() - last_flash_start > FLASH_DURATION) {
            end_flash();
        }
        update_laser_shots();
        for (int32_t i = 0; i < COLUMNS; ++i) {
            if (check_for_limitreach(i)) {
                send_error_block(i);
            }
        }
        handle_player();
        render();

        // timing
        stop = timer_get_ms();
        delta = stop - start;
//        printf("%d\n", delta);
        if (delta < FRAMETIME) {
            timer_block_ms(FRAMETIME - delta);
        }
        start = timer_get_ms();
    }
    return CODE_RESTART;
}
