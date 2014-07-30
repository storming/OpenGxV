#ifndef __GV_DRIVER_INFO_H__
#define __GV_DRIVER_INFO_H__

#include "gv_object.h"

GV_NS_BEGIN

class DriverInfo : public Object {
    friend class Object;
protected:
    DriverInfo() noexcept;
};

GV_NS_END

#endif


