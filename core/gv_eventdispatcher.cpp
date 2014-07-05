#include "opengv.h"
#include "gv_eventdispatcher.h"
#include "gv_log.h"
#include <map>

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

inline bool EventDispatcher::dispatchEvent(ptr<Event> &event, bool capture) {
    int cap = capture ? 0 : 1;
    EventListenerStub *stub = nullptr;
    _map.find(
        event->_type,
        [&](const ptr<UniStr> &lhs, const EventListenerStub &rhs) noexcept {
            if (stub) {
                if (lhs != rhs._name || cap != rhs._capture) {
                    return 0;
                }
                else {
                    stub = (EventListenerStub*)std::addressof<const EventListenerStub>(rhs);
                }
            }
            else {
                int n = lhs - rhs._name;
                if (n) {
                    return n;
                }
                n = cap - rhs._capture;
                if (n) {
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
    std::vector<context> ctxs;
    ctxs.reserve(16);
    do {
        ctxs.emplace_back(stub, stub->_holder);
        stub = map_type::next(stub);
    } while (stub && stub->_name == event->_type && stub->_capture == cap); 

    event->_currentTarget = this;
    for (auto &ctx : ctxs) {
        (*ctx._stub)(event);
        if (event->_stop == Event::StopType::IMMEDIATE) {
            break;
        }
    }
    return event->_stop == Event::StopType::NONE; 
}

bool EventDispatcher::dispatchEvent(ptr<Event> &event, 
                                    const ptr<EventDispatcher> &target, 
                                    const ptr<EventDispatcher> *dispatchers, 
                                    unsigned count, 
                                    bool reverse) noexcept {
    const ptr<EventDispatcher> *dispatcher, *end;
    event->_target = target;

    if (reverse) {
        if (count && event->_bubbles) {
            dispatcher = dispatchers + count - 1;
            end = dispatchers;
            event->_eventPhase = EventPhase::CAPTURING_PHASE;
            for (; dispatcher >= end; --dispatcher) {
                if (!(*dispatcher)->dispatchEvent(event, true)) {
                    return true;
                }
            }
        }

        event->_eventPhase = EventPhase::AT_TARGET;
        if (!target->dispatchEvent(event, false)) {
            return true;
        }

        if (count) {
            dispatcher = dispatchers; 
            end = dispatchers + count;
            event->_eventPhase = EventPhase::BUBBLING_PHASE;
            for (; dispatcher < end; ++dispatcher) {
                if (!(*dispatcher)->dispatchEvent(event, false)) {
                    return true;
                }
            }
        }
    }
    else {
        if (count && event->_bubbles) {
            dispatcher = dispatchers;
            end = dispatchers + count;
            event->_eventPhase = EventPhase::CAPTURING_PHASE;
            for (; dispatcher < end; ++dispatcher) {
                if (!(*dispatcher)->dispatchEvent(event, true)) {
                    return true;
                }
            }
        }

        event->_eventPhase = EventPhase::AT_TARGET;
        if (!target->dispatchEvent(event, false)) {
            return true;
        }

        if (count) {
            end = dispatchers; 
            dispatcher = dispatchers + count - 1;
            event->_eventPhase = EventPhase::BUBBLING_PHASE;
            for (; dispatcher >= end; --dispatcher) {
                if (!(*dispatcher)->dispatchEvent(event, false)) {
                    return true;
                }
            }
        }
    }
    return true; 
}

bool EventDispatcher::dispatchEvent(ptr<Event> &event) {
    if (event->_dispatchNum++) {
        event = event->clone();
    }

    if (event->_bubbles && _parent) {
        std::vector<ptr<EventDispatcher>> dispatchers;
        dispatchers.reserve(64);
        EventDispatcher *parent = _parent; 
        while (parent) {
            dispatchers.emplace_back(parent);
            parent = parent->_parent;
        }
        if (!dispatchEvent(event, this, dispatchers.data(), dispatchers.size(), true)) {
            return false;
        }
    }
    else {
        ptr<EventDispatcher> p(this);
        if (!dispatchEvent(event, this, nullptr, 0, true)) {
            return false;
        }
    }
    return event->isDefaultPrevented(); 
}

GV_NS_END
