#include "diagnostic_frame.h"

static void fill(GGSurface s,int x,int y,int w,int h,uint32_t c){
 if(!s.pixels)return;
 if(x<0){w+=x;x=0;} if(y<0){h+=y;y=0;}
 if(x+w>(int)s.width)w=(int)s.width-x;
 if(y+h>(int)s.height)h=(int)s.height-y;
 for(int yy=y;yy<y+h;yy++) for(int xx=x;xx<x+w;xx++) s.pixels[(unsigned)yy*s.pitch_pixels+(unsigned)xx]=c;
}
static void glyph(GGSurface s,int x,int y,char ch,int scale,uint32_t c){
 /* Compact 5x7 diagnostic font. Only characters required by the acceptance screen. */
 static const char chars[]=" ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.-";
 static const unsigned char rows[][7]={
 {0,0,0,0,0,0,0},{14,17,17,31,17,17,17},{30,17,17,30,17,17,30},{14,17,16,16,16,17,14},
 {30,17,17,17,17,17,30},{31,16,16,30,16,16,31},{31,16,16,30,16,16,16},{14,17,16,23,17,17,15},
 {17,17,17,31,17,17,17},{31,4,4,4,4,4,31},{7,2,2,2,18,18,12},{17,18,20,24,20,18,17},
 {16,16,16,16,16,16,31},{17,27,21,21,17,17,17},{17,25,21,19,17,17,17},{14,17,17,17,17,17,14},
 {30,17,17,30,16,16,16},{14,17,17,17,21,18,13},{30,17,17,30,20,18,17},{15,16,16,14,1,1,30},
 {31,4,4,4,4,4,4},{17,17,17,17,17,17,14},{17,17,17,17,17,10,4},{17,17,17,21,21,21,10},
 {17,17,10,4,10,17,17},{17,17,10,4,4,4,4},{31,1,2,4,8,16,31},
 {14,17,19,21,25,17,14},{4,12,4,4,4,4,14},{14,17,1,2,4,8,31},{30,1,1,14,1,1,30},
 {2,6,10,18,31,2,2},{31,16,16,30,1,1,30},{14,16,16,30,17,17,14},{31,1,2,4,8,8,8},
 {14,17,17,14,17,17,14},{14,17,17,15,1,1,14},{0,0,0,0,0,0,4},{0,0,0,31,0,0,0}};
 int idx=0; while(chars[idx]&&chars[idx]!=ch)idx++; if(!chars[idx])return;
 for(int r=0;r<7;r++)for(int col=0;col<5;col++)if(rows[idx][r]&(1u<<(4-col)))fill(s,x+col*scale,y+r*scale,scale,scale,c);
}
static void text(GGSurface s,int x,int y,const char*t,int scale,uint32_t c){for(;*t;t++,x+=6*scale)glyph(s,x,y,*t,scale,c);}
void gg_draw_acceptance_frame(GGSurface s){
 fill(s,0,0,(int)s.width,(int)s.height,0xff090d18u);
 fill(s,0,0,(int)s.width,18,0xff2d7df6u);
 text(s,100,70,"GOLDENGAMES RETROHUB PRO",5,0xffffffffu);
 text(s,102,125,"NATIVE TITLE - BUILD 001",2,0xff9fb4d8u);
 fill(s,0,205,(int)s.width,8,0xff2d7df6u);
 const uint32_t accents[5]={0xffd9d9d9u,0xff7f6bb2u,0xffd63232u,0xff3f7fd9u,0xffe6b22eu};
 for(int i=0;i<5;i++){int w=i==2?330:230,h=i==2?450:330,x=960+(i-2)*300-w/2,y=i==2?300:360;
   fill(s,x-6,y-6,w+12,h+12,accents[i]); fill(s,x,y,w,h,0xff171d2au);
 }
 text(s,765,790,"RETROHUB",4,0xffffffffu);
 fill(s,0,955,(int)s.width,125,0xff05070cu);
 text(s,120,990,"LEFT RIGHT  SELECT SYSTEM",2,0xffffffffu);
 text(s,1190,990,"GOLDENGAMES V0.2",2,0xff9fb4d8u);
}
