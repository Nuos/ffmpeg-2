//
// Created by 张义杰 on 2018/3/6.
//
#include <SDL.h>
int main(int argc, char *argv[]) {
  //声明表面
  SDL_Surface *hello = NULL;
  SDL_Surface *screen = NULL;
    //启动SDL
    SDL_Init( SDL_INIT_EVERYTHING );

    //设置窗口
    screen = SDL_SetVideoMode( 640, 480, 32, SDL_SWSURFACE );

    //加载图像
    hello = SDL_LoadBMP("hello.bmp");
    //将图像应用到窗口上
    SDL_BlitSurface( hello, NULL, screen, NULL );

    //更新窗口
    SDL_Flip( screen );

    //暂停
    SDL_Delay( 10000 );
    //释放已加载的图像
    SDL_FreeSurface( hello );

    //退出SDL
    SDL_Quit();

    return 0;
}
