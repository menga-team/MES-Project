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
//#include "background.m3if.asset"
//#include "warning.m3if.asset"
//#include "flash.m3if.asset"

#define FPS 60
#define FRAMETIME ((1.0/FPS)*1000)


#define AIR 0
#define UNFLIPPED 1
#define FLIPPED 2
#define BLASTER 3
#define ERR_BLOCK 4

#define ROWS 7
#define COLUMNS 6

#define GRAVITY 50
#define NEW_BIT_DELAY 2000
#define WARNING_DURATION 1000
#define BLASTER_CHANCE 0.1
#define FLASH_DURATION 50
#define DESTROIED_BIT_ANIMATION_DURATION 500
#define LASER_SHOT_DURATION 750
#define LASER_SHOT_DELAY 5000

#define BACKGROUND_GREY_PIXEL {{0, 0},{1, 0},{2, 0},{3, 0},{4, 0},{5, 0},{6, 0},{7, 0},{8, 0},{9, 0},{10, 0},{11, 0},{12, 0},{13, 0},{14, 0},{15, 0},{16, 0},{17, 0},{18, 0},{19, 0},{20, 0},{21, 0},{22, 0},{23, 0},{24, 0},{25, 0},{26, 0},{27, 0},{28, 0},{29, 0},{30, 0},{31, 0},{32, 0},{33, 0},{34, 0},{35, 0},{36, 0},{37, 0},{38, 0},{39, 0},{40, 0},{41, 0},{42, 0},{43, 0},{44, 0},{45, 0},{46, 0},{47, 0},{48, 0},{49, 0},{50, 0},{51, 0},{52, 0},{53, 0},{54, 0},{55, 0},{56, 0},{57, 0},{58, 0},{59, 0},{60, 0},{61, 0},{62, 0},{63, 0},{64, 0},{65, 0},{66, 0},{67, 0},{68, 0},{69, 0},{70, 0},{71, 0},{72, 0},{73, 0},{74, 0},{75, 0},{76, 0},{77, 0},{78, 0},{0, 1},{13, 1},{26, 1},{39, 1},{52, 1},{65, 1},{78, 1},{0, 2},{13, 2},{26, 2},{39, 2},{52, 2},{65, 2},{78, 2},{0, 3},{13, 3},{26, 3},{39, 3},{52, 3},{65, 3},{78, 3},{0, 4},{13, 4},{26, 4},{39, 4},{52, 4},{65, 4},{78, 4},{0, 5},{13, 5},{26, 5},{39, 5},{52, 5},{65, 5},{78, 5},{0, 6},{13, 6},{26, 6},{39, 6},{52, 6},{65, 6},{78, 6},{0, 7},{13, 7},{26, 7},{39, 7},{52, 7},{65, 7},{78, 7},{0, 8},{13, 8},{26, 8},{39, 8},{52, 8},{65, 8},{78, 8},{0, 9},{13, 9},{26, 9},{39, 9},{52, 9},{65, 9},{78, 9},{0, 10},{13, 10},{26, 10},{39, 10},{52, 10},{65, 10},{78, 10},{0, 11},{13, 11},{26, 11},{39, 11},{52, 11},{65, 11},{78, 11},{0, 12},{13, 12},{26, 12},{39, 12},{52, 12},{65, 12},{78, 12},{0, 13},{13, 13},{26, 13},{39, 13},{52, 13},{65, 13},{78, 13},{0, 14},{13, 14},{26, 14},{39, 14},{52, 14},{65, 14},{78, 14},{0, 15},{13, 15},{26, 15},{39, 15},{52, 15},{65, 15},{78, 15},{0, 16},{13, 16},{26, 16},{39, 16},{52, 16},{65, 16},{78, 16},{0, 17},{13, 17},{26, 17},{39, 17},{52, 17},{65, 17},{78, 17},{0, 18},{13, 18},{26, 18},{39, 18},{52, 18},{65, 18},{78, 18},{0, 19},{13, 19},{26, 19},{39, 19},{52, 19},{65, 19},{78, 19},{0, 20},{13, 20},{26, 20},{39, 20},{52, 20},{65, 20},{78, 20},{0, 21},{13, 21},{26, 21},{39, 21},{52, 21},{65, 21},{78, 21},{0, 22},{13, 22},{26, 22},{39, 22},{52, 22},{65, 22},{78, 22},{0, 23},{13, 23},{26, 23},{39, 23},{52, 23},{65, 23},{78, 23},{0, 24},{13, 24},{26, 24},{39, 24},{52, 24},{65, 24},{78, 24},{0, 25},{13, 25},{26, 25},{39, 25},{52, 25},{65, 25},{78, 25},{0, 26},{13, 26},{26, 26},{39, 26},{52, 26},{65, 26},{78, 26},{0, 27},{13, 27},{26, 27},{39, 27},{52, 27},{65, 27},{78, 27},{0, 28},{13, 28},{26, 28},{39, 28},{52, 28},{65, 28},{78, 28},{0, 29},{13, 29},{26, 29},{39, 29},{52, 29},{65, 29},{78, 29},{0, 30},{13, 30},{26, 30},{39, 30},{52, 30},{65, 30},{78, 30},{0, 31},{13, 31},{26, 31},{39, 31},{52, 31},{65, 31},{78, 31},{0, 32},{13, 32},{26, 32},{39, 32},{52, 32},{65, 32},{78, 32},{0, 33},{13, 33},{26, 33},{39, 33},{52, 33},{65, 33},{78, 33},{0, 34},{13, 34},{26, 34},{39, 34},{52, 34},{65, 34},{78, 34},{0, 35},{13, 35},{26, 35},{39, 35},{52, 35},{65, 35},{78, 35},{0, 36},{13, 36},{26, 36},{39, 36},{52, 36},{65, 36},{78, 36},{0, 37},{13, 37},{26, 37},{39, 37},{52, 37},{65, 37},{78, 37},{0, 38},{13, 38},{26, 38},{39, 38},{52, 38},{65, 38},{78, 38},{0, 39},{13, 39},{26, 39},{39, 39},{52, 39},{65, 39},{78, 39},{0, 40},{13, 40},{26, 40},{39, 40},{52, 40},{65, 40},{78, 40},{0, 41},{13, 41},{26, 41},{39, 41},{52, 41},{65, 41},{78, 41},{0, 42},{13, 42},{26, 42},{39, 42},{52, 42},{65, 42},{78, 42},{0, 43},{13, 43},{26, 43},{39, 43},{52, 43},{65, 43},{78, 43},{0, 44},{13, 44},{26, 44},{39, 44},{52, 44},{65, 44},{78, 44},{0, 45},{13, 45},{26, 45},{39, 45},{52, 45},{65, 45},{78, 45},{0, 46},{13, 46},{26, 46},{39, 46},{52, 46},{65, 46},{78, 46},{0, 47},{13, 47},{26, 47},{39, 47},{52, 47},{65, 47},{78, 47},{0, 48},{13, 48},{26, 48},{39, 48},{52, 48},{65, 48},{78, 48},{0, 49},{13, 49},{26, 49},{39, 49},{52, 49},{65, 49},{78, 49},{0, 50},{13, 50},{26, 50},{39, 50},{52, 50},{65, 50},{78, 50},{0, 51},{13, 51},{26, 51},{39, 51},{52, 51},{65, 51},{78, 51},{0, 52},{13, 52},{26, 52},{39, 52},{52, 52},{65, 52},{78, 52},{0, 53},{13, 53},{26, 53},{39, 53},{52, 53},{65, 53},{78, 53},{0, 54},{13, 54},{26, 54},{39, 54},{52, 54},{65, 54},{78, 54},{0, 55},{13, 55},{26, 55},{39, 55},{52, 55},{65, 55},{78, 55},{0, 56},{13, 56},{26, 56},{39, 56},{52, 56},{65, 56},{78, 56},{0, 57},{13, 57},{26, 57},{39, 57},{52, 57},{65, 57},{78, 57},{0, 58},{13, 58},{26, 58},{39, 58},{52, 58},{65, 58},{78, 58},{0, 59},{13, 59},{26, 59},{39, 59},{52, 59},{65, 59},{78, 59},{0, 60},{13, 60},{26, 60},{39, 60},{52, 60},{65, 60},{78, 60},{0, 61},{13, 61},{26, 61},{39, 61},{52, 61},{65, 61},{78, 61},{0, 62},{13, 62},{26, 62},{39, 62},{52, 62},{65, 62},{78, 62},{0, 63},{13, 63},{26, 63},{39, 63},{52, 63},{65, 63},{78, 63},{0, 64},{13, 64},{26, 64},{39, 64},{52, 64},{65, 64},{78, 64},{0, 65},{13, 65},{26, 65},{39, 65},{52, 65},{65, 65},{78, 65},{0, 66},{13, 66},{26, 66},{39, 66},{52, 66},{65, 66},{78, 66},{0, 67},{13, 67},{26, 67},{39, 67},{52, 67},{65, 67},{78, 67},{0, 68},{13, 68},{26, 68},{39, 68},{52, 68},{65, 68},{78, 68},{0, 69},{13, 69},{26, 69},{39, 69},{52, 69},{65, 69},{78, 69},{0, 70},{13, 70},{26, 70},{39, 70},{52, 70},{65, 70},{78, 70},{0, 71},{13, 71},{26, 71},{39, 71},{52, 71},{65, 71},{78, 71},{0, 72},{13, 72},{26, 72},{39, 72},{52, 72},{65, 72},{78, 72},{0, 73},{13, 73},{26, 73},{39, 73},{52, 73},{65, 73},{78, 73},{0, 74},{13, 74},{26, 74},{39, 74},{52, 74},{65, 74},{78, 74},{0, 75},{13, 75},{26, 75},{39, 75},{52, 75},{65, 75},{78, 75},{0, 76},{13, 76},{26, 76},{39, 76},{52, 76},{65, 76},{78, 76},{0, 77},{13, 77},{26, 77},{39, 77},{52, 77},{65, 77},{78, 77},{0, 78},{13, 78},{26, 78},{39, 78},{52, 78},{65, 78},{78, 78},{0, 79},{13, 79},{26, 79},{39, 79},{52, 79},{65, 79},{78, 79},{0, 80},{13, 80},{26, 80},{39, 80},{52, 80},{65, 80},{78, 80},{0, 81},{13, 81},{26, 81},{39, 81},{52, 81},{65, 81},{78, 81},{0, 82},{13, 82},{26, 82},{39, 82},{52, 82},{65, 82},{78, 82},{0, 83},{13, 83},{26, 83},{39, 83},{52, 83},{65, 83},{78, 83},{0, 84},{13, 84},{26, 84},{39, 84},{52, 84},{65, 84},{78, 84},{0, 85},{1, 85},{2, 85},{3, 85},{4, 85},{5, 85},{6, 85},{7, 85},{8, 85},{9, 85},{10, 85},{11, 85},{12, 85},{13, 85},{14, 85},{15, 85},{16, 85},{17, 85},{18, 85},{19, 85},{20, 85},{21, 85},{22, 85},{23, 85},{24, 85},{25, 85},{26, 85},{27, 85},{28, 85},{29, 85},{30, 85},{31, 85},{32, 85},{33, 85},{34, 85},{35, 85},{36, 85},{37, 85},{38, 85},{39, 85},{40, 85},{41, 85},{42, 85},{43, 85},{44, 85},{45, 85},{46, 85},{47, 85},{48, 85},{49, 85},{50, 85},{51, 85},{52, 85},{53, 85},{54, 85},{55, 85},{56, 85},{57, 85},{58, 85},{59, 85},{60, 85},{61, 85},{62, 85},{63, 85},{64, 85},{65, 85},{66, 85},{67, 85},{68, 85},{69, 85},{70, 85},{71, 85},{72, 85},{73, 85},{74, 85},{75, 85},{76, 85},{77, 85},{78, 85}}

