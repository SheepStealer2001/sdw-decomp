/*
 * T259 - guessed original name: InputMgr.cpp. SheepD3D.exe .text 0x527a00-0x529f07 plus the COMDAT
 * InputMgr_ScalarDeletingDtor 0x529f10-0x529f3e, .rdata 0x577138-0x57713c (??_7InputMgr), .data 0x57ba04-0x57bbac
 * (its string literals, $SG in the order the compiler meets them).
 * The input manager InputMgr (vtable 0x577138; the one instance g_inputMgr 0x6d6780 is defined, and constructed, in
 * src/engine/draw2d.cpp, T256, through the out-of-line InputMgr::InputMgr defined here, which is why the vtable is
 * first emitted by this object). The three colour lerps just before it (0x527854-0x5279fd) are src/engine/lerp.cpp
 * (T258).
 *
 * The names are descriptive, not recovered (the binary has no symbols); local names were chosen for their /Od stack
 * slots (tools/vc6_locals.py): they are plausible, not recovered. Shapes worth knowing: a logical expression whose VALUE
 * is used (the key edges, the reserved-key test, SaveConfig's result) is written `cond ? 1 : 0`, which is what gives the int temporary with the
 * mov 0 / mov 1 pair (a bare `a && b` stored to a u8 gives a byte temporary instead); local arrays of 8 bytes or more are
 * 8-aligned with their size rounded up to 8, which fixes the path buffers at 512 and the string scratch at 256.
 *
 * The manager owns up to three DirectInput devices (keyboard +0x14, joystick +0x18, mouse +0x1c; device index 1 / 2 / 4)
 * and two binding tables of 16 {device, button} pairs: +0x30 for the pad (joystick or mouse) and +0xb0 for the keyboard;
 * +0x28 points at the one in use. Slots 0..11 are the PS1 buttons InputMgr_Poll turns into the active-low pad word, slots
 * 12..15 the menu directions (keyboard only); InputBindSlot names them. InputMgr_Poll also derives the keyboard edges
 * (Esc, Enter, Pause / P) and the arrow-key menu word with auto-repeat. The tables are saved as
 * "BlackSheep Controller Config" V2.0 blobs, in the
 * registry (HKLM\...\Config\UserConf, KeybConf) or in .BSC files (the DefPConf / DefKConf defaults).
 *
 * The device objects are 0x64 bytes (Keyboard), 0x84 (Joystick) and 0x58 (Mouse): the generated classes, InputDevice
 * and its three subclasses, give `new` those sizes, and this file declares their constructors (Keyboard_Construct
 * 0x4072ac, Joystick_Construct 0x406db0, Mouse_Construct 0x409e90).
 */
/* BYTES: cast, dead-code, inline. */
/* BYTES(inline): InputDevice::SetAllowHeld, Keyboard::SetKeyXNeg .. (member-macro inlines): source-only inline: no symbol of its own, /Ob1 expands it as the original */
/* BYTES(cast): reserved-key macro (DIK_PAUSE / P / RETURN / ESCAPE): written cond ? 1 : 0 so the value is an int temporary (mov 0 / mov 1) and the operand is re-read per test */
#include "sdw_types.h"
#include "sdw_enums.h"

#include "../sdk/windef.h"
#include "../sdk/crt.h"
#include "../sdk/dinput.h"
#include "../sdk/win32.h"

class D3DApp;
class InputDevice;

/* Source-only inlines (no symbol; /Ob1 expands them): the device's "held counts as pressed" flag and the keyboard's six
 * axis keys (Keyboard.axisKeys: X-, X+, Y-, Y+, Z-, Z+, see
 * src/app/input_device.cpp). */
#define SDW_MEMBERS_InputDevice \
    void SetAllowHeld(u8 on)    \
    {                           \
        allowHeld = on;         \
    }
#define SDW_MEMBERS_Keyboard                                                \
    Keyboard(u16 buttonCount, D3DApp *app, HRESULT *result); /* 0x4072ac */ \
    void SetKeyXNeg(u16 code)                                               \
    {                                                                       \
        axisKeys[KB_AXIS_X_NEG] = code;                                     \
    }                                                                       \
    void SetKeyXPos(u16 code)                                               \
    {                                                                       \
        axisKeys[KB_AXIS_X_POS] = code;                                     \
    }                                                                       \
    void SetKeyYNeg(u16 code)                                               \
    {                                                                       \
        axisKeys[KB_AXIS_Y_NEG] = code;                                     \
    }                                                                       \
    void SetKeyYPos(u16 code)                                               \
    {                                                                       \
        axisKeys[KB_AXIS_Y_POS] = code;                                     \
    }
