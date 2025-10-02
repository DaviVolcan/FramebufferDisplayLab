#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "font8x8_basic.h"

struct framebuffer_context
{
    struct fb_var_screeninfo info;
    char* screen;
};

struct coordinates
{
    unsigned int x;
    unsigned int y;
};

int validate_coordinates(const struct fb_var_screeninfo info, const struct coordinates point)
{
    if (point.x >= info.xres || point.y >= info.yres) return -1;
    return 0;
}

static void set_pixel_in_framebuffer(const struct framebuffer_context* fb, const struct coordinates point)
{
    const unsigned int bytes_per_line = fb->info.xres / 8;
    const unsigned int mem_pos = point.y * bytes_per_line + point.x * fb->info.bits_per_pixel / 8;
    const char value = 0x01 << point.x % 8;
    fb->screen[mem_pos] |= value;
}

int set_pixel_value(const struct framebuffer_context* fb, const struct coordinates point)
{
    if (validate_coordinates(fb->info, point) != 0) return -1;
    set_pixel_in_framebuffer(fb, point);

    return 0;
}


int draw_line(const struct framebuffer_context* fb, const struct coordinates start, const struct coordinates end)
{
    //breBresenham's line algorithm
    if (validate_coordinates(fb->info, start) != 0 || validate_coordinates(fb->info, end) != 0) return -1;

    int dx = abs((int)end.x - (int)start.x);
    int dy = abs((int)end.y - (int)start.y);
    int sx = start.x < end.x ? 1 : -1;
    int sy = start.y < end.y ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;

    struct coordinates current_point = start;

    while (1)
    {
        set_pixel_in_framebuffer(fb, current_point);

        if (current_point.x == end.x && current_point.y == end.y) break;

        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            current_point.x += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            current_point.y += sy;
        }
    }

    return 0;
}

int is_bit_set(char c, unsigned int bit)
{
    return (c >> bit) & 1;
}


void draw_character(const struct framebuffer_context* fb, const struct coordinates offset,
                    unsigned int character_number)
{
    struct coordinates bit_cordinates = {0, 0};
    for (unsigned int y = 0; y < 8; y++)
    {
        bit_cordinates.y = offset.y + y;
        for (unsigned int x = 0; x < 8; x++)
        {
            bit_cordinates.x = offset.x + x;
            if (is_bit_set(font8x8_basic[character_number][y], x))
            {
                set_pixel_in_framebuffer(fb, bit_cordinates);
            }
        }
    }
}

void draw_string(const struct framebuffer_context* fb, struct coordinates offset, const char* str)
{
    const size_t size = strlen(str);
    for (int i = 0; i < size; ++i)
    {
        draw_character(fb, offset, str[i]);
        offset.x += 8;
    }
}

int main()
{
    int fb = open("/dev/fb0", O_RDWR);
    struct framebuffer_context fb_ctx;
    ioctl(fb, FBIOGET_VSCREENINFO, &fb_ctx.info);

    printf("Screen: %dx%d\n", fb_ctx.info.xres, fb_ctx.info.yres);

    unsigned int tamanho = fb_ctx.info.xres * fb_ctx.info.yres / 8;
    fb_ctx.screen = (char*)mmap(0, tamanho, PROT_WRITE, MAP_SHARED, fb, 0);

    struct coordinates point_a = {10, 10};
    struct coordinates point_b = {118, 54};

    struct coordinates invalid_point = {150, 50};


    char str[] = "VolcanLab";

    draw_string(&fb_ctx, point_a, str);
    munmap(fb_ctx.screen, tamanho);
    close(fb);
    return 0;
}
