#include "opengxv.h"
#include "gv_event.h"
#include "gv_log.h"

GV_NS_BEGIN

GV_IMPL_UNISTR(Event, ACTIVATE);
GV_IMPL_UNISTR(Event, ADDED);
GV_IMPL_UNISTR(Event, ADD_TO_STAGE);
GV_IMPL_UNISTR(Event, CANCEL);
GV_IMPL_UNISTR(Event, CHANGE);

Event::Event(const ptr<UniStr> &type, bool bubbles, bool cancelable) noexcept 
: _type(type),
  _target(), 
  _currentTarget(),
  _stop(StopType::NONE),
  _isDefaultPrevented(false),
  _cancelable(cancelable), 
  _bubbles(bubbles),
  _eventPhase(EventPhase::AT_TARGET)
{ }

Event::Event(const char *type, bool bubbles, bool cancelable) noexcept 
: Event(gv_unistr(type), bubbles, cancelable) 
{ }

Event::Event(const std::string &type, bool bubbles, bool cancelable) noexcept 
: Event(gv_unistr(type), bubbles, cancelable) 
{ }

GV_NS_END
