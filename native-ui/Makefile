PS5_HOST ?= ps5
SDK ?= /opt/ps5-payload-sdk
CC := $(SDK)/bin/prospero-clang
OUT := build
UI := $(OUT)/goldengames_retrohub_ui.elf
BOOT := $(OUT)/goldengames_retrohub_bootstrap.elf
GPL := gpl/pt.c gpl/elfldr.c gpl/hbldr.c gpl/fs_readfile.c
INC := -I$(SDK)/target/user/homebrew/include -I$(SDK)/target/user/homebrew/include/SDL2 -I.
CFLAGS := -O2 -Wall -Wextra -D__PS5__ $(INC)
SDL := $(SDK)/target/user/homebrew/lib/libSDL2main.a $(SDK)/target/user/homebrew/lib/libSDL2.a
SYS := -lSceSystemService -lSceUserService -lSceVideoOut -lSceAudioOut -lScePad -lkernel_sys -lm -lz -lbz2 -liconv
all: $(UI) $(BOOT)
$(OUT): ; mkdir -p $(OUT)
$(UI): retrohub_main.c retrohub_ui.c retrohub_runtime.c retrohub_systems.h | $(OUT)
	$(CC) $(CFLAGS) -o $@ retrohub_main.c retrohub_ui.c retrohub_runtime.c $(SDL) $(SYS)
$(BOOT): retrohub_bootstrap.c $(GPL) | $(OUT)
	$(CC) $(CFLAGS) -o $@ retrohub_bootstrap.c $(GPL) $(SYS)
clean: ; rm -rf $(OUT)
.PHONY: all clean
