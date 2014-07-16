#include "opengxv.h"

#include <vector>
#include "gv_eventdispatcher.h"
#include "gv_log.h"

GV_NS_BEGIN

EventListenerStub::~EventListenerStub() noexcept {
    if (_dispatcher) {
        _dispatcher->_map.erase(this);
    }
}

/* EventDispatcher */
EventDispatcher::~EventDispatcher() noexcept {
    _map.clear([](EventListenerStub *stub) {
        stub->_dispatcher = nullptr;
    });
}

ptr<EventListenerStub> EventDispatcher::addEventListener(ptr<EventListenerStub> &stub) noexcept {
    _map.emplace(*stub, [=](){ return stub; });
    return stub;
}

inline  bool EventDispatcher::dispatchEvent(ptr<Event> &event, bool cap) {
    int capture = cap ? 0 : 1;
    EventListenerStub *stub = nullptr;
    _map.find(
        event->_type,
        [&](const ptr<UniStr> &lhs, const EventListenerStub &rhs) noexcept {
            if (stub) {
                if (lhs != rhs._name || capture != rhs._capture) {
                    return 0;
                }
                else {
                    stub = (EventListenerStub*)std::addressof<const EventListenerStub>(rhs);
                }
            }
            else {
                int n;
                if ((n = lhs - rhs._name)) {
                    return n;
                }
                if ((n = capture - rhs._capture)) {
                    return n;
                }
                stub = (EventListenerStub*)std::addressof<const EventListenerStub>(rhs);
            }
            return -1;
        });

    if (!stub) {
        return true;
    }

    struct context {
        context() noexcept {}
        context(EventListenerStub *stub, Object *holder) noexcept :
            _stub(stub), 
            _holder(holder) {}

        ptr<EventListenerStub> _stub;
        ptr<Object> _holder;
    };
    static std::vector<context> ctxs;
    size_t old_size = ctxs.size();
    do {
        ctxs.emplace_back(stub, stub->_holder);
        stub = map_type::next(stub);
    } while (stub && stub->_name == event->_type && stub->_capture == capture); 

    event->_currentTarget = this; 
    for (auto &ctx : ctxs) {
        (*ctx._stub)(event);
        if (event->_stop == Event::StopType::IMMEDIATE) {
            break;
        }
    }
    ctxs.resize(old_size);
    return true; 
}

bool EventDispatcher::dispatchEvent(ptr<Event> event, 
                                    const ptr<EventDispatcher> &target, 
                                    const ptr<EventDispatcher> *dispatchers, 
                                    unsigned count) {
    const ptr<EventDispatcher> *dispatcher, *end;

    if (event->_target) {
        event = event->clone();
    }
    event->_target = target; 

    if (count) {
        dispatcher = dispatchers + count - 1;
        end = dispatchers;
        event->_eventPhase = EventPhase::CAPTURING_PHASE;
        for (; dispatcher >= end; --dispatcher) {
            if (!(*dispatcher)->dispatchEvent(event, true)) {
                return false;
            }
            if (event->_stop != Event::StopType::NONE) {
                return event->isDefaultPrevented(); 
            }
        }
    }

    event->_eventPhase = EventPhase::AT_TARGET;
    if (!target->dispatchEvent(event, false)) {
        return false;
    }
    if (event->_stop != Event::StopType::NONE) {
        return event->isDefaultPrevented(); 
    }

    if (count && event->_bubbles) {
        dispatcher = dispatchers; 
        end = dispatchers + count;
        event->_eventPhase = EventPhase::BUBBLING_PHASE;
        for (; dispatcher < end; ++dispatcher) {
            if (!(*dispatcher)->dispatchEvent(event, false)) {
                return false;
            }
            if (event->_stop != Event::StopType::NONE) {
                break;
            }
        }
    }
    return event->isDefaultPrevented(); 
}

bool EventDispatcher::dispatchEvent(ptr<Event> event) {
    if (event->_target) {
        event = event->clone();
    }
    event->_target = this;
    event->_eventPhase = EventPhase::AT_TARGET;
    if (!dispatchEvent(event, false)) {
        return false;
    }
    return event->isDefaultPrevented(); 
}

GV_NS_END
