/* Goldengames RetroHub Pro return bridge. SPDX-License-Identifier: GPL-3.0-or-later */
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

typedef struct {
  uint32_t structsize;
  uint32_t user_id;
  uint32_t app_opt;
  uint64_t crash_report;
  uint32_t check_flag;
} app_launch_ctx_t;

int sceUserServiceInitialize(void *);
int sceUserServiceGetForegroundUser(uint32_t *);
int sceSystemServiceGetAppIdOfRunningBigApp(void);
int sceSystemServiceLaunchApp(const char *, char **, app_launch_ctx_t *);
int sceKernelGetAppState(int, int *, int *);

int main(void) {
  /* The daemon starts while RetroHub is still the foreground big app. */
  const int origin = sceSystemServiceGetAppIdOfRunningBigApp();
  int retroarch = -1;
  if (origin <= 0) return 1;

  /* A failed game launch must never leave a watcher that fires later. */
  for (int n = 0; n < 150; ++n) {
    usleep(200000);
    int current = sceSystemServiceGetAppIdOfRunningBigApp();
    if (current > 0 && current != origin) {
      retroarch = current;
      break;
    }
  }
  if (retroarch <= 0) return 2;

  /* Exit after four hours even if the emulator never terminates. */
  for (int n = 0; n < 14400; ++n) {
    usleep(1000000);
    if (sceKernelGetAppState(retroarch, 0, 0) != 0) {
      /* Give the shell time to settle. Never interrupt another running app. */
      sleep(2);
      if (sceSystemServiceGetAppIdOfRunningBigApp() > 0) return 0;
      app_launch_ctx_t ctx = {0};
      ctx.structsize = sizeof(ctx);
      if (sceUserServiceInitialize(0) != 0 ||
          sceUserServiceGetForegroundUser(&ctx.user_id) != 0) return 3;
      char *args[] = {0};
      int result = sceSystemServiceLaunchApp("PPSA99202", args, &ctx);
      printf("RetroHub return launch: %d\n", result);
      return result < 0 ? 4 : 0;
    }
  }
  return 5;
}
