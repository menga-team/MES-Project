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
#define JUMP_HEIGHT 1800
#define STOMP_SPEED 300
#define HORIZONTAL_SPEED 60
#define NEW_BIT_DELAY 2000
#define WARNING_DURATION 1000
#define BLASTER_CHANCE 0.2
#define FLASH_DURATION 50
#define DESTROIED_BIT_ANIMATION_DURATION 500
#define LASER_SHOT_DURATION 750
#define LASER_SHOT_DELAY 5000

#define CONTROLLER 3

#define BACKGROUND_GREY_PIXEL {{0, 0},{1, 0},{2, 0},{3, 0},{4, 0},{5, 0},{6, 0},{7, 0},{8, 0},{9, 0},{10, 0},{11, 0},{12, 0},{13, 0},{14, 0},{15, 0},{16, 0},{17, 0},{18, 0},{19, 0},{20, 0},{21, 0},{22, 0},{23, 0},{24, 0},{25, 0},{26, 0},{27, 0},{28, 0},{29, 0},{30, 0},{31, 0},{32, 0},{33, 0},{34, 0},{35, 0},{36, 0},{37, 0},{38, 0},{39, 0},{40, 0},{41, 0},{42, 0},{43, 0},{44, 0},{45, 0},{46, 0},{47, 0},{48, 0},{49, 0},{50, 0},{51, 0},{52, 0},{53, 0},{54, 0},{55, 0},{56, 0},{57, 0},{58, 0},{59, 0},{60, 0},{61, 0},{62, 0},{63, 0},{64, 0},{65, 0},{66, 0},{67, 0},{68, 0},{69, 0},{70, 0},{71, 0},{72, 0},{73, 0},{74, 0},{75, 0},{76, 0},{77, 0},{78, 0},{0, 1},{13, 1},{26, 1},{39, 1},{52, 1},{65, 1},{78, 1},{0, 2},{13, 2},{26, 2},{39, 2},{52, 2},{65, 2},{78, 2},{0, 3},{13, 3},{26, 3},{39, 3},{52, 3},{65, 3},{78, 3},{0, 4},{13, 4},{26, 4},{39, 4},{52, 4},{65, 4},{78, 4},{0, 5},{13, 5},{26, 5},{39, 5},{52, 5},{65, 5},{78, 5},{0, 6},{13, 6},{26, 6},{39, 6},{52, 6},{65, 6},{78, 6},{0, 7},{13, 7},{26, 7},{39, 7},{52, 7},{65, 7},{78, 7},{0, 8},{13, 8},{26, 8},{39, 8},{52, 8},{65, 8},{78, 8},{0, 9},{13, 9},{26, 9},{39, 9},{52, 9},{65, 9},{78, 9},{0, 10},{13, 10},{26, 10},{39, 10},{52, 10},{65, 10},{78, 10},{0, 11},{13, 11},{26, 11},{39, 11},{52, 11},{65, 11},{78, 11},{0, 12},{13, 12},{26, 12},{39, 12},{52, 12},{65, 12},{78, 12},{0, 13},{13, 13},{26, 13},{39, 13},{52, 13},{65, 13},{78, 13},{0, 14},{13, 14},{26, 14},{39, 14},{52, 14},{65, 14},{78, 14},{0, 15},{13, 15},{26, 15},{39, 15},{52, 15},{65, 15},{78, 15},{0, 16},{13, 16},{26, 16},{39, 16},{52, 16},{65, 16},{78, 16},{0, 17},{13, 17},{26, 17},{39, 17},{52, 17},{65, 17},{78, 17},{0, 18},{13, 18},{26, 18},{39, 18},{52, 18},{65, 18},{78, 18},{0, 19},{13, 19},{26, 19},{39, 19},{52, 19},{65, 19},{78, 19},{0, 20},{13, 20},{26, 20},{39, 20},{52, 20},{65, 20},{78, 20},{0, 21},{13, 21},{26, 21},{39, 21},{52, 21},{65, 21},{78, 21},{0, 22},{13, 22},{26, 22},{39, 22},{52, 22},{65, 22},{78, 22},{0, 23},{13, 23},{26, 23},{39, 23},{52, 23},{65, 23},{78, 23},{0, 24},{13, 24},{26, 24},{39, 24},{52, 24},{65, 24},{78, 24},{0, 25},{13, 25},{26, 25},{39, 25},{52, 25},{65, 25},{78, 25},{0, 26},{13, 26},{26, 26},{39, 26},{52, 26},{65, 26},{78, 26},{0, 27},{13, 27},{26, 27},{39, 27},{52, 27},{65, 27},{78, 27},{0, 28},{13, 28},{26, 28},{39, 28},{52, 28},{65, 28},{78, 28},{0, 29},{13, 29},{26, 29},{39, 29},{52, 29},{65, 29},{78, 29},{0, 30},{13, 30},{26, 30},{39, 30},{52, 30},{65, 30},{78, 30},{0, 31},{13, 31},{26, 31},{39, 31},{52, 31},{65, 31},{78, 31},{0, 32},{13, 32},{26, 32},{39, 32},{52, 32},{65, 32},{78, 32},{0, 33},{13, 33},{26, 33},{39, 33},{52, 33},{65, 33},{78, 33},{0, 34},{13, 34},{26, 34},{39, 34},{52, 34},{65, 34},{78, 34},{0, 35},{13, 35},{26, 35},{39, 35},{52, 35},{65, 35},{78, 35},{0, 36},{13, 36},{26, 36},{39, 36},{52, 36},{65, 36},{78, 36},{0, 37},{13, 37},{26, 37},{39, 37},{52, 37},{65, 37},{78, 37},{0, 38},{13, 38},{26, 38},{39, 38},{52, 38},{65, 38},{78, 38},{0, 39},{13, 39},{26, 39},{39, 39},{52, 39},{65, 39},{78, 39},{0, 40},{13, 40},{26, 40},{39, 40},{52, 40},{65, 40},{78, 40},{0, 41},{13, 41},{26, 41},{39, 41},{52, 41},{65, 41},{78, 41},{0, 42},{13, 42},{26, 42},{39, 42},{52, 42},{65, 42},{78, 42},{0, 43},{13, 43},{26, 43},{39, 43},{52, 43},{65, 43},{78, 43},{0, 44},{13, 44},{26, 44},{39, 44},{52, 44},{65, 44},{78, 44},{0, 45},{13, 45},{26, 45},{39, 45},{52, 45},{65, 45},{78, 45},{0, 46},{13, 46},{26, 46},{39, 46},{52, 46},{65, 46},{78, 46},{0, 47},{13, 47},{26, 47},{39, 47},{52, 47},{65, 47},{78, 47},{0, 48},{13, 48},{26, 48},{39, 48},{52, 48},{65, 48},{78, 48},{0, 49},{13, 49},{26, 49},{39, 49},{52, 49},{65, 49},{78, 49},{0, 50},{13, 50},{26, 50},{39, 50},{52, 50},{65, 50},{78, 50},{0, 51},{13, 51},{26, 51},{39, 51},{52, 51},{65, 51},{78, 51},{0, 52},{13, 52},{26, 52},{39, 52},{52, 52},{65, 52},{78, 52},{0, 53},{13, 53},{26, 53},{39, 53},{52, 53},{65, 53},{78, 53},{0, 54},{13, 54},{26, 54},{39, 54},{52, 54},{65, 54},{78, 54},{0, 55},{13, 55},{26, 55},{39, 55},{52, 55},{65, 55},{78, 55},{0, 56},{13, 56},{26, 56},{39, 56},{52, 56},{65, 56},{78, 56},{0, 57},{13, 57},{26, 57},{39, 57},{52, 57},{65, 57},{78, 57},{0, 58},{13, 58},{26, 58},{39, 58},{52, 58},{65, 58},{78, 58},{0, 59},{13, 59},{26, 59},{39, 59},{52, 59},{65, 59},{78, 59},{0, 60},{13, 60},{26, 60},{39, 60},{52, 60},{65, 60},{78, 60},{0, 61},{13, 61},{26, 61},{39, 61},{52, 61},{65, 61},{78, 61},{0, 62},{13, 62},{26, 62},{39, 62},{52, 62},{65, 62},{78, 62},{0, 63},{13, 63},{26, 63},{39, 63},{52, 63},{65, 63},{78, 63},{0, 64},{13, 64},{26, 64},{39, 64},{52, 64},{65, 64},{78, 64},{0, 65},{13, 65},{26, 65},{39, 65},{52, 65},{65, 65},{78, 65},{0, 66},{13, 66},{26, 66},{39, 66},{52, 66},{65, 66},{78, 66},{0, 67},{13, 67},{26, 67},{39, 67},{52, 67},{65, 67},{78, 67},{0, 68},{13, 68},{26, 68},{39, 68},{52, 68},{65, 68},{78, 68},{0, 69},{13, 69},{26, 69},{39, 69},{52, 69},{65, 69},{78, 69},{0, 70},{13, 70},{26, 70},{39, 70},{52, 70},{65, 70},{78, 70},{0, 71},{13, 71},{26, 71},{39, 71},{52, 71},{65, 71},{78, 71},{0, 72},{13, 72},{26, 72},{39, 72},{52, 72},{65, 72},{78, 72},{0, 73},{13, 73},{26, 73},{39, 73},{52, 73},{65, 73},{78, 73},{0, 74},{13, 74},{26, 74},{39, 74},{52, 74},{65, 74},{78, 74},{0, 75},{13, 75},{26, 75},{39, 75},{52, 75},{65, 75},{78, 75},{0, 76},{13, 76},{26, 76},{39, 76},{52, 76},{65, 76},{78, 76},{0, 77},{13, 77},{26, 77},{39, 77},{52, 77},{65, 77},{78, 77},{0, 78},{13, 78},{26, 78},{39, 78},{52, 78},{65, 78},{78, 78},{0, 79},{13, 79},{26, 79},{39, 79},{52, 79},{65, 79},{78, 79},{0, 80},{13, 80},{26, 80},{39, 80},{52, 80},{65, 80},{78, 80},{0, 81},{13, 81},{26, 81},{39, 81},{52, 81},{65, 81},{78, 81},{0, 82},{13, 82},{26, 82},{39, 82},{52, 82},{65, 82},{78, 82},{0, 83},{13, 83},{26, 83},{39, 83},{52, 83},{65, 83},{78, 83},{0, 84},{13, 84},{26, 84},{39, 84},{52, 84},{65, 84},{78, 84},{0, 85},{1, 85},{2, 85},{3, 85},{4, 85},{5, 85},{6, 85},{7, 85},{8, 85},{9, 85},{10, 85},{11, 85},{12, 85},{13, 85},{14, 85},{15, 85},{16, 85},{17, 85},{18, 85},{19, 85},{20, 85},{21, 85},{22, 85},{23, 85},{24, 85},{25, 85},{26, 85},{27, 85},{28, 85},{29, 85},{30, 85},{31, 85},{32, 85},{33, 85},{34, 85},{35, 85},{36, 85},{37, 85},{38, 85},{39, 85},{40, 85},{41, 85},{42, 85},{43, 85},{44, 85},{45, 85},{46, 85},{47, 85},{48, 85},{49, 85},{50, 85},{51, 85},{52, 85},{53, 85},{54, 85},{55, 85},{56, 85},{57, 85},{58, 85},{59, 85},{60, 85},{61, 85},{62, 85},{63, 85},{64, 85},{65, 85},{66, 85},{67, 85},{68, 85},{69, 85},{70, 85},{71, 85},{72, 85},{73, 85},{74, 85},{75, 85},{76, 85},{77, 85},{78, 85}}

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

