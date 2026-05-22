#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspctrl.h>
#include <pspdebug.h>
#include <psputility.h>
#include <pspreg.h>
#include <pspiofilemgr.h>
#include <string.h>
#include <stdio.h>

PSP_MODULE_INFO("Finalizer", 0, 1, 0);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_USER);
PSP_HEAP_SIZE_KB(64);

#define printf pspDebugScreenPrintf

#define WALLPAPER_PATH "ms0:/PICTURE/bg.jpg"

static const unsigned short OWNER_NAME[] = {
    'R','o','y','a','l',' ','G','a','m','e','z', 0x0000
};
#define OWNER_NAME_LEN (sizeof(OWNER_NAME))

int exit_callback(int arg1, int arg2, void *common) {
    sceKernelExitGame();
    return 0;
}

int callback_thread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

void setup_callbacks() {
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if (thid >= 0) sceKernelStartThread(thid, 0, 0);
}

void fix_button_assign() {
    REGHANDLE h, hd;
    int ret;
    printf("Checking button assignment...\n");
    ret = sceRegOpenRegistry(NULL, 1, &h);
    if (ret < 0) { printf("  ERROR: sceRegOpenRegistry failed (0x%08X)\n", ret); return; }
    ret = sceRegOpenCategory(h, "/CONFIG/SYSTEM/XMB", 1, &hd);
    if (ret < 0) { printf("  ERROR: sceRegOpenCategory failed (0x%08X)\n", ret); sceRegCloseRegistry(h); return; }
    unsigned int current = 0, type = 0, size = 4;
    ret = sceRegGetKeyValue(hd, NULL, "button_assign", &type, &current, size, &size);
    if (ret < 0) { printf("  ERROR: sceRegGetKeyValue failed (0x%08X)\n", ret); sceRegCloseCategory(hd); sceRegCloseRegistry(h); return; }
    if (current == 0) {
        printf("  Already X=Confirm. No change needed.\n");
    } else {
        printf("  Swapping to X=Confirm...\n");
        unsigned int new_val = 0;
        ret = sceRegSetKeyValue(hd, "button_assign", &new_val, sizeof(new_val));
        printf("  Button assign: %s\n", ret < 0 ? "ERROR" : "OK");
    }
    sceRegFlushCategory(hd); sceRegCloseCategory(hd); sceRegCloseRegistry(h);
}

void set_wallpaper() {
    REGHANDLE h, hd;
    int ret;
    printf("Setting wallpaper...\n");
    ret = sceRegOpenRegistry(NULL, 1, &h);
    if (ret < 0) { printf("  ERROR (0x%08X)\n", ret); return; }
    ret = sceRegOpenCategory(h, "/CONFIG/PHOTO/SLIDESHOW", 1, &hd);
    if (ret < 0) { printf("  ERROR (0x%08X)\n", ret); sceRegCloseRegistry(h); return; }
    ret = sceRegSetKeyValue(hd, "wallpaper_path_0", WALLPAPER_PATH, strlen(WALLPAPER_PATH) + 1);
    printf("  Wallpaper path: %s\n", ret < 0 ? "ERROR" : "OK");
    sceRegFlushCategory(hd); sceRegCloseCategory(hd);
    ret = sceRegOpenCategory(h, "/CONFIG/DISPLAY", 1, &hd);
    if (ret < 0) { printf("  ERROR (0x%08X)\n", ret); sceRegCloseRegistry(h); return; }
    unsigned int bg_mode = 2;
    ret = sceRegSetKeyValue(hd, "background_category", &bg_mode, sizeof(bg_mode));
    printf("  Background mode: %s\n", ret < 0 ? "ERROR" : "OK");
    sceRegFlushCategory(hd); sceRegCloseCategory(hd); sceRegCloseRegistry(h);
}

void set_username() {
    REGHANDLE h, hd;
    int ret;
    printf("Setting username...\n");
    ret = sceRegOpenRegistry(NULL, 1, &h);
    if (ret < 0) { printf("  ERROR (0x%08X)\n", ret); return; }
    ret = sceRegOpenCategory(h, "/CONFIG/SYSTEM/OWNER_INFO", 1, &hd);
    if (ret < 0) { printf("  ERROR (0x%08X)\n", ret); sceRegCloseRegistry(h); return; }
    ret = sceRegSetKeyValue(hd, "name", OWNER_NAME, OWNER_NAME_LEN);
    printf("  Username: %s\n", ret < 0 ? "ERROR" : "OK");
    sceRegFlushCategory(hd); sceRegCloseCategory(hd); sceRegCloseRegistry(h);
}

static void delete_dir(const char *path) {
    int dd = sceIoDopen(path);
    if (dd < 0) return;
    SceIoDirent entry;
    char filepath[256];
    while (sceIoDread(dd, &entry) > 0) {
        if (strcmp(entry.d_name, ".") == 0 || strcmp(entry.d_name, "..") == 0) continue;
        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry.d_name);
        sceIoRemove(filepath);
    }
    sceIoDclose(dd);
    sceIoRmdir(path);
}

void delete_ark_installers() {
    printf("Cleaning up ARK installers...\n");
    delete_dir("ms0:/PSP/GAME/ARK_cIPL");
    delete_dir("ms0:/PSP/GAME/ARK_Loader");
    printf("  Done. OK\n");
}

int main(void) {
    setup_callbacks();
    pspDebugScreenInit();
    printf("=== Finalizer v1.0 ===\n\n");
    fix_button_assign();
    printf("\n");
    set_wallpaper();
    printf("\n");
    set_username();
    printf("\n");
    delete_ark_installers();
    printf("\nDone! Press X to exit.\n");
    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    while (1) {
        sceCtrlReadBufferPositive(&pad, 1);
        if (pad.Buttons & PSP_CTRL_CROSS) break;
        sceDisplayWaitVblankStart();
    }
    sceKernelExitGame();
    return 0;
}
