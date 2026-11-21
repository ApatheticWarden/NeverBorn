#ifndef GRAPHICS_H
#define GRAPHICS_H
#include<pspdisplay.h>
#include<pspgu.h>
#include<pspgum.h>
#include"model_manager.h"
#include"font.h"

constexpr unsigned int BUF_WIDTH  = 512;
constexpr unsigned int SCR_WIDTH  = 480;
constexpr unsigned int SCR_HEIGHT = 272;
constexpr unsigned int FRAME_SIZE = BUF_WIDTH * SCR_HEIGHT * 4;

void init_gu();
void begin_frame();
void ShowModel(const Model& model);
void ShowText(short x, short y, unsigned int color, const char* text, float size);

#endif