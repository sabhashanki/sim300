/****************************************************************************************/
// Very simple scheduling class.
//
//   Created : 2 March 2012
//     Author: Wouter Cloete
//    Sponsor: Firmlogik (Pty) Ltd
//    License: GNU General Public License Version 3
//License URL: http://www.gnu.org/licenses/gpl.txt
//  Copyright: 2012 Firmlogik (Pty) Ltd 2012.  All rights reserved.
//        URL: http://www.firmlogik.co.za
/****************************************************************************************/
#ifndef SCHEDULER_H
#define SCHEDULER_H
/****************************************************************************************/
#include "types.h"
#include "timer.h"
/****************************************************************************************/
namespace SCHEDULER {
  typedef u08 tSignal;
}
/****************************************************************************************/
using namespace SCHEDULER;
/****************************************************************************************/
class Csignal {
  private:
    bool set;
    u16 period;
    u16 ticks;
  public:
    static const u08 divisor = COUNTER_CLK_DIV1024;
    static const u08 periodCnt = (255);
    static const f32 tickBase = 0.0177083;
    //static const f32 tickBase = (u32)((u16)1024 * (u08)periodCnt) / F_CPU;
    static const u08 maxSignals = 8;
    Csignal() {
      set = false;
      ticks = 0;
    }
    void clear() {
      set = false;
      ticks = 0;
    }
    void setPeriod(u16 _period) {
      period = _period;
    }
    bool isSet(void) {
      bool _s = set;
      set = false;
      return _s;
    }
    void tick() {
      ticks++;
      if (ticks > period) {
        set = true;
        ticks = 0;
      }
    }
};
/****************************************************************************************/
class Cscheduler {
  private:
    //static sSignal* signals[SCHEDULER::MAX_SIGNALS];
  public:
    Cscheduler(void);
    bool attach(Csignal* signal);
    static void service(void);
    void start(void);
};
/****************************************************************************************/
using namespace SCHEDULER;
/****************************************************************************************/
extern Cscheduler scheduler;
/****************************************************************************************/
#endif
