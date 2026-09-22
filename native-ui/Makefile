PS5_HOST ?= ps5
SDK ?= /opt/ps5-payload-sdk
PORTROOT ?= $(SDK)/target/user/homebrew
CC := $(SDK)/bin/prospero-clang
OUT := build
UI := $(OUT)/goldengames_retrohub_ui.elf
BOOT := $(OUT)/goldengames_retrohub_bootstrap.elf
GPL := gpl/pt.c gpl/elfldr.c gpl/hbldr.c gpl/fs_readfile.c
INC := -I$(SDK)/target/include -I$(SDK)/target/user/homebrew/include -I$(PORTROOT)/include -I$(PORTROOT)/include/SDL2 -I.
CFLAGS := -O2 -Wall -Wextra -D__PS5__ $(INC)
SDL := $(PORTROOT)/lib/libSDL2main.a $(PORTROOT)/lib/libSDL2.a
SYS := -L$(PORTROOT)/lib -lSceSystemService -lSceUserService -lSceImeDialog -lSceKeyboard -lSceVideoOut -lSceAudioOut -lScePad -lkernel_sys -lm -lz -lbz2 -liconv
PROBE := $(OUT)/goldengames_retrohub_probe.elf
NOTIFY_PROBE := $(OUT)/goldengames_retrohub_notify_probe.elf
all: $(UI) $(BOOT) $(PROBE) $(NOTIFY_PROBE)
$(OUT): ; mkdir -p $(OUT)
$(UI): retrohub_main.c retrohub_ui.c retrohub_runtime.c retrohub_systems.h | $(OUT)
	$(CC) $(CFLAGS) -o $@ retrohub_main.c retrohub_ui.c retrohub_runtime.c $(SDL) $(SYS)
$(BOOT): retrohub_bootstrap.c $(GPL) | $(OUT)
	$(CC) $(CFLAGS) -o $@ retrohub_bootstrap.c $(GPL) $(SYS)
$(PROBE): payload_probe.c | $(OUT)
	$(CC) -Wall -Werror -g -o $@ payload_probe.c
$(NOTIFY_PROBE): payload_notify_probe.c | $(OUT)
	$(CC) -Wall -Werror -g -lSceNotification -o $@ payload_notify_probe.c
clean: ; rm -rf $(OUT)
.PHONY: all clean
