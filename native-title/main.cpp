#include "diagnostic_frame.h"
#include <unistd.h>
extern "C" int sceSystemServiceHideSplashScreen(void);
extern "C" int gg_platform_display_open(unsigned,unsigned);
extern "C" int gg_platform_display_present(void);
int main(void){
 if(!gg_platform_display_open(1920,1080)) for(;;) usleep(1000000);
 sceSystemServiceHideSplashScreen();
 GGSurface s={1920,1080};
 for(;;){gg_draw_acceptance_frame(s);if(!gg_platform_display_present())for(;;)usleep(1000000);usleep(16000);}
}
