/* Rockets never run out of fuel and never break. The rocket spends fuel and takes damage through its message handler
 * (Rocket::HandleMessage): MSG_ROCKET_BURN_FUEL takes the fuel burnt off `fuel` (0 = empty), and MSG_ROCKET_DAMAGE,
 * which Ralph sends on every impact while flying, takes the impact off `hits` (0 = broken, and the flight ends). After
 * every burn this puts the fuel back to full; damage is not taken at all, the hits stay full and the rocket answers
 * that it is intact. Both gauges stay full. */
#include "sdw_mod.h"
#include "sdw_enums.h"
#include "sdw_classes.h"

typedef s32(SDW_THIS_CC *HandleMessageFn)(Rocket *self, SDW_THIS_EDX, ScnObject *sender, u32 msgId, void *arg);
static HandleMessageFn s_original;

static s32 SDW_THIS_CC HandleMessage(Rocket *self, SDW_THIS_EDX, ScnObject *sender, u32 msgId, void *arg)
{
    s32 result;
    if (msgId == MSG_ROCKET_DAMAGE) {
        self->hits = self->hitsMax;
        return 1; /* intact */
    }
    result = s_original(self, edx_unused, sender, msgId, arg);
    if (msgId == MSG_ROCKET_BURN_FUEL) {
        self->fuel = self->fuelMax;
        return 1; /* not empty */
    }
    return result;
}

SDW_MOD_EXPORT int SdwModInit(const SdwModApi *api)
{
    return api->hook("Rocket_HandleMessage", (void *)HandleMessage, (void **)&s_original);
}