#define BLACK 0b000
#define WHITE 0b001
#define YELLOW 0b010
#define RED 0b011
#define DARK_RED 0b100
#define GREEN 0b101
#define BLUE 0b110
#define GRAY 0b111

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
    int x;
    int y;
};

struct line {
    int x;
    int y;
    int a;
    int b;
};

struct player {
    int x;
    int y;
    int velx;
    int vely;
    bool last_up_keystate;
    bool last_down_keystate;
    bool in_air;
    bool gliding;
    bool stomping;
};

struct bit_descturction_animation {
    int animation_start;
    int row;
    int column;
};

struct laser_shot {
    int last_shot;
    int row;
    int column;
    bool active;
    int right_expansion;
    int left_expansion;
};

bool warning = false;
bool flash = false;
int last_warning_start = 0;
int last_flash_start = 0;
int interest_column = 0;

struct bit_descturction_animation bit_descturction_animations[ROWS * COLUMNS];
int bit_destruction_animation_array_pointer = 0;

int count = 0;

int bit_blocks[ROWS][COLUMNS] = {
        {AIR,     AIR,       AIR,       AIR,       AIR,       AIR},
        {AIR,     AIR,       AIR,       AIR,       AIR,       AIR},
        {AIR,     AIR,       AIR,       AIR,       AIR,       AIR},
        {BLASTER, UNFLIPPED, FLIPPED,   UNFLIPPED, FLIPPED,   BLASTER},
        {BLASTER, FLIPPED,   UNFLIPPED, FLIPPED,   UNFLIPPED, ERR_BLOCK},
        {BLASTER, UNFLIPPED, FLIPPED,   UNFLIPPED, FLIPPED,   BLASTER},
        {BLASTER, FLIPPED,   UNFLIPPED, FLIPPED,   UNFLIPPED, ERR_BLOCK},
};

