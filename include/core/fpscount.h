#define FPS_COUNT 10

int fpsPast[FPS_COUNT];
double limitUpdateFPS = 1.0 / 60.0;
// maybe cap fps at 144 for limit? 30, 60, 120, 144
double targetFps;
double limitDrawFPS;
double averageFps;
int input[128];

double lastTime, timer;
double deltaTime = 0, nowTime = 0;
int frames = 0, updates = 0;
double lastRenderTime = 0;

int fps;