struct bit_descturction_animation bit_descturction_animations[ROWS * COLUMNS];
int32_t bit_destruction_animation_array_pointer = 0;

struct laser_shot laser_shots[ROWS * COLUMNS * 8];
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
struct player p1;
Surface *box;
int32_t backgound_gray_pixel[746][2] = BACKGROUND_GREY_PIXEL;

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
    shot.last_shot = 0;
    shot.destroyed = false;
    laser_shots[laser_shots_poiter] = shot;
    laser_shots_poiter++;
    if (laser_shots_poiter >= ROWS*COLUMNS*8){
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

void update_laser_shots(){

    for (int i = 0; i < ROWS * COLUMNS * 8; i++) {
        if (laser_shots[i].destroyed){
            continue;
        }
        if (bit_blocks[laser_shots[i].row][laser_shots[i].column] != BLASTER){
            laser_shots[i].destroyed = true;
            return;
        }
        if (timer_get_ms() - laser_shots[i].last_shot > LASER_SHOT_DELAY && timer_get_ms() > LASER_SHOT_DELAY) {
            laser_shots[i].active = true;
        }
        if (timer_get_ms() - laser_shots[i].last_shot > LASER_SHOT_DELAY + LASER_SHOT_DURATION) {
            laser_shots[i].active = false;
            laser_shots[i].last_shot = timer_get_ms();
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
        if (p1.x - p1.velx < 43 * 1000 + 2) {
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
            if ( linesIntersect(horizontal_mesh[i], gravity_center_line)) {
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
        if (p1.gliding && p1.vely > (int32_t) (12 * MAX(FRAMETIME, delta))) {
            p1.vely = (int32_t) (12 * MAX(FRAMETIME, delta));
        }
    }
    p1.x += p1.velx;
    p1.y += p1.vely;
}


void render() {
    surf_fill(box, BLACK);
    // render Background
    for (int32_t pixel = 0; pixel < 746; pixel++) {
        surf_set_pixel(box, backgound_gray_pixel[pixel][0], backgound_gray_pixel[pixel][1], GRAY);
    }

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
            int32_t y = (laser_shots[i].row * 12) + 4;
            int32_t w = ((laser_shots[i].left_expansion + laser_shots[i].right_expansion + 1) * 13) + 1;
            for (int j = x; j < x+w; j++){
                surf_set_pixel(box, j, y, RED);
                surf_set_pixel(box, j, y+1, DARK_RED);
                surf_set_pixel(box, j, y+2, RED);
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
                case BLASTER:
                    surf_draw_filled_rectangle(box, x, y, w, h, GRAY);
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
    rng_init();
    gpu_update_palette(palette);
    p1.x = 96000;
    p1.y = 79000;
    p1.velx = 0;
    p1.vely = -100;

    uint32_t stop;
    uint32_t start;

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
