#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

struct coordinates
{
    unsigned int x;
    unsigned int y;
};

int validate_coordinates(const struct fb_var_screeninfo info, const struct coordinates point)
{
    if(point.x >= info.xres || point.y >= info.yres) return -1;
        return 0;
}

void set_pixel_value(const struct fb_var_screeninfo info, char* tela, const struct coordinates point)
{
    const unsigned int bytes_per_line = info.xres / 8;
    const unsigned int mem_pos = point.y * bytes_per_line + point.x * info.bits_per_pixel / 8;
    const char value = 0x01 << point.x % 8;
    printf("mem_pos = %d, value = %d (0x%02X)\n", mem_pos, value,
           (unsigned char)value);
    tela[mem_pos] |= value;
}

int main() {
    int fb = open("/dev/fb0", O_RDWR);

    struct fb_var_screeninfo info;
    ioctl(fb, FBIOGET_VSCREENINFO, &info);

    printf("Tela: %dx%d\n", info.xres, info.yres);

    int tamanho = info.xres * info.yres / 8;
    char* screen = (char*)mmap(0, tamanho, PROT_WRITE, MAP_SHARED, fb, 0);

    struct coordinates point;

    point.x = 0;
    point.y = 0;

    set_pixel_value(info, screen, point);

    close(fb);
    return 0;
}
