#ifndef IRADIO_H
#define IRADIO_H

#include "avrlibtypes.h"
#include "Master2Radio.h"

using namespace MASTER2RADIO;

typedef enum {
  LOADING_UNKNOWN = 0,
  LOADING_TRUE = 1,
  LOADING_FALSE = 2
} eLoadingState;

typedef enum {
  AUTHORIZE_UNKNOWN = 0,
  AUTHORIZE_TRUE = 1,
  AUTHORIZE_FALSE = 2
} eAuthorizeState;

typedef enum {
  BIOVRD_UNKNOWN = 0,
  BIOVRD_TRUE = 1,
  BIOVRD_FALSE = 2
} eBiometricOverrideState;

namespace RADIO {

  class Iradio {
    public:
      virtual eAuthorizeState getAuthoriseState(void) = 0;
      virtual eLoadingState getLoadingState(void) = 0;
      virtual eBiometricOverrideState getBiometricOverrideState(void) = 0;
      virtual bool setLoadingOn(void) = 0;
      virtual bool setLoadingOff(void) = 0;
      virtual bool setBioOvrOn(void) = 0;
      virtual bool setBioOvrOff(void) = 0;
      virtual bool setTamperOn(void) = 0;
      virtual bool setTamperOff(void) = 0;
  };
}
#endif