#define SDW_MEMBERS_Joystick Joystick(u16 buttonCount, D3DApp *app, HRESULT *result); /* 0x406db0 */
#define SDW_MEMBERS_Mouse Mouse(u16 buttonCount, D3DApp *app, HRESULT *result);       /* 0x409e90 */
#define SDW_MEMBERS_InputMgr InputMgr();                                              /* 0x527a00 */
#include "sdw_classes.h"

/* ---- engine ---- */
#include "bs_io.h"
long Reg_CreateSubKey(HKEY *out, const char *name);                                         /* 0x55f8dc */
void Reg_CloseKey(HKEY *key);                                                               /* 0x55f93a */
unsigned long Reg_ReadBinary(HKEY key, const char *name, void *buf, unsigned long bufSize); /* 0x55fb41 */
u8 Reg_WriteBinary(HKEY key, const char *name, const void *data, unsigned long size);       /* 0x55fbb2 */

#include "draw2d.h"
extern s32 g_dtRawMs; /* 0x71b2d8 */

/* A keyboard key the bindings may not take (Pause, P, Enter, Esc). Written as a conditional expression: its value is
 * materialised into an int temporary (the mov 0 / mov 1 pair in all four users), and the operand is re-read per test. */
#define DIK_IS_RESERVED(code) \
    ((code) == DIK_PAUSE || (code) == DIK_P || (code) == DIK_RETURN || (code) == DIK_ESCAPE ? 1 : 0)

/* ======================================================================== InputMgr, 0x527a00-0x529f3f */

/* 0x527a00 */
InputMgr::InputMgr()
{
    keyboard = 0;
    joystick = 0;
    device3 = 0;
    current = 0;
    preferred = 0;
    bindings = 0;
    axisY = 0x80;
    axisX = 0x80;
    padBits = 0xffff;
    pausePressed = 0;
    escPressed = 0;
    enterPressed = 0;
    menuNav = PAD_ALL_RELEASED;
    pausePrev = 0;
    escPrev = 0;
    enterPrev = 0;
    lastNav = 0;
    navRepeatMs = 500;
}

/* 0x527abc - deletes the three devices (not zeroed). */
InputMgr::~InputMgr()
{
    if (keyboard)
        delete keyboard;
    if (joystick)
        delete joystick;
    if (device3)
        delete device3;
}

/* 0x527b74 - creates the keyboard always, the joystick if wanted & 2, the mouse if wanted & 4; each is acquired, and the
 * keyboard becomes the current and preferred device. A device whose constructor failed stops the chain. Returns the
 * present mask (also stored at +0x20). */
u8 InputMgr::CreateDevices(u8 wanted)
{
    u8 present = 0;
    HRESULT hr;
    /* cast kept: the size-only device shell stands for the generated InputDevice (see the header comment) */
    keyboard = new Keyboard(0x100, g_pD3DAppMain, &hr);
    if (hr >= 0) {
        keyboard->SetAcquired(1);
        preferred = keyboard;
        current = preferred;
        present |= INPUTDEV_MASK_KEYBOARD;
        if (wanted & INPUTDEV_MASK_JOYSTICK) {
            /* cast kept: the size-only device shell stands for the generated InputDevice (see the header comment) */
            joystick = new Joystick(12, g_pD3DAppMain, &hr);
            if (hr >= 0) {
                joystick->SetAcquired(1);
                present |= INPUTDEV_MASK_JOYSTICK;
            }
        }
        if (wanted & INPUTDEV_MASK_MOUSE) {
            /* cast kept: the size-only device shell stands for the generated InputDevice (see the header comment) */
            device3 = new Mouse(12, g_pD3DAppMain, &hr);
            if (hr >= 0) {
                device3->SetAcquired(1);
                present |= INPUTDEV_MASK_MOUSE;
            }
        }
    }
    presentMask = present;
    return present;
}

/* 0x527ced - 1 if the device is digital (its axes become d-pad bits) or absent. */
u8 InputMgr::IsDeviceDigital(u8 devIdx)
{
    InputDevice *dev = DeviceFromIndex(devIdx);
    if (dev)
        return dev->isDigital;
    return 1;
}

/* 0x527d1b - updates one device; 1 if that succeeded. */
u8 InputMgr::PollDevice(u8 devIdx)
{
    InputDevice *dev = DeviceFromIndex(devIdx);
    if (dev && dev->Update() >= 0)
        return 1;
    return 0;
}

/* 0x527d54 - makes a device current and preferred, carries the old current device's allowHeld flag over, and points the
 * bindings at the keyboard or the pad table. */
