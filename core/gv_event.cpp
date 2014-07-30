#include "opengxv.h"
#include "gv_event.h"
#include "gv_log.h"

GV_NS_BEGIN

GV_IMPL_UNISTR(Event, ACTIVATE);
GV_IMPL_UNISTR(Event, ADDED);
GV_IMPL_UNISTR(Event, ADD_TO_STAGE);
GV_IMPL_UNISTR(Event, CANCEL);
GV_IMPL_UNISTR(Event, CHANGE);
GV_IMPL_UNISTR(Event, REMOVED);
GV_IMPL_UNISTR(Event, REMOVED_FROM_STAGE);
GV_IMPL_UNISTR(Event, MOVE);
GV_IMPL_UNISTR(Event, RESIZE);

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
: Event(unistr(type), bubbles, cancelable) 
{ }

Event::Event(const std::string &type, bool bubbles, bool cancelable) noexcept 
: Event(unistr(type), bubbles, cancelable) 
{ }

Event::Event(const Event &x) noexcept 
: _type(x._type),
  _target(), 
  _currentTarget(),
  _stop(StopType::NONE),
  _isDefaultPrevented(false),
  _cancelable(x._cancelable), 
  _bubbles(x._bubbles),
  _eventPhase(EventPhase::AT_TARGET)
{ }

GV_NS_END
