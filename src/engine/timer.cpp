/*
 * Timer class, SheepD3D.exe 0x40b060-0x40b5fd: 11 routines and the compiler-generated scalar deleting destructor. The
 * names are descriptive, not recovered.
 * Two devices in it are there only to reproduce the original stack layout, not claims about the source text:
 * TimerCalibrationWork and the {inner, outer} loop pair (VC6 /Od allocates locals in its own order, and grouping
 * them in a struct pins their offsets). Timer_ReadTSC is inline assembly in the original too (cpuid; rdtsc).
 */
/* BYTES: flow, slot-group. */
/* BYTES(slot-group): Timer::Timer: locals grouped in TimerCalibrationWork and the {inner, outer} pair only to pin the original frame offsets */
#include "timer.h"
#include "sdw_classes.h"

#include "../sdk/win32.h"
#include "../sdk/crt.h"

/* T010 .bss 0x5c7aa8..0x5c7ab0: recovered timer calibration storage. */
double g_cpuHz;

struct TimerCalibrationWork {
    s64 tscBefore, tscAfter, pcAfter;
    s64 samples[100];
    s64 frequency, pcBefore, sum;
};

/* 0x40b060 - on first construction, measures TSC ticks per QueryPerformanceCounter interval 100 times around a busy loop
 * of 10,000 sin(pow(i, 10)) calls, and sets g_cpuHz = sum * 1e6 / 100228400.0 (constant 0x574348). */
Timer::Timer()
{
    TimerCalibrationWork work;
    struct {
        u16 inner, outer;
    } loop;
    if (g_cpuHz == 0.0) {
        work.sum = 0;
        QueryPerformanceFrequency(&work.frequency);
        for (loop.outer = 0; loop.outer < 100; loop.outer++) {
            QueryPerformanceCounter(&work.pcBefore);
            work.tscBefore = ReadTSC();
            for (loop.inner = 0; loop.inner < 10000; loop.inner++)
                sin(pow((double)loop.inner, 10.0));
            QueryPerformanceCounter(&work.pcAfter);
            work.tscAfter = ReadTSC();
            work.samples[loop.outer] =
                work.frequency * (work.tscAfter - work.tscBefore) / (work.pcAfter - work.pcBefore);
            work.sum += work.samples[loop.outer];
        }
        g_cpuHz = (double)(work.sum * 1000000) / 100228400.0;
    }
    baseTsc = 0;
    elapsedTsc = 0;
    deltaTsc = 0;
    running = 0;
}

/* 0x40b27f - the vtable holds the compiler-generated scalar deleting destructor 0x40b5d0 that calls this. */
Timer::~Timer() {}

/* 0x40b293 */
void Timer::Start()
{
    running = 1;
    baseTsc = ReadTSC();
    elapsedTsc = 0;
    deltaTsc = 0;
}

/* 0x40b2d8 - takes a last sample, then stops. */
void Timer::Stop()
{
    s64 previous = elapsedTsc;
    elapsedTsc = ReadTSC() - baseTsc;
    deltaTsc = elapsedTsc - previous;
    running = 0;
}

/* 0x40b32d - Time_Resume passes the negated unconsumed delta saved by Time_Pause. */
void Timer::OffsetBase(double ticks)
{
    baseTsc += (s64)ticks;
}

/* 0x40b35a - samples (so it also advances deltaTsc), returns the elapsed time. */
/* BYTES(slot-group): previous / result grouped in one struct to pin their frame offsets */
/* BYTES(flow, inferred): Convert is called in both arms (two call sites in the original), not hoisted */
double Timer::GetElapsed(int unit)
{
    struct {
        s64 previous;
        double result;
    } work;
    if (running == 1) {
        work.previous = elapsedTsc;
        elapsedTsc = ReadTSC() - baseTsc;
        deltaTsc = elapsedTsc - work.previous;
        work.result = Convert(elapsedTsc, unit);
    } else {
        work.result = Convert(elapsedTsc, unit);
    }
    return work.result;
}

/* 0x40b3ef - samples, returns the time since the previous sample; 0.0 while stopped. */
/* BYTES(slot-group): previous / result grouped in one struct to pin their frame offsets */
double Timer::GetDelta(int unit)
{
    struct {
        s64 previous;
        double result;
    } work;
    if (running == 1) {
        work.previous = elapsedTsc;
        elapsedTsc = ReadTSC() - baseTsc;
        deltaTsc = elapsedTsc - work.previous;
        work.result = Convert(deltaTsc, unit);
    } else {
        work.result = 0.0;
    }
    return work.result;
}

/* 0x40b478 */
double Timer::PeekElapsed(int unit)
{
    return Convert(elapsedTsc, unit);
}

/* 0x40b49c */
double Timer::PeekDelta(int unit)
{
    double result;
    if (running == 1)
        result = Convert(deltaTsc, unit);
    else
        result = 0.0;
    return result;
}

/* 0x40b4e4 */
s64 Timer::ReadTSC()
{
    s64 ticks = 0;
    __asm {
        push eax
        push ebx
        push edx
        cpuid
        rdtsc
        lea ebx, ticks
        mov [ebx], eax
        mov [ebx+4], edx
        pop edx
        pop ebx
        pop eax
    }
    return ticks;
}

/* 0x40b51c - the 16 bytes after the code (to 0x40b5c8) are this switch's jump table; Ghidra's function ends before it. */
double Timer::Convert(s64 ticks, int unit)
{
    double result;
    switch (unit) {
        case TIMER_TICKS:
            result = (double)ticks;
            break;
        case TIMER_SECONDS:
            result = (double)ticks / g_cpuHz;
            break;
        case TIMER_MILLISECONDS:
            result = (double)(ticks * 1000) / g_cpuHz;
            break;
        case TIMER_MICROSECONDS:
            result = (double)(ticks * 1000000) / g_cpuHz;
            break;
        default:
            result = -1.0;
            break;
    }
    return result;
}
