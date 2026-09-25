/* Goldengames RetroHub Pro return bridge. SPDX-License-Identifier: GPL-3.0-or-later */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/sysctl.h>
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

/* websrv sets the launched big app's process name to retroarch.elf. App IDs
   can be reused across the handoff, so track the process rather than app ID. */
static pid_t retroarch_pid(void) {
  int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PROC, 0};
  size_t bytes = 0;
  if (sysctl(mib, 4, NULL, &bytes, NULL, 0) != 0 || bytes == 0) return -1;
  char *data = malloc(bytes);
  if (!data) return -1;
  if (sysctl(mib, 4, data, &bytes, NULL, 0) != 0) { free(data); return -1; }
  pid_t found = 0;
  for (char *ptr = data; ptr < data + bytes;) {
    struct kinfo_proc *proc = (struct kinfo_proc *)ptr;
    if (proc->ki_structsize <= 0 ||
        (size_t)proc->ki_structsize > (size_t)(data + bytes - ptr)) break;
    if (strcmp(proc->ki_comm, "retroarch.elf") == 0) {
      found = proc->ki_pid;
      break;
    }
    ptr += proc->ki_structsize;
  }
  free(data);
  return found;
}

static FILE *trace_file;
static void trace(const char *stage, int value) {
  if (!trace_file) return;
  fprintf(trace_file, "%s %d\n", stage, value);
  fflush(trace_file);
}

int main(void) {
  trace_file = fopen("/data/homebrew/PPSA99202/return-watchdog.log", "w");
  trace("watchdog-version", 4);
  /* Diagnostic only: the native title's app ID is not reliably available to
     a daemon on every start. Process tracking below does not depend on it. */
  const int origin = sceSystemServiceGetAppIdOfRunningBigApp();
  trace("origin-app-id", origin);
  pid_t retroarch = 0;

  /* A failed game launch must never leave a watcher that fires later. */
  for (int n = 0; n < 150; ++n) {
    usleep(200000);
    pid_t current = retroarch_pid();
    if (current > 0) {
      retroarch = current;
      trace("retroarch-pid", retroarch);
      break;
    }
  }
  if (retroarch <= 0) { trace("stop-no-retroarch-process", retroarch); return 2; }

  /* Exit after four hours even if the emulator never terminates. */
  int missing_checks = 0;
  for (int n = 0; n < 72000; ++n) {
    usleep(200000);
    pid_t current_pid = retroarch_pid();
    if (n == 0) trace("first-process-check", current_pid);
    if (current_pid < 0) { trace("process-query-error", current_pid); continue; }
    if (current_pid == retroarch) { missing_checks = 0; continue; }
    if (current_pid > 0) { trace("stop-new-retroarch-process", current_pid); return 0; }
    if (++missing_checks >= 3) {
      trace("retroarch-process-gone", current_pid);
      /* Wait briefly for the emulator's big-app slot to clear. */
      int current = sceSystemServiceGetAppIdOfRunningBigApp();
      for (int settle = 0; current > 0 && settle < 15; ++settle) {
        usleep(100000);
        current = sceSystemServiceGetAppIdOfRunningBigApp();
      }
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
      for (int attempt = 0; attempt < 8; ++attempt) {
        if (sceSystemServiceGetAppIdOfRunningBigApp() > 0) return 0;
        int result = sceSystemServiceLaunchApp("PPSA99202", args, &ctx);
        trace("relaunch-result", result);
        if (result >= 0) return 0;
        usleep(250000);
      }
      return 4;
    }
  }
  trace("stop-timeout", 14400);
  return 5;
}