u8 InputMgr::SelectDevice(u8 devIdx)
{
    InputDevice *dev = DeviceFromIndex(devIdx);
    if (dev) {
        u8 held = current->allowHeld;
        preferred = dev;
        current = dev;
        SetActiveFlags(held);
        if (current == keyboard)
            bindings = keyTable;
        else
            bindings = padTable;
        return 1;
    }
    return 0;
}

/* 0x527dd2 */
u8 InputMgr::GetCurrentDeviceIdx()
{
    return IndexFromDevice(current);
}

/* 0x527dec */
void InputMgr::SetAxisCalibration(float sx, float sy, float sz, float ox, float oy, float oz)
{
    current->SetAxisCalibration(sx, sy, sz, ox, oy, oz);
}

/* 0x527e1c - binds pad slot 0..11 of the current table to a button of a device; a reserved key on the keyboard and a
 * code past the device's button count are refused. */
u8 InputMgr::BindAction(u8 slot, u8 devIdx, u16 code)
{
    InputDevice *dev = DeviceFromIndex(devIdx);
    if (dev) {
        if ((!DIK_IS_RESERVED(code) || dev != keyboard) && slot < 12 && code < dev->buttonCount) {
            bindings[slot].dev = dev;
            bindings[slot].code = code;
            return 1;
        }
    }
    return 0;
}

/* 0x527ecd - binds menu direction slot 12..15 (up, right, down, left) to a keyboard key in both tables and in the
 * keyboard's axis keys. */
u8 InputMgr::BindMenuKey(u8 slot, u16 code)
{
    u8 ok = 0;
    if (slot > 11 && slot < 16 && keyboard) {
        if (!DIK_IS_RESERVED(code)) {
            keyTable[slot].dev = keyboard;
            padTable[slot].dev = keyTable[slot].dev;
            keyTable[slot].code = code;
            padTable[slot].code = code;
            switch (slot) {
                case INPUT_BIND_MENU_UP:
                    keyboard->SetKeyYNeg(code);
                    break;
                case INPUT_BIND_MENU_RIGHT:
                    keyboard->SetKeyXPos(code);
                    break;
                case INPUT_BIND_MENU_DOWN:
                    keyboard->SetKeyYPos(code);
                    break;
                case INPUT_BIND_MENU_LEFT:
                    keyboard->SetKeyXNeg(code);
                    break;
            }
            ok = 1;
        }
    }
    return ok;
}

/* 0x528026 */
u8 InputMgr::GetBinding(u8 slot, InputBindingIdx *out)
{
    u8 ok = 0;
    if (slot < 16) {
        out->devIdx = IndexFromDevice(bindings[slot].dev);
        out->code = bindings[slot].code;
        ok = 1;
    }
    return ok;
}

/* 0x52807a - deletes and reconstructs one device; the current / preferred pointers follow it if the new one is good. */
long InputMgr::RecreateDevice(u8 devIdx)
{
    InputDevice *old = DeviceFromIndex(devIdx);
    HRESULT hr;
    InputDevice *created;
    switch (devIdx) {
        case INPUTDEV_MASK_KEYBOARD:
            delete keyboard;
            keyboard = 0;
            /* cast kept: the size-only device shell stands for the generated InputDevice (see the header comment) */
            keyboard = new Keyboard(0x100, g_pD3DAppMain, &hr);
            if (keyboard)
                created = keyboard;
            else
                return E_FAIL;
            break;
        case INPUTDEV_MASK_JOYSTICK:
            delete joystick;
            joystick = 0;
            /* cast kept: the size-only device shell stands for the generated InputDevice (see the header comment) */
            joystick = new Joystick(12, g_pD3DAppMain, &hr);
            if (joystick)
                created = joystick;
            else
                return E_FAIL;
            break;
        case INPUTDEV_MASK_MOUSE:
            delete device3;
            device3 = 0;
            /* cast kept: the size-only device shell stands for the generated InputDevice (see the header comment) */
            device3 = new Mouse(12, g_pD3DAppMain, &hr);
            if (device3)
                created = device3;
            else
                return E_FAIL;
            break;
        default:
            return E_FAIL;
    }
    if (hr >= 0) {
        if (current == old)
            current = created;
        if (preferred == old)
            preferred = created;
    }
    return hr;
}

/* 0x5282b8 - clears the key edges and marks the keys as held, so a key already down does not fire. */
void InputMgr::ResetEdges()
{
    pausePressed = 0;
    enterPressed = 0;
    escPressed = 0;
    menuNav = PAD_ALL_RELEASED;
    pausePrev = 1;
    enterPrev = 1;
    escPrev = 1;
    lastNav = 0;
    navRepeatMs = 500;
}