Surface warning_texture;
struct player p1;
Surface *box;
int backgound_gray_pixel[746][2] = BACKGROUND_GREY_PIXEL;
Surface flash_texture;


void handle_input() {

    if (input_get_button(0, BUTTON_B) && p1.in_air && !p1.last_down_keystate) {
        p1.stomping = true;
    }
    if (input_get_button(0, BUTTON_LEFT) && !p1.stomping) {
        p1.velx += -700;
    }
    if (input_get_button(0, BUTTON_RIGHT) && !p1.stomping) {
        p1.velx += 700;
    }

//        if ((! p1.in_air or  p1.gliding) &&event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_UP) {
//             p1.vely = -1.3;
//        }
    if (input_get_button(0, BUTTON_A) && (!p1.in_air || p1.gliding) && !p1.last_up_keystate) {
        p1.vely = -1.3;
//        p1.y -= 0.05;
    }
    p1.last_up_keystate = input_get_button(0, BUTTON_A);
    p1.last_down_keystate = input_get_button(0, BUTTON_B);
}

int column_air_amount(int column) {
    for (int i = 0; i < ROWS; ++i) {
        if (bit_blocks[i][column] != AIR) {
            return i;
        }
    }
    return ROWS;
}

void spawn_warning() {
    last_warning_start = count;
    interest_column = rng_u32() % 6;
    warning = true;
    flash = false;
}

