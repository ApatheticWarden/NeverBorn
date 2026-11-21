#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H
#include<malloc.h>
#include<cstdio>
#include<cstring>

struct Vertex {
    unsigned int color; // ABGR (0xAABBGGRR)
    float x, y, z;
};

struct Model {
    Vertex* vertices = nullptr;
    unsigned short* indices = nullptr;
    unsigned int vertex_count = 0;
    unsigned int index_count = 0;
};

Model LoadModel(const char* path);
void UnloadModel(Model& model);

#endif