/* 0x528315 - (un)acquires every present device; S_OK if any succeeded. */
long InputMgr::SetAcquiredAll(u8 acquire)
{
    HRESULT hr = E_FAIL;
    if ((presentMask & INPUTDEV_MASK_KEYBOARD) && keyboard->SetAcquired(acquire) >= 0)
        hr = S_OK;
    if ((presentMask & INPUTDEV_MASK_JOYSTICK) && joystick->SetAcquired(acquire) >= 0)
        hr = S_OK;
    if ((presentMask & INPUTDEV_MASK_MOUSE) && device3->SetAcquired(acquire) >= 0)
        hr = S_OK;
    return hr;
}

/* 0x5283be */
long InputMgr::SetAcquired(u8 devIdx, u8 acquire)
{
    HRESULT hr = E_FAIL;
    switch (devIdx) {
        case INPUTDEV_MASK_KEYBOARD:
            if ((presentMask & INPUTDEV_MASK_KEYBOARD) && keyboard->SetAcquired(acquire) >= 0)
                hr = S_OK;
            break;
        case INPUTDEV_MASK_JOYSTICK:
            if ((presentMask & INPUTDEV_MASK_JOYSTICK) && joystick->SetAcquired(acquire) >= 0)
                hr = S_OK;
            break;
        case INPUTDEV_MASK_MOUSE:
            if ((presentMask & INPUTDEV_MASK_MOUSE) && device3->SetAcquired(acquire) >= 0)
                hr = S_OK;
            break;
    }
    return hr;
}

/* 0x528486 - allowHeld per device: the keyboard 1 when current; the joystick 1 (and the keyboard 0) when current; the
 * mouse 1; then the current device's flag = the argument. */
void InputMgr::SetActiveFlags(u8 flag)
{
    if ((presentMask & INPUTDEV_MASK_KEYBOARD) && current == keyboard)
        keyboard->SetAllowHeld(1);
    if ((presentMask & INPUTDEV_MASK_JOYSTICK) && current == joystick) {
        joystick->SetAllowHeld(1);
        keyboard->SetAllowHeld(0);
    }
    if (presentMask & INPUTDEV_MASK_MOUSE)
        device3->SetAllowHeld(1);
    current->SetAllowHeld(flag);
}

/* 0x52852d - builds the PS1-layout, active-low pad word from the current device: the stick (or, for a digital device, the
 * d-pad bits from the axes' signs), then the twelve bound buttons; then the keyboard edges and the arrow-key menu word
 * with its auto-repeat (first frame, then after 500 ms, then every 125 ms, timed by g_dtRawMs). */
