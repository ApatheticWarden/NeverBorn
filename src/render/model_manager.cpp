#include"model_manager.h"
#include<pspkernel.h>

Model LoadModel(const char* path){
    Model model;
    FILE* f = fopen(path, "r");
    if(!f) return model;

    char line[256];
    unsigned int v_count = 0;
    unsigned int f_count = 0;

    while(fgets(line, sizeof(line), f)){
        if(line[0] == 'v' && line[1] == ' '){
            ++v_count;
        } else if(line[0] == 'f' && line[1] == ' '){
            ++f_count;
        }
    }

    if(v_count == 0 || f_count == 0){ fclose(f); return model;}
    model.vertex_count = v_count;
    model.index_count = f_count * 3;

    model.vertices = (Vertex*)memalign(16, model.vertex_count * sizeof(Vertex));
    model.indices = (unsigned short*)memalign(16, model.index_count * sizeof(unsigned short));

    rewind(f); // get on the first line

    unsigned int v_idx = 0;
    unsigned int i_idx = 0;

    while(fgets(line, sizeof(line), f)){
        if(line[0] == 'v' && line[1] == ' '){
            float x,y,z;
            if(sscanf(line+2, "%f %f %f", &x, &y, &z) == 3){
                /* FILLER */
                unsigned char r = (unsigned char)((x + 1.0f) * 127.0f);
                unsigned char g = (unsigned char)((y + 1.0f) * 127.0f);
                unsigned char b = (unsigned char)((z + 1.0f) * 127.0f);
                unsigned int color = 0xFF000000 | (b << 16) | (g << 8) | r;
                /*        */

                model.vertices[v_idx++] = { color, x, y, z };
            }
        }else if(line[0] == 'f' && line[1] == ' '){
            char tok[3][64];
            if(sscanf(line+2, "%63s %63s %63s", tok[0], tok[1], tok[2]) == 3){
                for(int i = 0; i < 3; ++i){
                    int vi = 0;
                    sscanf(tok[i], "%d", &vi);
                    model.indices[i_idx++] = (unsigned short)(vi - 1);
                }
            }
        }
    }
    fclose(f);
    // Сбрасываем кэш CPU в RAM, чтобы видеочип GE через DMA увидел актуальные данные
    sceKernelDcacheWritebackInvalidateAll();
    
    return model;
}

void UnloadModel(Model& model){
    free(model.vertices);
    free(model.indices);
    model.vertices = nullptr;
    model.indices = nullptr;
    model.index_count = 0;
    model.vertex_count = 0;
}