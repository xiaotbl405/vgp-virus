#include <windows.h>
#include <math.h>
#include <time.h>

#define MAX_ICONS 300
#define ICON_SPEED 30
#define AMPLITUDE 1000
#define BLOCK_SIZE 1000
#define FRAME_DELAY 0 // 恢复合理帧率

typedef struct {
    int x, y;
    int dx, dy;
    HICON hIcon;
} IconObject;

// 蜂鸣器线程函数
DWORD WINAPI BeepThread(LPVOID lpParam) {
    int melody[] = {523, 659, 784, 932};
    while(1) {
        for(int i=0; i<4; i++) {
            Beep(melody[i], 150);
            Sleep(0);
        }
    }
}

HICON LoadSystemIcon(LPCSTR lpIconName) {
    return (HICON)LoadImage(NULL, lpIconName, IMAGE_ICON,
        GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_SHARED);
}

void InvertScreen(HDC hdc, int w, int h) {
    BitBlt(hdc, 0,0,w,h,hdc,0,0,NOTSRCCOPY);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    ShowWindow(GetConsoleWindow(), SW_HIDE);
    
    // 启动蜂鸣器线程
    CreateThread(NULL, 0, BeepThread, NULL, 0, NULL);

    HDC hdc = GetDC(NULL);
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int screenH = GetSystemMetrics(SM_CYSCREEN);

    IconObject icons[MAX_ICONS];
    srand(time(NULL));
    for(int i=0; i<MAX_ICONS; i++) {
        icons[i] = (IconObject){
            rand()%screenW, rand()%screenH,
            (rand()%2?ICON_SPEED:-ICON_SPEED),
            (rand()%2?ICON_SPEED:-ICON_SPEED),
            LoadSystemIcon(IDI_WINLOGO)
        };
    }

    for(int frame=0; ;frame++) {
        // 波纹特效
        for(int y=0; y<screenH; y+=BLOCK_SIZE) {
            for(int x=0; x<screenW; x+=BLOCK_SIZE) {
                int dx = AMPLITUDE*sin(x*0.05+frame*0.1);
                int dy = AMPLITUDE*cos(y*0.05+frame*0.1);
                BitBlt(hdc,x+dx,y+dy,BLOCK_SIZE,BLOCK_SIZE,hdc,x,y,SRCCOPY);
            }
        }

        // 图标运动与碰撞音效
        for(int i=0; i<MAX_ICONS; i++) {
            if(icons[i].x<0 || icons[i].x>screenW) {
                icons[i].dx *= -1;
                Beep(2000, 20); // 边界碰撞音‌:ml-citation{ref="2" data="citationList"}
            }
            if(icons[i].y<0 || icons[i].y>screenH) {
                icons[i].dy *= -1; 
                Beep(1500, 20); // 边界碰撞音‌:ml-citation{ref="3" data="citationList"}
            }
            icons[i].x += icons[i].dx;
            icons[i].y += icons[i].dy;
            DrawIcon(hdc, icons[i].x, icons[i].y, icons[i].hIcon);
        }

        InvertScreen(hdc, screenW, screenH);
        Sleep(FRAME_DELAY);
    }

    ReleaseDC(NULL, hdc);
    return 0;
}
