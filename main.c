#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdlib.h>

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

static void set_pixel_in_framebuffer(const struct fb_var_screeninfo info, char* screen, const struct coordinates point)
{
    const unsigned int bytes_per_line = info.xres / 8;
    const unsigned int mem_pos = point.y * bytes_per_line + point.x * info.bits_per_pixel / 8;
    const char value = 0x01 << point.x % 8;
    screen[mem_pos] |= value;
}

int set_pixel_value(const struct fb_var_screeninfo info, char* tela, const struct coordinates point)
{
    if (validate_coordinates(info, point) != 0) return -1;
    set_pixel_in_framebuffer(info, tela, point);

    return 0;
}


int draw_line(struct fb_var_screeninfo info, char* screen, const struct coordinates start, const struct coordinates end)
{
    //breBresenham's line algorithm
    if (validate_coordinates(info, start) != 0 || validate_coordinates(info, end) != 0) return -1;

    int dx = abs((int)end.x - (int)start.x);
    int dy = abs((int)end.y - (int)start.y);
    int sx = start.x < end.x ? 1 : -1;
    int sy = start.y < end.y ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;

    struct coordinates current_point = start;

    while (1)
    {
        set_pixel_in_framebuffer(info, screen, current_point);

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

int main()
{
    int fb = open("/dev/fb0", O_RDWR);

    struct fb_var_screeninfo info;
    ioctl(fb, FBIOGET_VSCREENINFO, &info);

    printf("Tela: %dx%d\n", info.xres, info.yres);

    unsigned int tamanho = info.xres * info.yres / 8;
    char* screen = (char*)mmap(0, tamanho, PROT_WRITE, MAP_SHARED, fb, 0);

    struct coordinates point_a = {10, 10};
    struct coordinates point_b = {118, 54};

    struct coordinates invalid_point = {150, 50};

    set_pixel_value(info, screen, invalid_point);
    if (draw_line(info, screen, point_a, point_b) != 0)
    {
        printf("Error: coordinate (%u, %u) is invalid, could not set pixel.\n",
               invalid_point.x, invalid_point.y);
    }
    else
    {
        printf("Pixel set successfully!\n");
    }


    close(fb);
    return 0;
}
