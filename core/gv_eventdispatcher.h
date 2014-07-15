#ifndef __GV_EVENT_DISPATCHER_H__
#define __GV_EVENT_DISPATCHER_H__

#include "gv_object.h"
#include "gv_rbmap.h"
#include "gv_event.h"

GV_NS_BEGIN

class EventDispatcher;

class EventListenerStub : public Object {
    friend class Object;
    friend class EventDispatcher;

    EventListenerStub(EventDispatcher *dispatcher, const ptr<UniStr> &name, Object *holder, bool capture, int priority) noexcept : 
        _dispatcher(dispatcher),
        _name(name), 
        _holder(holder),
        _capture(capture ? 0 : 1),
        _priority(priority) { }
    ~EventListenerStub() noexcept;
    virtual void operator ()(ptr<Event>&) noexcept = 0;

    map_entry        _entry;
    EventDispatcher *_dispatcher;
    Object          *_holder;
    ptr<UniStr>      _name;
    int              _priority;
    int              _capture; 
};

class EventDispatcher : public Object {
    friend class EventListenerStub;

    template <typename _Functor>
    struct FunctorStub : EventListenerStub {
        _Functor _functor;
        void operator()(ptr<Event> &e)noexcept override {
            _functor(e);
        }
        template <typename _F>
        FunctorStub(EventDispatcher *dispatcher, const ptr<UniStr> &name, Object *holder, _F &&functor, bool useCapture, int priority) noexcept :
            EventListenerStub(dispatcher, name, holder, useCapture, priority),
            _functor(std::forward<_F>(functor)){ }
    };
public:
    ~EventDispatcher() noexcept;

    template <typename _Functor>
    ptr<EventListenerStub> addEventListener(
        const ptr<UniStr> &name,
        Object *holder,
        _Functor &&functor,
        bool useCapture = false,
        int priority = 0)noexcept 
    {
        return addEventListener(object<FunctorStub<_Functor>>(this, name, holder, std::forward<_Functor>(functor), useCapture, priority));
    }

    template<typename _T>
    typename std::enable_if<
        std::is_base_of<Object, _T>::value,
        ptr<EventListenerStub>
    >::type addEventListener(
        const ptr<UniStr> &name, 
        _T *holder, 
        void (_T::*func)(Event&), 
        bool useCapture = false, 
        int priority = 0) noexcept 
    {
        struct Stub : EventListenerStub {
            void (_T::*_func)(Event&);
            void operator()(ptr<Event> &e) noexcept override {
                (static_cast<_T*>(_holder)->*(_func))(e);
            }
            Stub(EventDispatcher *dispatcher, 
                 const ptr<UniStr> &name, 
                 _T *holder, 
                 void (_T::*func)(Event&), 
                 bool useCapture, 
                 int priority) noexcept :
                EventListenerStub(this, name, holder, useCapture, priority),
                _func(func) { }
        };
        return addEventListener(object<Stub>(this, name, holder, func, useCapture, priority));
    }

    virtual bool dispatchEvent(ptr<Event> event);
protected:
    bool dispatchEvent(ptr<Event> event, const ptr<EventDispatcher> &target, const ptr<EventDispatcher> *dispatchers, unsigned count);

private:
    bool dispatchEvent(ptr<Event> &event, bool cap);
    ptr<EventListenerStub> addEventListener(ptr<EventListenerStub> &stub) noexcept;

private:
    struct compare {
        int operator()(const EventListenerStub &lhs, const EventListenerStub &rhs) const noexcept {
            return lhs._name < rhs._name ||
                   lhs._capture < rhs._capture ||
                   lhs._priority > rhs._priority ? -1 : 1;
        }
    };
    typedef gv_map(EventListenerStub, EventListenerStub, _entry, compare) map_type;
    map_type _map;
};

GV_NS_END

#endif

