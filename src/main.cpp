#include<pspkernel.h>
#include <pspdebug.h>
#include"graphics.h"
#include"model_manager.h"
#include"input.h"

// Module init
PSP_MODULE_INFO("Never Born",0,1,1); // game | name, usermode, major_version, minor_version
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER | PSP_THREAD_ATTR_VFPU);

// APP VARIABLES
volatile bool running = true;

int exit_callback(int arg1, int arg2, void* common){
    running = false;
    return 0;
}

int callback_thread(SceSize args, void* argp){
    // UIDs are used to describe many different kernel objects. 
    SceUID cbid = sceKernelCreateCallback("exit_callback", exit_callback, nullptr); 

    // registering an thread as callback
    sceKernelRegisterExitCallback(cbid);

    // sleep for cb
    sceKernelSleepThreadCB();
    return 0;
}

int main(){
    // name, func, priority, stacksize, common attribs, option
    SceUID thid = sceKernelCreateThread("callback_thread", callback_thread, 0x11, 0xFA0, 0, nullptr);
    if(thid >= 0) sceKernelStartThread(thid, 0, nullptr);

    // inits from my lib
    input_init();
    init_gu();

    /* CHANGE */
    float angle = 0.0f;
    Model model = LoadModel("monke.obj");

    /* INPUT DATA */
    SceCtrlData pad;
    unsigned int old_buttons = 0; // db for btn
    
    /* DEL */
    bool show = true;
    int x = 100;
    int y = 100;
    /* DEL */

    /* MAIN LOOP */
    while(running){
        begin_frame(); // custom wrapper

        // Enable control read
        sceCtrlReadBufferPositive(&pad, 1);

        // Fill with color
        sceGuClearColor(0xFF221144); // BACK COLOR
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT); // clear

        // Lens
        sceGumMatrixMode(GU_PROJECTION);
        sceGumLoadIdentity();
        sceGumPerspective(75.0f, 16.0f / 9.0f, 0.5f, 100.0f);

        // Camera
        sceGumMatrixMode(GU_VIEW);
        sceGumLoadIdentity();
        ScePspFVector3 p = {0.0f,0.0f,-1.0f}; // pos of camera
        sceGumTranslate(&p);        
        
        /* CODE HERE */
        ScePspFVector3 pos = { 0.0f, 0.0f, -2.5f }; // pos of model
        sceGumMatrixMode(GU_MODEL); // model matrix
        sceGumLoadIdentity();   // loading

        sceGumTranslate(&pos);  // move
        
        sceGumRotateY(angle);
        
 
        // If pressed X or O
        if(pad.Buttons & PSP_CTRL_CROSS){
            angle += 0.03f;
        }
        if(pad.Buttons & PSP_CTRL_CIRCLE){
            angle -= 0.03f;
        }

        // DB
        unsigned int pressed = pad.Buttons & ~old_buttons;
        // []
        if (pressed & PSP_CTRL_SQUARE) {
            show = !show;
        }
        if(show) ShowModel(model);
        old_buttons = pad.Buttons;

        ShowText(x, y, 0xFFFFFFFF, "NEVER BORN", 0.7f);
        
        /* END */

        sceGuFinish();
        sceGuSync(0, 0);

        sceDisplayWaitVblankStart(); // Wait for vertical blank start
        sceGuSwapBuffers(); // swap
    }

    UnloadModel(model); // Remember to unload properly
    sceGuTerm();    // Always term! 
    sceKernelExitGame();    // save exit
}