/* BYTES(cast): cond ? 1 : 0: the original materialises an int temporary (mov 0 / mov 1), not a byte */
long InputMgr::Poll()
{
    u16 nav;
    if (CheckDevices() == 1) {
        padBits = 0xffff;
        if (current->isDigital == 0) {
            axisX = (u8)(s32)(current->axisX * 128.0f) + 0x80;
            axisY = (u8)(s32)(current->axisY * 128.0f) + 0x80;
        } else {
            axisX = 0x80;
            axisY = 0x80;
            if (current->axisX < 0.0f)
                padBits &= (u16)~PAD_LEFT;
            else if (current->axisX > 0.0f)
                padBits &= (u16)~PAD_RIGHT;
            if (current->axisY < 0.0f)
                padBits &= (u16)~PAD_UP;
            else if (current->axisY > 0.0f)
                padBits &= (u16)~PAD_DOWN;
        }
        if (bindings[0].dev->buttonCount) {
            if (bindings[INPUT_BIND_TRIANGLE].dev->buttonPressed[bindings[INPUT_BIND_TRIANGLE].code] == 1)
                padBits &= (u16)~PAD_TRIANGLE; /* Triangle */
            if (bindings[INPUT_BIND_CIRCLE].dev->buttonPressed[bindings[INPUT_BIND_CIRCLE].code] == 1)
                padBits &= (u16)~PAD_CIRCLE; /* Circle */
            if (bindings[INPUT_BIND_CROSS].dev->buttonPressed[bindings[INPUT_BIND_CROSS].code] == 1)
                padBits &= (u16)~PAD_CROSS; /* Cross */
            if (bindings[INPUT_BIND_SQUARE].dev->buttonPressed[bindings[INPUT_BIND_SQUARE].code] == 1)
                padBits &= (u16)~PAD_SQUARE; /* Square */
            if (bindings[INPUT_BIND_L1].dev->buttonPressed[bindings[INPUT_BIND_L1].code] == 1)
                padBits &= (u16)~PAD_L1; /* L1 */
            if (bindings[INPUT_BIND_R1].dev->buttonPressed[bindings[INPUT_BIND_R1].code] == 1)
                padBits &= (u16)~PAD_R1; /* R1 */
            if (bindings[INPUT_BIND_L2].dev->buttonPressed[bindings[INPUT_BIND_L2].code] == 1)
                padBits &= (u16)~PAD_L2; /* L2 */
            if (bindings[INPUT_BIND_R2].dev->buttonPressed[bindings[INPUT_BIND_R2].code] == 1)
                padBits &= (u16)~PAD_R2; /* R2 */
            if (bindings[INPUT_BIND_SELECT].dev->buttonPressed[bindings[INPUT_BIND_SELECT].code] == 1)
                padBits &= (u16)~PAD_SELECT; /* Select */
            if (bindings[INPUT_BIND_START].dev->buttonPressed[bindings[INPUT_BIND_START].code] == 1)
                padBits &= (u16)~PAD_START; /* Start */
            if (bindings[INPUT_BIND_L3].dev->buttonPressed[bindings[INPUT_BIND_L3].code] == 1)
                padBits &= (u16)~PAD_L3; /* L3 */
            if (bindings[INPUT_BIND_R3].dev->buttonPressed[bindings[INPUT_BIND_R3].code] == 1)
                padBits &= (u16)~PAD_R3; /* R3 */
        }
        nav = PAD_ALL_RELEASED;
        escPressed = !escPrev && keyboard->buttonPressed[DIK_ESCAPE] ? 1 : 0;
        enterPressed = !enterPrev && keyboard->buttonPressed[DIK_RETURN] ? 1 : 0;
        pausePressed = !pausePrev && (keyboard->buttonPressed[DIK_PAUSE] || keyboard->buttonPressed[DIK_P]) ? 1 : 0;
        if (enterPressed == 1)
            enterPressed = 1;
        escPrev = keyboard->buttonPressed[DIK_ESCAPE];
        enterPrev = keyboard->buttonPressed[DIK_RETURN];
        pausePrev = keyboard->buttonPressed[DIK_PAUSE] || keyboard->buttonPressed[DIK_P] ? 1 : 0;
        if (keyboard->buttonPressed[DIK_UP] == 1)
            nav = (u16)~PAD_UP;
        else if (keyboard->buttonPressed[DIK_DOWN] == 1)
            nav = (u16)~PAD_DOWN;
        else if (keyboard->buttonPressed[DIK_RIGHT] == 1)
            nav = (u16)~PAD_RIGHT;
        else if (keyboard->buttonPressed[DIK_LEFT] == 1)
            nav = (u16)~PAD_LEFT;
        if (nav == lastNav) {
            navRepeatMs -= g_dtRawMs;
            if (navRepeatMs <= 0) {
                navRepeatMs = 125;
                menuNav = nav;
            } else {
                menuNav = PAD_ALL_RELEASED;
            }
        } else {
            navRepeatMs = 500;
            lastNav = nav;
            menuNav = nav;
        }
        return S_OK;
    }
    return E_FAIL;
}

/* 0x528b0e - the first button reading 1 on the keyboard, then the joystick, then the mouse; S_OK if one was found, S_FALSE
 * if none, E_FAIL if the devices could not be polled. No callers. */
long InputMgr::GetAnyPressedRaw(InputBinding *out)
{
    HRESULT hr = E_FAIL;
    u8 done = 0;
    u16 btn = 0;
    out->dev = 0;
    out->code = 0;
    if (CheckDevices() == 1) {
        while (btn < keyboard->buttonCount && !done) {
            if (keyboard->buttonPressed[btn] == 1) {
                out->dev = keyboard;
                out->code = btn;
                done = 1;
            }
            btn++;
        }
        btn = 0;
        while (btn < joystick->buttonCount && !done) {
            if (joystick->buttonPressed[btn] == 1) {
                out->dev = joystick;
                out->code = btn;
                done = 1;
            }
            btn++;
        }
        btn = 0;
        while (btn < device3->buttonCount && !done) {
            if (device3->buttonPressed[btn] == 1) {
                out->dev = device3;
                out->code = btn;
                done = 1;
            }
            btn++;
        }
        if (done == 1)
            hr = S_OK;
        else
            hr = S_FALSE;
    }
    return hr;
}

