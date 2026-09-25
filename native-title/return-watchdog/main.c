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

static FILE *trace_file;
static void trace(const char *stage, int value) {
  if (!trace_file) return;
  fprintf(trace_file, "%s %d\n", stage, value);
  fflush(trace_file);
}

int main(void) {
  trace_file = fopen("/data/homebrew/PPSA99202/return-watchdog.log", "w");
  trace("watchdog-version", 2);
  /* The daemon starts while RetroHub is still the foreground big app. */
  const int origin = sceSystemServiceGetAppIdOfRunningBigApp();
  trace("origin-app-id", origin);
  int retroarch = -1;
  if (origin <= 0) { trace("stop-invalid-origin", origin); return 1; }

  /* A failed game launch must never leave a watcher that fires later. */
  for (int n = 0; n < 150; ++n) {
    usleep(200000);
    int current = sceSystemServiceGetAppIdOfRunningBigApp();
    if (current > 0 && current != origin) {
      retroarch = current;
      trace("retroarch-app-id", retroarch);
      break;
    }
  }
  if (retroarch <= 0) { trace("stop-no-retroarch", retroarch); return 2; }

  /* Exit after four hours even if the emulator never terminates. */
  int no_bigapp_seconds = 0;
  for (int n = 0; n < 14400; ++n) {
    usleep(1000000);
    int state = sceKernelGetAppState(retroarch, 0, 0);
    int current = sceSystemServiceGetAppIdOfRunningBigApp();
    if (n == 0) { trace("first-app-state", state); trace("first-running-app", current); }
    if (current > 0 && current != retroarch) {
      trace("stop-other-app", current);
      return 0;
    }
    no_bigapp_seconds = current <= 0 ? no_bigapp_seconds + 1 : 0;
    if (state != 0 || no_bigapp_seconds >= 3) {
      trace("retroarch-exited-state", state);
      /* Give the shell time to settle. Never interrupt another running app. */
      sleep(2);
      current = sceSystemServiceGetAppIdOfRunningBigApp();
      trace("app-before-relaunch", current);
      if (current > 0) return 0;
      app_launch_ctx_t ctx = {0};
      ctx.structsize = sizeof(ctx);
      int user_init = sceUserServiceInitialize(0);
      trace("user-init", user_init);
      if (user_init != 0) return 3;
      int user_result = sceUserServiceGetForegroundUser(&ctx.user_id);
      trace("foreground-user-result", user_result);
      if (user_result != 0) return 3;
      char *args[] = {0};
      for (int attempt = 0; attempt < 5; ++attempt) {
        if (sceSystemServiceGetAppIdOfRunningBigApp() > 0) return 0;
        int result = sceSystemServiceLaunchApp("PPSA99202", args, &ctx);
        trace("relaunch-result", result);
        if (result >= 0) return 0;
        sleep(1);
      }
      return 4;
    }
  }
  trace("stop-timeout", 14400);
  return 5;
}
