#include"graphics.h"

// Display list (aka command buffer for GE) used in DMA
// CPU working while this is rendering
// 16 bc DMA reads 16 bytes at once
static unsigned int __attribute__((aligned(16))) list[262144];

void init_gu(){
    sceGuInit();
    
    // GU_DIRECT - type of command passing. Means that at the moment of sending - begins to process
    sceGuStart(GU_DIRECT, list);
    
    // Use shade
    sceGuShadeModel(GU_SMOOTH); // Important for gradient and smoothing

    /* VRAM BUFFERS ALLOCATION */

    // Allocate some space in VRAM
    // Draw Buffer
    sceGuDrawBuffer(GU_PSM_8888, (void*)0, BUF_WIDTH);
    // Display Buffer
    sceGuDispBuffer(SCR_WIDTH, SCR_HEIGHT, (void*)FRAME_SIZE, BUF_WIDTH);
    // Depth
    sceGuDepthBuffer((void*)(FRAME_SIZE * 2), BUF_WIDTH);   // offset in VRAM

    // cuts off everything that is not in SCR_WIDTH & SCR_HEIGHT 
    sceGuScissor(0, 0, SCR_WIDTH, SCR_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);

    /* VRAM END */

    /* FOR FAIR 3D  */
    
    sceGuOffset(2048 - (SCR_WIDTH / 2), 2048 - (SCR_HEIGHT / 2));
    // Center the screen (480x272) inside the GE's unsigned 4096x4096 coordinate space (2048 = 4096/2)
    // to create a safe margin so off-screen negative vertex coordinates don't underflow and wrap around.
    
    // setting the viewport
    sceGuViewport(2048, 2048, SCR_WIDTH, SCR_HEIGHT);
    
    // 
    sceGuDepthRange(65535, 0); // On PSP Z-buffer is inverted: 65535 — close, 0 — far

    // Idk
    sceGuDepthFunc(GU_GEQUAL);
    sceGuEnable(GU_DEPTH_TEST); // To our depth work
    sceGuFrontFace(GU_CCW);     // Front face counter clockvise
    sceGuEnable(GU_CULL_FACE); // Cut faces we dont see
    /* 3D CONFIG ENDS */

    /* Always clear after use */
    sceGuClearDepth(0);
    sceGuFinish();
    sceGuSync(0, 0);
    /* END */

    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);
}

void ShowModel(const Model& model) {
    if (!model.vertices || !model.indices || model.index_count == 0) return;

    sceGumDrawArray(
        GU_TRIANGLES,   // primitives
        GU_COLOR_8888 | GU_VERTEX_32BITF | GU_INDEX_16BIT | GU_TRANSFORM_3D, // masks
        model.index_count,
        model.indices,
        model.vertices
    );
}

void ShowText(short x, short y, unsigned int color, const char* text, float scale = 1.0f) {
    if (!text) return;

    int len = strlen(text);
    if (len == 0) return;

    FontVertex* vertices = (FontVertex*)sceGuGetMemory(len * 2 * sizeof(FontVertex));

    // our cursor
    short curr_x = x;
    int v_idx = 0;  // Current vertex index in the dynamic vertex buffer
    
    // Go for every char in text
    for (int i = 0; i < len; ++i) {
        unsigned char c = (unsigned char)text[i];
        // if not an ASCII 
        if (c >= 128) continue;

        const Glyph& g = font_glyphs[c];

        // if Glyph width and height not 0
        if (g.w > 0 && g.h > 0) {
            // set first point (top-left)
            short x0 = (short)(curr_x + g.xoff *scale); // mul by scale to adjust
            short y0 = (short)(y + g.yoff *scale);

            // set second point (bot-right)
            short x1 = (short)(x0 + g.w *scale);
            short y1 = (short)(y0 + g.h *scale);

            // Texture-space UV bounding box inside the font atlas (in texels)
            unsigned short u0 = g.x;
            unsigned short v0 = g.y;
            unsigned short u1 = g.x + g.w;
            unsigned short v1 = g.y + g.h;

            // append it to one one quad
            vertices[v_idx++] = { u0, v0, color, x0, y0, 0 };
            vertices[v_idx++] = { u1, v1, color, x1, y1, 0 };
        }

        curr_x += g.xadv;
    }

    if (v_idx == 0) return; // if only spaces or blanks

    /* CONFIG */
    sceGuDisable(GU_DEPTH_TEST); // forget about Z-axis
    sceGuDisable(GU_CULL_FACE); // Render both front and back faces of triangles (no backface culling)
    sceGuEnable(GU_TEXTURE_2D); // set to 2d

    // Configure 32-bit RGBA texture format, no mipmaps (maxmip=0), linear memory layout (swizzle=0)
    sceGuTexMode(GU_PSM_8888, 0, 0, 0);
    // Bind mipmap level 0 of the font atlas in RAM (width, height, stride, pointer)
    sceGuTexImage(0, FONT_ATLAS_W, FONT_ATLAS_H, FONT_ATLAS_W, font_atlas);
    // Multiply texture RGBA by vertex color (allows tinting white glyphs to any color and keeping alpha)
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);

    // texel filter, can be set to LINEAR
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);

    sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    /* CONFIG END */

    sceGuDrawArray(
        GU_SPRITES,
        GU_TEXTURE_16BIT | GU_COLOR_8888 | GU_VERTEX_16BIT | GU_TRANSFORM_2D,
        v_idx,
        nullptr,
        vertices
    );

    /* Remember to set as it was before */
    sceGuDisable(GU_BLEND);
    sceGuDisable(GU_TEXTURE_2D);
    sceGuEnable(GU_CULL_FACE);
    sceGuEnable(GU_DEPTH_TEST);
    /* That`s important, really */
}

// famous wrapper
void begin_frame(){
    sceGuStart(GU_DIRECT, list);
}