/* 0x528c85 - as GetAnyPressedRaw, reporting a device index; the controls menu uses it to capture a new binding. */
long InputMgr::GetAnyPressed(InputBindingIdx *out)
{
    HRESULT hr = E_FAIL;
    u8 done = 0;
    u16 btn = 0;
    out->devIdx = INPUTDEV_NONE;
    out->code = 0;
    if (CheckDevices() == 1) {
        while (btn < keyboard->buttonCount && !done) {
            if (keyboard->buttonPressed[btn] == 1) {
                out->devIdx = IndexFromDevice(keyboard);
                out->code = btn;
                done = 1;
            }
            btn++;
        }
        btn = 0;
        while (btn < joystick->buttonCount && !done) {
            if (joystick->buttonPressed[btn] == 1) {
                out->devIdx = IndexFromDevice(joystick);
                out->code = btn;
                done = 1;
            }
            btn++;
        }
        btn = 0;
        while (btn < device3->buttonCount && !done) {
            if (device3->buttonPressed[btn] == 1) {
                out->devIdx = IndexFromDevice(device3);
                out->code = btn;
                done = 1;
            }
            btn++;
        }
        if (done == 1)
            hr = S_OK;
        else
            hr = S_FALSE;
    }
    return hr;
}

/* 0x528e14 - the keyboard table from the registry (KeybConf) or else the default file (DefKConf), the pad table from the
 * registry (UserConf) or else the default file (DefPConf); then re-selects the current device. */
u8 InputMgr::LoadConfig(const char *dir)
{
    u8 ok = 1;
    u8 devIdx = GetCurrentDeviceIdx();
    if (LoadBindingTable(1, dir, "KeybConf") == 1) {
        if (LoadBindingTable(1, dir, "UserConf") == 1)
            SelectDevice(devIdx);
        else if (LoadBindingTable(0, dir, "DefPConf") == 1)
            SelectDevice(devIdx);
        else
            ok = 0;
    } else {
        if (LoadBindingTable(0, dir, "DefKConf") == 1 && LoadBindingTable(0, dir, "DefPConf") == 1)
            SelectDevice(devIdx);
        else
            ok = 0;
    }
    return ok;
}

/* 0x528ef2 - the pad table (with the preferred device's calibration) as UserConf, the keyboard table as KeybConf, both in
 * the registry. */
u8 InputMgr::SaveConfig(const char *dir)
{
    return SaveBindingTable(1, dir, "UserConf", padTable, preferred) &&
                   SaveBindingTable(1, dir, "KeybConf", keyTable, 0)
               ? 1
               : 0;
}

/* 0x528f64 - reads a binding blob from the registry (Config\<name>, 256 bytes) or from the file <dir><name>.BSC and
 * applies it: "BlackSheep Controller Config" "V2.0", optionally "MASTER" (device index, three axis scales, three offsets:
 * this blob is the pad table, and that device is selected and calibrated), "REMAP" (u16 count <= 12, then count
 * {u8 devIdx; u16 code} records for slots 0..), "REMAP_DIR" (four u16 key codes for the menu slots 12..15, bound through
 * BindMenuKey). */
u8 InputMgr::LoadBindingTable(u8 fromRegistry, const char *dir, const char *name)
{
    float *scale;
    u8 *p;
    HKEY rkey;
    FILE *strm;
    u8 result;
    u32 fsize;
    char fileName[512];
    u8 *cfgBlob;
    InputBinding *table;
    InputBindingIdx *remaps;
    float *bias;
    u8 devIdx;
    u16 remapCount;
    u16 k;
    u16 m;

    result = 0;
    if (!fromRegistry) {
        strcpy(fileName, dir);
        strcat(fileName, name);
        strcat(fileName, ".BSC");
        strm = fopen(fileName, "rb");
        if (strm) {
            fsize = Bs_FileSize(strm);
            cfgBlob = new u8[fsize];
            fread(cfgBlob, 1, fsize, strm);
            fclose(strm);
            result = 1;
        }
    } else {
        if (Reg_CreateSubKey(&rkey, "Config") == ERROR_SUCCESS) {
            cfgBlob = new u8[0x100];
            if (Reg_ReadBinary(rkey, name, cfgBlob, 0x100) == 0x100) {
                Reg_CloseKey(&rkey);
                result = 1;
            }
        }
    }
    if (result == 1) {
        /* cast kept: the config blob is raw bytes; strncmp takes char * (these two tests) */
        if (strncmp((char *)cfgBlob, "BlackSheep Controller Config", strlen("BlackSheep Controller Config")) ||
            strncmp((char *)cfgBlob + strlen("BlackSheep Controller Config"), "V2.0", strlen("V2.0"))) {
            delete cfgBlob;
        } else {
            devIdx = 0;
            table = keyTable;
            p = cfgBlob + (strlen("BlackSheep Controller Config") + strlen("V2.0"));
            /* cast kept: the config blob is raw bytes; strncmp takes char * */
            if (strncmp((char *)p, "MASTER", strlen("MASTER")) == 0) {
                table = padTable;
                p += strlen("MASTER");
                devIdx = *p;
                p++;
                SelectDevice(devIdx);
                scale = (float *)p; /* cast kept: the blob holds the three axis scales here */
                p += 12;
                bias = (float *)p; /* cast kept: the blob holds the three axis offsets here */
                p += 12;
                SetAxisCalibration(scale[0], scale[1], scale[2], bias[0], bias[1], bias[2]);
            }
            /* cast kept: the config blob is raw bytes; strncmp takes char * */
            if (strncmp((char *)p, "REMAP", strlen("REMAP"))) {
                delete cfgBlob;
                result = 0;
            } else {
                p += strlen("REMAP");
                remapCount = *(u16 *)p; /* cast kept: the blob holds the u16 remap count here */
                p += 2;
                if (remapCount > 12) {
                    delete cfgBlob;
                    result = 0;
                } else {
                    remaps = (InputBindingIdx *)p; /* cast kept: the blob holds the {devIdx, code} records here */
                    for (k = 0; k < remapCount; k++) {
                        table[k].dev = DeviceFromIndex(remaps[k].devIdx);
                        table[k].code = remaps[k].code;
                    }
                    p += remapCount * 4;
                    /* cast kept: the config blob is raw bytes; strncmp takes char * */
                    if (strncmp((char *)p, "REMAP_DIR", strlen("REMAP_DIR"))) {
                        delete cfgBlob;
                        return 0;
                    }
                    p += strlen("REMAP_DIR");
                    for (m = 12; m < 16; m++) {
                        table[m].code = *(u16 *)p; /* cast kept: the blob holds the four u16 menu key codes here */
                        p += 2;
                    }
                    BindMenuKey(INPUT_BIND_MENU_UP, table[INPUT_BIND_MENU_UP].code);
                    BindMenuKey(INPUT_BIND_MENU_RIGHT, table[INPUT_BIND_MENU_RIGHT].code);
                    BindMenuKey(INPUT_BIND_MENU_DOWN, table[INPUT_BIND_MENU_DOWN].code);
                    BindMenuKey(INPUT_BIND_MENU_LEFT, table[INPUT_BIND_MENU_LEFT].code);
                    delete cfgBlob;
                    result = 1;
                }
            }
        }
    }
    return result;
}

