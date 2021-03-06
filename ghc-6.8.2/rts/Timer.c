/* -----------------------------------------------------------------------------
 *
 * (c) The GHC Team, 1995-2005
 *
 * Interval timer service for profiling and pre-emptive scheduling.
 *
 * ---------------------------------------------------------------------------*/

/*
 * The interval timer is used for profiling and for context switching in the
 * threaded build. 
 *
 * This file defines the platform-independent view of interval timing, relying
 * on platform-specific services to install and run the timers.
 *
 */
#include "Rts.h"
#include "RtsFlags.h"
#include "Proftimer.h"
#include "Storage.h"
#include "Schedule.h"
#include "Timer.h"
#include "Ticker.h"
#include "Capability.h"
#include "RtsSignals.h"

/* ticks left before next pre-emptive context switch */
       int ticks_to_ctxt_switch = 0;

#if defined(THREADED_RTS)
/* idle ticks left before we perform a GC */
static int ticks_to_gc = 0;
#endif

/*
 * Function: handle_tick()
 *
 * At each occurrence of a tick, the OS timer will invoke
 * handle_tick().
 */
static
void
handle_tick(int unused STG_UNUSED)
{
  handleProfTick();
  // We don't want to dilute how often timer interrupts happen...because we have very high priority
  // haskell land code now.  Scheduler can decide to give longer timeslices if it wants to.
  //if (TICKS_TO_TIMER_IRQ > 0) {
      //ticks_to_ctxt_switch--;
      //if (ticks_to_ctxt_switch <= 0) {
	  //ticks_to_ctxt_switch = TICKS_TO_TIMER_IRQ;
	  pending_irqs |= 1; /* flag that timerHandler should be run ASAP */
      //}
  //}

#if defined(THREADED_RTS)
  /* 
   * If we've been inactive for idleGCDelayTime (set by +RTS
   * -I), tell the scheduler to wake up and do a GC, to check
   * for threads that are deadlocked.
   */
  switch (recent_activity) {
  case ACTIVITY_YES:
      recent_activity = ACTIVITY_MAYBE_NO;
      ticks_to_gc = RtsFlags.GcFlags.idleGCDelayTime /
                    RtsFlags.MiscFlags.tickInterval;
      break;
  case ACTIVITY_MAYBE_NO:
      if (ticks_to_gc == 0) {
          /* 0 ==> no idle GC */
          recent_activity = ACTIVITY_DONE_GC;
          // disable timer signals (see #1623)
          stopTimer();
      } else {
          ticks_to_gc--;
          if (ticks_to_gc == 0) {
              ticks_to_gc = RtsFlags.GcFlags.idleGCDelayTime /
                  RtsFlags.MiscFlags.tickInterval;
              recent_activity = ACTIVITY_INACTIVE;
              blackholes_need_checking = rtsTrue;
              /* hack: re-use the blackholes_need_checking flag */
              wakeUpRts();
          }
      }
      break;
  default:
      break;
  }
#endif
}

void
initTimer(void)
{
    initProfTimer();
    if (RtsFlags.MiscFlags.tickInterval != 0) {
        initTicker(RtsFlags.MiscFlags.tickInterval, handle_tick);
    }
}

void
startTimer(void)
{
#ifndef house_HOST_OS
    if (RtsFlags.MiscFlags.tickInterval != 0) {
        startTicker();
    }
#endif
}

void
stopTimer(void)
{
#ifndef house_HOST_OS
    if (RtsFlags.MiscFlags.tickInterval != 0) {
        stopTicker();
    }
#endif
}

void
exitTimer(void)
{
    if (RtsFlags.MiscFlags.tickInterval != 0) {
        exitTicker();
    }
}
