#include <fcntl.h>
#include <linux/fb.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

void set_pixel_value(struct fb_var_screeninfo info, char* tela, int x, int y){
    const unsigned int bytes_per_line = info.xres / 8;
    const unsigned int mem_pos = y * bytes_per_line + x * info.bits_per_pixel / 8;
    const char value = 0x01 << x % 8;
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

    int x = 0;
    int y = 0;

    set_pixel_value(info, screen, x, y);

    close(fb);
    return 0;
}