/* Appends len bytes to the blob being built (SaveBindingTable). */
#define PUT(src, n) (size = (n), memcpy(dst, (src), size), dst += size, written += size)
#define PUT_STR(s) (strcpy(text, (s)), size = strlen(text), memcpy(dst, text, size), dst += size, written += size)

/* 0x529510 - writes a binding table in the format LoadBindingTable reads, with a MASTER section when a device is given.
 * The REMAP_DIR codes always come from the pad table. The file variant opens the file in text mode ("w"). */
/* BYTES(dead-code): size and text are never used and dst is stored and never read: they occupy slots the original frame has */
u8 InputMgr::SaveBindingTable(u8 toRegistry, const char *dir, const char *name, InputBinding *table,
                              InputDevice *master)
{
    Vec3f scale;
    u8 blob[256];
    u8 size;
    u8 written;
    u16 nRemap;
    u16 d;
    u16 slot;
    char text[256];
    u8 devIdx;
    u8 *dst;
    Vec3f bias;
    InputBindingIdx remaps[12];

    if (master) {
        devIdx = IndexFromDevice(master);
        if (devIdx == INPUTDEV_NONE)
            return 0;
        master->GetAxisCalibration(&scale.x, &scale.y, &scale.z, &bias.x, &bias.y, &bias.z);
    }
    nRemap = 12;
    for (slot = 0; slot < 12; slot++) {
        u8 idx = IndexFromDevice(table[slot].dev);
        if (idx == INPUTDEV_NONE)
            return 0;
        remaps[slot].devIdx = idx;
        remaps[slot].code = table[slot].code;
    }
    written = 0;
    dst = blob;
    PUT_STR("BlackSheep Controller Config");
    PUT_STR("V2.0");
    if (master) {
        PUT_STR("MASTER");
        PUT(&devIdx, 1);
        PUT(&scale, 12);
        PUT(&bias, 12);
    }
    PUT_STR("REMAP");
    PUT(&nRemap, 2);
    PUT(remaps, 0x30);
    PUT_STR("REMAP_DIR");
    for (d = 12; d < 16; d++)
        PUT(&padTable[d].code, 2);
    if (!toRegistry) {
        char path[512];
        FILE *f;
        strcpy(path, dir);
        strcat(path, name);
        strcat(path, ".BSC");
        f = fopen(path, "w");
        if (!f)
            return 0;
        fwrite(blob, 1, written, f);
        fclose(f);
        return 1;
    } else {
        HKEY key;
        if (Reg_CreateSubKey(&key, "Config") == ERROR_SUCCESS && Reg_WriteBinary(key, name, blob, 0x100) == 1) {
            Reg_CloseKey(&key);
            return 1;
        }
        return 0;
    }
}

