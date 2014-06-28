#include "opengv.h"
#include "gv_eventdispatcher.h"
#include "gv_log.h"
#include <map>

GV_NS_BEGIN

EventListenerStub::~EventListenerStub() noexcept {
    if (_dispatcher) {
        _dispatcher->_tree.remove(this);
    }
}

/* EventDispatcher */
void EventDispatcher::removeStub(EventListenerStub *stub) noexcept {
    if (stub->_left) {
        removeStub(static_cast<EventListenerStub*>(stub->_left));
    }
    if (stub->_right) {
        removeStub(static_cast<EventListenerStub*>(stub->_right));
    }
    stub->_dispatcher = nullptr;
}

EventDispatcher::~EventDispatcher() noexcept {
    RBTree::node *node;
    if ((node = _tree.root())) {
        removeStub(static_cast<EventListenerStub*>(node));
    }
}

ptr<EventListenerStub> EventDispatcher::addEventListener(ptr<EventListenerStub> &stub) noexcept {
    RBTree::node **link = &_tree._root;
    RBTree::node *parent = nullptr;

    while (*link) {
        parent = *link;
        EventListenerStub *node = static_cast<EventListenerStub*>(parent);
        if (stub->_name < node->_name) {
            link = &(*link)->_left;
        }
        else if (stub->_capture > node->_capture) {
            link = &(*link)->_left;
        }
        else if (stub->_priority > node->_priority) {
            link = &(*link)->_left;
        }
        else {
            link = &(*link)->_right;
        }
    }
    RBTree::link(stub, parent, link);
    _tree.insert(stub);
    return stub;
}

inline bool EventDispatcher::dispatchEvent(ptr<Event> &event, bool capture) {
    int cap = capture ? 1 : 0;
    RBTree::node *node = _tree._root;
    RBTree::node *head = nullptr;

    EventListenerStub *stub;
    while (node) {
        stub = static_cast<EventListenerStub*>(node);
        if (stub->_name >= event->_type) {
            head = node;
            node = node->_left;
        }
        else if (stub->_capture <= cap) {
            head = node;
            node = node->_left;
        }
        else {
            node = node->_right;
        }
    }

    if (!head) {
        return true;
    }

    stub = static_cast<EventListenerStub*>(head);
    if (stub->_name != event->_type || stub->_capture != cap) {
        return true;
    }

    struct context {
        context() noexcept {}
        context(EventListenerStub *stub, Object *holder) noexcept :
            _stub(ptr_cast<EventListenerStub>(stub->self())), 
            _holder(holder->self()) {}

        ptr<EventListenerStub> _stub;
        ptr<Object> _holder;
    };
    std::vector<context> ctxs;
    ctxs.reserve(16);
    do {
        ctxs.emplace_back(stub, stub->_holder);
        head = head->next();
        if (!head) {
            break;
        }
        stub = static_cast<EventListenerStub*>(head);
    } while (stub->_name == event->_type && stub->_capture == cap); 

    for (auto &ctx : ctxs) {
        (*ctx._stub)(event);
        if (event->_stop == Event::StopType::IMMEDIATE) {
            break;
        }
    }
    return event->_stop == Event::StopType::NONE; 
}

bool EventDispatcher::dispatchEvent(ptr<Event> &event, const ptr<EventDispatcher> *dispatchers, unsigned count, bool reverse) noexcept {
    const ptr<EventDispatcher> *dispatcher, *end;
    if (reverse) {
        if (event->_bubbles) {
            dispatcher = dispatchers + count - 1;
            end = dispatchers;
            event->_eventPhase = EventPhase::CAPTURING_PHASE;
            for (; dispatcher > end; --dispatcher) {
                if (!(*dispatcher)->dispatchEvent(event, true)) {
                    return true;
                }
            }
        }

        event->_eventPhase = EventPhase::AT_TARGET;
        dispatcher = dispatchers;
        if (!(*dispatcher)->dispatchEvent(event, false)) {
            return true;
        }

        end = dispatchers + count;
        event->_eventPhase = EventPhase::BUBBLING_PHASE;
        for (++dispatcher; dispatcher < end; ++dispatcher) {
            if (!(*dispatcher)->dispatchEvent(event, false)) {
                return true;
            }
        }
    }
    else {
        if (event->_bubbles) {
            dispatcher = dispatchers;
            end = dispatchers + count - 1;
            event->_eventPhase = EventPhase::CAPTURING_PHASE;
            for (; dispatcher < end; ++dispatcher) {
                if (!(*dispatcher)->dispatchEvent(event, true)) {
                    return true;
                }
            }
        }

        event->_eventPhase = EventPhase::AT_TARGET;
        dispatcher = dispatchers + count - 1;
        if (!(*dispatcher)->dispatchEvent(event, false)) {
            return true;
        }

        end = dispatchers;
        event->_eventPhase = EventPhase::BUBBLING_PHASE;
        for (--dispatcher; dispatcher >= end; --dispatcher) {
            if (!(*dispatcher)->dispatchEvent(event, false)) {
                return true;
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
        dispatchers.emplace_back(ptr_cast<EventDispatcher>(self()));
        while (parent) {
            dispatchers.emplace_back(ptr_cast<EventDispatcher>(parent->self()));
            parent = parent->_parent;
        }
        if (!dispatchEvent(event, dispatchers.data(), dispatchers.size(), true)) {
            return false;
        }
    }
    else {
        ptr<EventDispatcher> p(ptr_cast<EventDispatcher>(self()));
        if (!dispatchEvent(event, &p, 1, true)) {
            return false;
        }
    }
    return event->isDefaultPrevented(); 
}

GV_NS_END
