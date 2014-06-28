#ifndef __GV_EVENT_DISPATCHER_H__
#define __GV_EVENT_DISPATCHER_H__

#include "gv_object.h"
#include "gv_list.h"
#include "gv_rbtree.h"
#include "gv_event.h"

GV_NS_BEGIN

class EventListenerStub : public Object, public RBTree::node {
    GV_FRIEND_PTR();
    friend class EventDispatcher;

    EventListenerStub(EventDispatcher *dispatcher, const ptr<UniStr> name, Object *holder, bool capture, int priority) noexcept : 
        _dispatcher(dispatcher),
        _name(name), 
        _holder(holder),
        _capture(capture ? 1 : 0),
        _priority(priority) { }
    ~EventListenerStub() noexcept;
    virtual void operator ()(ptr<Event>&) noexcept = 0;

    EventDispatcher *_dispatcher;
    Object          *_holder;
    ptr<UniStr>      _name;
    int              _priority;
    int              _capture; 
};

class EventDispatcher : public Object {
    friend class EventListenerStub;
public:
    EventDispatcher() noexcept : _parent() {}
    ~EventDispatcher() noexcept;

    template <typename _Functor>
    ptr<EventListenerStub> addEventListener(
        const ptr<UniStr> &name,
        Object *holder,
        _Functor &&functor,
        bool useCapture = false,
        int priority = 0)noexcept 
    {
        struct Stub : EventListenerStub {
            _Functor _functor;
            void operator()(ptr<Event> &e)noexcept override {
                _functor(e);
            }
            template <typename _F>
            Stub(EventDispatcher *dispatcher, const ptr<UniStr> &name, _F &&functor, bool useCapture, int priority) noexcept :
                EventListenerStub(dispatcher, name, holder, useCapture, priority),
                _functor(std::forward<_F>(functor)){ }
        };
        return addEventListener(object<Stub>(this, name, holder, std::forward<_Functor>(functor), useCapture, priority));
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

    virtual bool dispatchEvent(ptr<Event> &event);
protected:
    bool dispatchEvent(ptr<Event> &event, const ptr<EventDispatcher> *dispatchers, unsigned count, bool reverse);

private:
    bool dispatchEvent(ptr<Event> &event, bool capture);
    ptr<EventListenerStub> addEventListener(ptr<EventListenerStub> &stub) noexcept;
    static void removeStub(EventListenerStub *stub);

protected:
    EventDispatcher *_parent;
private:
    RBTree _tree; 
};

GV_NS_END

#endif

