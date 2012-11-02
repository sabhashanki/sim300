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
#include "scheduler.h"
/****************************************************************************************/
#undef DEBUG_SCHEDULER
/****************************************************************************************/
static Ctimer timer(0, Csignal::divisor, CONTINUOUS);
/****************************************************************************************/
Csignal* signals[Csignal::maxSignals];
Cscheduler::Cscheduler scheduler;
/****************************************************************************************/
Cscheduler::Cscheduler(void) {
  u08 cnt;
  timer.attach(service);
  for (cnt = 0; cnt < Csignal::maxSignals; cnt++) {
    signals[cnt] = 0;
  }
}
/****************************************************************************************/
void Cscheduler::start(void) {
  timer.enableInterrupt(OVERFLOW);
  timer.start(Csignal::periodCnt);
}
/****************************************************************************************/
bool Cscheduler::attach(Csignal* _signal) {
  u08 cnt = 0;
  while (cnt < Csignal::maxSignals) {
    if (signals[cnt] == 0) {
      signals[cnt] = _signal;
      return true;
    }
    cnt++;
  }
  return false;
}
/****************************************************************************************/
void Cscheduler::service(void) {
  u08 cnt;
  for (cnt = 0; cnt < Csignal::maxSignals; cnt++) {
    if (signals[cnt] != 0) {
      signals[cnt]->tick();
    }
  }
}
/****************************************************************************************/

