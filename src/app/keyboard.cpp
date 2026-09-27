/*
 * Object T006 (data/tu_map.json), guessed original file Keyboard.cpp.
 *   .text 0x4071b0-0x4077b4, then the COMDAT ??_GKeyboard 0x4077c0-0x4077ee
 *   .rdata 0x574314-0x574320 (the vtable)       .data 0x579418-0x579440 (GetObjectName's two literals)
 * InputDevice::GetObjectName and the Keyboard device class. Shared declarations: input_device.h next to this file.
 */
#include "input_device.h"
#include "sdw_enums.h"
#include "../sdk/dinput.h"

/* 0x4071b0 - the name DirectInput gives the object at diObjOffset (a key name for the keyboard), in the ANSI code page,
 * at most 0x40 bytes. The Portuguese space bar's name is spelled without its cedilla (the game font has no Ç). 1 on
 * success, 0 on failure. */
u32 InputDevice::GetObjectName(u32 diObjOffset, char *out)
{
    long hr;
    DIDEVICEOBJECTINSTANCEA doi;
    DIPROPSTRING prop;

    *out = 0;
    doi.dwSize = sizeof(DIDEVICEOBJECTINSTANCEA);
    hr = pDevice->GetObjectInfo(&doi, diObjOffset, DIPH_BYOFFSET);
    if (hr < 0)
        return 0;
    prop.diph.dwSize = sizeof(DIPROPSTRING);
    prop.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    prop.diph.dwHow = DIPH_BYID;
    prop.diph.dwObj = doi.dwType;
    hr = pDevice->GetProperty(DIPROP_KEYNAME, &prop.diph);
    if (hr >= 0) {
        WideCharToMultiByte(0, 0, prop.wsz, -1, out, 0x40, NULL, NULL);
        if (strcmp(out, "BARRA DE ESPA\xc7OS") == 0)
            strcpy(out, "BARRA DE ESPACOS");
    } else {
        return 0;
    }
    return 1;
}

/* ======================================================================== Keyboard, 0x4072ac-0x4077ef */

/* 0x4072ac - all 0x100 DIK scan codes as buttons (the input manager passes 0x100), exclusive foreground. The axis keys
 * default to the arrows (X, Y) and PgDn / PgUp (Z) whether or not the device came up. */
Keyboard::Keyboard(u16 buttonCount, D3DApp *app, HRESULT *result)
{
    AllocButtons(buttonCount, app);
    deviceKind = INPUTDEV_KEYBOARD;
    isDigital = 1;
    *result = pApp->CreateKeyboardDevice(&pDevice);
    if (*result >= 0) {
        *result = pDevice->SetDataFormat(&c_dfDIKeyboard);
        if (*result >= 0)
            *result = pDevice->SetCooperativeLevel(pApp->hWnd, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
    }
    KB_AXISKEY(KB_AXIS_X_NEG) = DIK_LEFT;
    KB_AXISKEY(KB_AXIS_X_POS) = DIK_RIGHT;
    KB_AXISKEY(KB_AXIS_Y_NEG) = DIK_UP;
    KB_AXISKEY(KB_AXIS_Y_POS) = DIK_DOWN;
    KB_AXISKEY(KB_AXIS_Z_NEG) = DIK_NEXT;
    KB_AXISKEY(KB_AXIS_Z_POS) = DIK_PRIOR;
}

/* 0x407388 */
Keyboard::~Keyboard()
{
    if (pDevice) {
        pDevice->Unacquire();
        pDevice->Release();
        pDevice = NULL;
    }
}

/* 0x4073db - as Joystick::SetAcquired. */
long Keyboard::SetAcquired(u8 acquire)
{
    if (pDevice == NULL)
        return E_FAIL;
    if (acquire == 1) {
        pDevice->Acquire();
        acquired = 1;
    } else {
        pDevice->Unacquire();
        acquired = 0;
    }
    return 0;
}

/* 0x407437 - reads the 256 key bytes (re-acquiring while the input is lost), then the axes from the axis keys: +1 when
 * only the positive key is held, -1 when only the negative one is, else 0. The two error tests are bitwise ANDs with
 * E_NOTIMPL and DIERR_NOTINITIALIZED where equality was surely meant: the first can never be true for a failure code
 * (bit 31 is always set), the second rejects every failure and also S_FALSE (DI_BUFFEROVERFLOW), with E_FAIL, before
 * buttonDown is touched - the previous frame's keys stay in buttonDown and buttonPressed. */
long Keyboard::Update()
{
    u8 state[256];
    long hr;
    u16 i;

    if (pDevice != NULL && acquired == 1) {
        hr = DIERR_INPUTLOST;
        while (hr == DIERR_INPUTLOST) {
            hr = pDevice->GetDeviceState(sizeof(state), state);
            if (hr == DIERR_INPUTLOST) {
                hr = pDevice->Acquire();
                if (hr < 0)
                    return hr;
            }
        }
        if (!(hr & E_NOTIMPL) && hr < 0) /* E_NOTIMPL = DIERR_UNSUPPORTED */
            return hr;
        if (!(hr & DIERR_NOTINITIALIZED)) { /* DIERR_NOTINITIALIZED */
            for (i = 0; i < buttonCount; i++)
                buttonDown[i] = (state[i] & 0x80) ? 1 : 0;
            InputDevice::Update();
            if (buttonDown[KB_AXISKEY(KB_AXIS_X_POS)] && !buttonDown[KB_AXISKEY(KB_AXIS_X_NEG)])
                axisX = 1.0f;
            else if (!buttonDown[KB_AXISKEY(KB_AXIS_X_POS)] && buttonDown[KB_AXISKEY(KB_AXIS_X_NEG)])
                axisX = -1.0f;
            else
                axisX = 0.0f;
            if (buttonDown[KB_AXISKEY(KB_AXIS_Y_POS)] && !buttonDown[KB_AXISKEY(KB_AXIS_Y_NEG)])
                axisY = 1.0f;
            else if (!buttonDown[KB_AXISKEY(KB_AXIS_Y_POS)] && buttonDown[KB_AXISKEY(KB_AXIS_Y_NEG)])
                axisY = -1.0f;
            else
                axisY = 0.0f;
            if (buttonDown[KB_AXISKEY(KB_AXIS_Z_POS)] && !buttonDown[KB_AXISKEY(KB_AXIS_Z_NEG)])
                axisZ = 1.0f;
            else if (!buttonDown[KB_AXISKEY(KB_AXIS_Z_POS)] && buttonDown[KB_AXISKEY(KB_AXIS_Z_NEG)])
                axisZ = -1.0f;
            else
                axisZ = 0.0f;
            return 0;
        }
        return E_FAIL;
    }
    return E_FAIL;
}

/* 0x407765 - replaces the six axis keys (no callers in the shipped build). */
void Keyboard::SetAxisKeys(u16 xNeg, u16 xPos, u16 yNeg, u16 yPos, u16 zNeg, u16 zPos)
{
    KB_AXISKEY(KB_AXIS_X_NEG) = xNeg;
    KB_AXISKEY(KB_AXIS_X_POS) = xPos;
    KB_AXISKEY(KB_AXIS_Y_NEG) = yNeg;
    KB_AXISKEY(KB_AXIS_Y_POS) = yPos;
    KB_AXISKEY(KB_AXIS_Z_NEG) = zNeg;
    KB_AXISKEY(KB_AXIS_Z_POS) = zPos;
}

/* 0x4077c0 Keyboard_ScalarDeletingDtor: generated by the compiler from the virtual destructor. */