void spawn_flash() {
    last_flash_start = count;
    interest_column = interest_column;
    warning = false;
    flash = true;
}

void end_flash() {
    warning = false;
    flash = false;
}


bool check_for_limitreach(int column) {
    return bit_blocks[0][column] != AIR;
}

void add_bit_descturction_animation(struct bit_descturction_animation ani) {
    bit_descturction_animations[bit_destruction_animation_array_pointer] = ani;
    bit_destruction_animation_array_pointer++;
    if (bit_destruction_animation_array_pointer >= ROWS * COLUMNS)
        bit_destruction_animation_array_pointer = 0;
}

void send_error_block(int column) {
    bit_blocks[0][column] = AIR;
    for (int i = 1; i < ROWS - 1; ++i) {
        struct bit_descturction_animation ani;
        ani.animation_start = count;
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
    int new_block_type = 0;
    if (rng_u32() % (int) (1 / BLASTER_CHANCE) == 0) { new_block_type = BLASTER; }
    else {
//        rand = dis(gen);
        new_block_type = UNFLIPPED;
    }

    for (int i = 1; i < ROWS; ++i) {
        if (bit_blocks[i][interest_column] != AIR) {
            bit_blocks[i - 1][interest_column] = new_block_type;
//            if (new_block_type == MEM_LEAK) { handle_new_laser(i - 1, column); } TODO:
            return;
        }
    }
    bit_blocks[6][interest_column] = new_block_type;
//    if (new_block_type == MEM_LEAK) { handle_new_laser(6, column); } TODO:
}

void update_player_speed() {
//        if ( player1.gliding) {  player1.vely = 4; }
    if (p1.stomping) { p1.vely = 100000; }

    if (p1.in_air) {
        p1.vely += GRAVITY;
    } else {
        p1.vely = 0;
    }
//    if (!player1.in_air && player1.last_tick_in_air)
}

void handle_box_collision() {
    p1.gliding = false;
    if (p1.velx > 0) {
        if (p1.x + p1.velx + 7*1000 > 119*1000) {
            p1.velx = 0;
            p1.x = 119*1000 - 7*1000;
            p1.gliding = true;
        }
    } else if (p1.velx < 0) {
        if (p1.x - p1.velx < 43*1000) {
            p1.velx = 0;
            p1.x = 42*1000;
            p1.gliding = true;
        }
    }
    if (p1.vely >= 0 && p1.y >= 119*1000 - 9*1000) {
        p1.vely = 0;
        p1.y = 119*1000 - 9*1000;
        p1.in_air = false;
    } else {
        p1.in_air = true;
    }
    if (p1.vely <= 0 && p1.y <= 32*1000) {
        p1.vely = 0;
    }
}

void correct_player_stomp_trajectory() {
//    if (p1.x / 1000 * 2 % 2) // TODO figure this outt
//    int x = ((int) round(p1.x + 4.5) - 42) % 13;
//    std::cout << x << std::endl;
//    if (x > 13 - 2.5) {
//        p1.velx = -1;
//        p1.vely = 0.1;
//    } else if (x < 3.5) {
//        p1.velx = 1;
//        p1.vely = 0.1;
//    }
}

void handle_player() {
    update_player_speed();
    handle_input();
    if (p1.stomping) {
        p1.vely = 2;
        handle_box_collision();
        if (p1.vely == 0) {

            p1.vely = -1.3;
            p1.in_air = true;
            p1.stomping = false;
            return;
        }
        correct_player_stomp_trajectory();
//        handle_stomp_obstacle();
        p1.x += p1.velx;
        p1.y += p1.vely;

    } else {
//        flip_blocks();

        handle_box_collision();
//        handle_block_collision();
        if (p1.gliding && p1.vely > 0.2) {
            p1.vely = 0.2;
        }
        p1.x += p1.velx;
        p1.y += p1.vely;
    }
}


void render() {
    surf_fill(box, WHITE);
//    // render Background
//    for (int pixel = 0; pixel < 746; pixel++) {
//        surf_set_pixel(box, backgound_gray_pixel[pixel][0], backgound_gray_pixel[pixel][1], WHITE);
//    }
//
//    for (int bit = 0; bit < ROWS * COLUMNS; bit++) {
//        if (count - bit_descturction_animations[bit].animation_start < DESTROIED_BIT_ANIMATION_DURATION &&
//            count > DESTROIED_BIT_ANIMATION_DURATION) {
//            surf_set_pixel(box, 1 + (bit_descturction_animations[bit].column * 13) + 3,
//                           1 + (bit_descturction_animations[bit].row * 12) + 3, WHITE);
//            surf_set_pixel(box, 1 + (bit_descturction_animations[bit].column * 13) + 3,
//                           1 + (bit_descturction_animations[bit].row * 12) + 4, WHITE);
//            surf_set_pixel(box, 1 + (bit_descturction_animations[bit].column * 13) + 4,
//                           1 + (bit_descturction_animations[bit].row * 12) + 3, WHITE);
//            surf_set_pixel(box, 1 + (bit_descturction_animations[bit].column * 13) + 4,
//                           1 + (bit_descturction_animations[bit].row * 12) + 4, WHITE);
//        }
//    }


//    if (warning) {
//        SDL_Rect destination_rect;
//        int x = 1 + (interest_column * 13);
//        int y = 1;
////        int w = 12;
////        int h = 12 * (column_air_amount(interest_column));
//        surf_draw_surf(box, &warning_texture,  x, y);
//    } else if (flash) {
//        SDL_Rect destination_rect;
//        int x = 1 + (interest_column * 13);
//        int y = 1 - (12 * (COLUMNS - column_air_amount(interest_column)));
//        int w = 12;
//        int h = 12 * (COLUMNS);
//
////        SDL_Rect source_rect;
////        source_rect.x = 0;
////        source_rect.y = 84 - (12 * (column_air_amount(interest_column)));
////        source_rect.w = 12;
////        source_rect.h = 12 * (column_air_amount(interest_column));
//        surf_draw_surf(box, &flash_texture,  x, y);
//    }


    // render blocks
//    for (int i = 0; i < ROWS; ++i) {
//        for (int j = 0; j < COLUMNS; ++j) {
//            int x = 1 + (j * 13);
//            int y = 1 + (i * 12);
//            int w = 12;
//            int h = 12;
//
////            std::cout << zero_texture.get_surface_rect(count, 0)->x << ", " << zero_texture.get_surface_rect(count, 0)->y << "\n";
//            switch (bit_blocks[i][j]) {
//                case UNFLIPPED:
//                    surf_draw_filled_rectangle(box, x, y, w, h, RED);
//                    break;
//                case FLIPPED:
//                    surf_draw_filled_rectangle(box, x, y, w, h, YELLOW);
//                    break;
//                case BLASTER:
//                    surf_draw_filled_rectangle(box, x, y, w, h, GRAY);
//                    break;
//                case ERR_BLOCK:
//                    surf_draw_filled_rectangle(box, x, y, w, h, GREEN);
//                    break;
//            }
//        }
//    }

    gpu_send_buf(BACK_BUFFER, box->w, box->h, 41, 34, box->d);
    gpu_swap_buf();

}

uint8_t start(void) {
    Surface temp = surf_create(1+(COLUMNS*13), 2+(ROWS*12));
    box = &temp;
//    warning_texture = surf_create_from_memory(12, 84, ASSET_WARNING_M3IF);
//    flash_texture = surf_create_from_memory(12, 84, ASSET_FLASH_M3IF);
    rng_init();
    gpu_update_palette(palette);
    p1.x = 96000;
    p1.y = 79000;
    p1.velx = 0;
    p1.vely = -100;

    uint32_t stop;
    uint32_t start;
    uint32_t delta;

//    gpu_blank(FRONT_BUFFER, 0);
//    gpu_blank(BACK_BUFFER, 0);

    while (1) {
        count += 10;


//        if (count % NEW_BIT_DELAY == 0 && !warning && !flash) {
//            spawn_warning();
//        } else if (warning && count - last_warning_start > WARNING_DURATION) {
//            spawn_flash();
//            spawn_block();
//        } else if (flash && count - last_flash_start > FLASH_DURATION) {
//            end_flash();
//        }
////        Framework::trash_expred_bit_destruction_animations();
////        Framework::update_laser_shots(); //TODO:
//        for (int i = 0; i < COLUMNS; ++i) {
//            if (check_for_limitreach(i)) {
//                send_error_block(i);
//            }
//        }



//        gpu_print_text(FRONT_BUFFER, 2, 2, WHITE, WHITE, "LEVEL");

//        gpu_send_buf(FRONT_BUFFER, space.w, space.h, x(10), y(0), space.d);

        render();
        // timing
        stop = timer_get_ms();
        delta = stop - start;
        printf("%u\n", delta);
        if (delta < FRAMETIME) {gpu_blank(FRONT_BUFFER, 0);gpu_blank(FRONT_BUFFER, 0);
            timer_block_ms(FRAMETIME - delta);
        }
        start = timer_get_ms();
    }
    return CODE_RESTART;
}
