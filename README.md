# PSPSetup

A PSP homebrew app that does two things automatically:

1. **Fixes X/O button assignment** — reads current setting. If already US (X=Confirm), leaves it alone. If Japanese (O=Confirm), swaps it to X=Confirm.
2. **Sets XMB wallpaper** — registers `ms0:/PICTURE/bg.jpg` as your XMB background image.

---

## Building (Windows)

**Requirements:** Windows 10/11 with WSL enabled.

1. Double-click `BUILD_WINDOWS.bat`
2. If WSL isn't installed, it will tell you how
3. If PSPSDK isn't installed inside WSL, the script installs it automatically
4. `EBOOT.PBP` will appear in this folder when done

---

## Installing on PSP

1. On your memory stick, create the folder:
   ```
   PSP\GAME\PSPSetup\
   ```
2. Copy `EBOOT.PBP` into that folder

3. Place your wallpaper image at:
   ```
   PICTURE\bg.jpg
   ```
   (480x272 JPG recommended for best results)

4. Launch **PSP Setup** from the PSP's Game menu

---

## What happens when you run it

- Prints status as it runs
- Shows OK or ERROR for each step
- Press **X** to exit when done
- Changes take effect as soon as you return to XMB

---

## Notes

- Compatible with ARK-4 CFW
- Requires kernel mode registry access — runs as user mode but uses sceReg which is available in user mode on CFW
- Safe to run multiple times — button assign check is non-destructive