/* 0x529afb - 0 the current device, 1 the keyboard, 2 the joystick, 4 the mouse, else null. */
InputDevice *InputMgr::DeviceFromIndex(u8 devIdx)
{
    InputDevice *dev;
    switch (devIdx) {
        case INPUTDEV_MASK_KEYBOARD:
            dev = keyboard;
            break;
        case INPUTDEV_MASK_JOYSTICK:
            dev = joystick;
            break;
        case INPUTDEV_MASK_MOUSE:
            dev = device3;
            break;
        case INPUTDEV_MASK_NONE:
            dev = current;
            break;
        default:
            dev = 0;
            break;
    }
    return dev;
}

/* 0x529b6b - 1 / 2 / 4, or 0xff. */
u8 InputMgr::IndexFromDevice(InputDevice *dev)
{
    u8 idx;
    if (dev == keyboard)
        idx = INPUTDEV_MASK_KEYBOARD;
    else if (joystick && dev == joystick)
        idx = INPUTDEV_MASK_JOYSTICK;
    else if (dev == device3)
        idx = INPUTDEV_MASK_MOUSE;
    else
        idx = INPUTDEV_NONE;
    return idx;
}

/* 0x529bbd - the slot 0..15 of the current table holding that binding, 16 if none. */
u8 InputMgr::FindBinding(const InputBinding *b)
{
    u8 match = 0;
    u8 slot = 0;
    while (slot < 16 && !match) {
        if (bindings[slot].dev == b->dev && bindings[slot].code == b->code)
            match = 1;
        else
            slot++;
    }
    return slot;
}

/* 0x529c27 */
u8 InputMgr::FindBindingByIndex(const InputBindingIdx *b)
{
    u8 match = 0;
    u8 slot = 0;
    InputDevice *target = DeviceFromIndex(b->devIdx);
    while (slot < 16 && !match) {
        if (bindings[slot].dev == target && bindings[slot].code == b->code)
            match = 1;
        else
            slot++;
    }
    return slot;
}

/* 0x529ca0 - a keyboard binding to Pause, P, Enter or Esc. */
u8 InputMgr::IsReservedKeyBinding(const InputBinding *b)
{
    u8 reserved = 0;
    if (b->dev == keyboard)
        reserved = DIK_IS_RESERVED(b->code);
    return reserved;
}

/* 0x529d0c - the same by device index (1 = keyboard). */
u8 InputMgr::IsReservedKey(const InputBindingIdx *b)
{
    u8 reserved = 0;
    if (b->devIdx == INPUTDEV_MASK_KEYBOARD)
        reserved = DIK_IS_RESERVED(b->code);
    return reserved;
}

/* 0x529d75 - the keyboard's name for a key, "" if it has none. outSize is not used. */
/* BYTES(dead-code): ofs is copied and never read: it is the original's -4 slot */
void InputMgr::Input_GetKeyName(u32 diObjOffset, char *out, u32 outSize)
{
    u32 ofs = diObjOffset; /* copied and never read (the original's -4 slot) */
    if (keyboard->GetObjectName(diObjOffset, out) == 0)
        *out = 0;
}

/* 0x529da7 - re-acquires when the current device is acquired, polls the keyboard (failure: 0), the joystick and the
 * mouse; a pad device that stopped answering hands over to the keyboard table, and the preferred device is taken back
 * when it answers again. */
u8 InputMgr::CheckDevices()
{
    if (current->acquired == 1)
        SetAcquiredAll(1);
    if (!PollDevice(INPUTDEV_MASK_KEYBOARD))
        return 0;
    if ((presentMask & INPUTDEV_MASK_JOYSTICK) && !PollDevice(INPUTDEV_MASK_JOYSTICK)) {
        if (current == joystick) {
            current = keyboard;
            bindings = keyTable;
        }
    } else if ((presentMask & INPUTDEV_MASK_MOUSE) && !PollDevice(INPUTDEV_MASK_MOUSE)) {
        if (current == device3) {
            current = keyboard;
            bindings = keyTable;
        }
    } else if (preferred != current) {
        current = preferred;
        bindings = padTable;
    }
    return 1;
}

/* 0x529e9f - whether any of the 16 current bindings uses that device. */
u8 InputMgr::DeviceHasBindings(u8 devIdx)
{
    u8 match = 0;
    u16 i = 0;
    InputDevice *target = DeviceFromIndex(devIdx);
    if (target) {
        while (i < 16 && !match) {
            if (bindings[i++].dev == target)
                match = 1;
        }
    }
    return match;
}
