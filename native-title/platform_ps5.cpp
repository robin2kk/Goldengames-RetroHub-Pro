/*
 * Goldengames RetroHub Pro - PS5 native VideoOut adapter
 * Copyright (C) 2026 GoldenGames contributors
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Platform bootstrap derived from the GPL-3.0-or-later native VideoOut example in
 * BlackBearReloaded/ps5-native-app-boilerplate. GoldenGames rendering and UI are separate.
 */
#include "diagnostic_frame.h"
#include <stdint.h>
#include <stddef.h>

extern "C" {
size_t sceKernelGetDirectMemorySize();
int sceKernelAllocateDirectMemory(int64_t,int64_t,size_t,size_t,int,int64_t*);
int sceKernelMapDirectMemory(void**,size_t,int,int,int64_t,size_t);
int sceVideoOutOpen(int32_t,int32_t,int32_t,const void*);
int sceVideoOutSetFlipRate(int32_t,int32_t);
int sceVideoOutSubmitFlip(int32_t,int32_t,uint32_t,int64_t);
int sceVideoOutWaitVblank(int32_t);
void sceVideoOutSetBufferAttribute2(void*,uint64_t,uint32_t,uint32_t,uint32_t,uint64_t,uint32_t,uint64_t);
int sceVideoOutRegisterBuffers2(int32_t,int32_t,int32_t,void*,int32_t,void*,int32_t,void*);
}

namespace {
constexpr unsigned W=1920,H=1080;
constexpr size_t FRAME_BYTES=0x1000000, ALIGN=0x200000;
struct Buffer { void *data,*metadata,*reserved0,*reserved1; };
struct Attribute { uint8_t reserved[80]; };
int video=-1,front=0; uint8_t *base=nullptr;

static size_t tiled_offset(unsigned x,unsigned y){
 uint32_t o=((y<<4)&0x70U)^((y<<5)&0xf00U)^((y<<9)&0x1000U)^((y<<8)&0x4000U)^
            ((x<<2)&0xcU)^((x<<5)&0x380U)^((x<<4)&0x400U)^((x<<6)&0x800U)^((x<<9)&0xa000U);
 uint32_t blocks=(W+127U)>>7, block=(y>>7)*blocks+(x>>7);
 return ((size_t)block<<16)+o;
}
static void flush(void *p,size_t n){
 for(uint8_t *q=(uint8_t*)p,*e=q+n;q<e;q+=64) __asm__ volatile("clflush (%0)"::"r"(q):"memory");
 __asm__ volatile("mfence":::"memory");
}
}

/* Renderer-facing surface is linear in coordinates but PS5 VideoOut memory is tiled.
 * The diagnostic renderer therefore writes through this helper in the next commit. */
extern "C" void gg_platform_put_pixel(unsigned x,unsigned y,uint32_t color){
 if(!base||x>=W||y>=H)return;
 uint8_t *frame=base+(size_t)front*FRAME_BYTES;
 *(uint32_t*)(frame+tiled_offset(x,y))=color;
}

extern "C" int gg_platform_display_open(unsigned width,unsigned height){
 if(width!=W||height!=H)return 0;
 video=sceVideoOutOpen(0xff,0,0,nullptr); if(video<0)return 0;
 size_t pool=sceKernelGetDirectMemorySize(); if(pool<FRAME_BYTES*2)return 0;
 int64_t phys=0;
 if(sceKernelAllocateDirectMemory(0,(int64_t)pool,FRAME_BYTES*2,ALIGN,3,&phys)<0)return 0;
 void *mapped=nullptr;
 if(sceKernelMapDirectMemory(&mapped,FRAME_BYTES*2,0x33,0,phys,ALIGN)<0)return 0;
 base=(uint8_t*)mapped;
 Buffer buffers[2]={{base,nullptr,nullptr,nullptr},{base+FRAME_BYTES,nullptr,nullptr,nullptr}};
 Attribute attr{};
 sceVideoOutSetFlipRate(video,0);
 sceVideoOutSetBufferAttribute2(&attr,UINT64_C(0x8000000022000000),0,W,H,0,0,0);
 if(sceVideoOutRegisterBuffers2(video,0,0,buffers,2,&attr,0,nullptr)<0)return 0;
 return 1;
}
extern "C" uint32_t *gg_platform_display_backbuffer(unsigned *pitch_pixels){
 if(pitch_pixels)*pitch_pixels=W;
 return (uint32_t*)(base+(size_t)front*FRAME_BYTES);
}
extern "C" int gg_platform_display_present(void){
 if(video<0||!base)return 0;
 flush(base+(size_t)front*FRAME_BYTES,FRAME_BYTES);
 if(sceVideoOutSubmitFlip(video,front,1,1)<0)return 0;
 sceVideoOutWaitVblank(video); front^=1; return 1;
}
