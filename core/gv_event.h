#ifndef __GV_EVENT_H__
#define __GV_EVENT_H__

#include "gv_object.h"
#include "gv_unistr.h"

GV_NS_BEGIN

enum class EventPhase {
    CAPTURING_PHASE,
    AT_TARGET,
    BUBBLING_PHASE,
};

class Event : public Object {
    friend class EventDispatcher;

public:
    Event(const ptr<UniStr> &type, bool bubbles = false, bool cancelable = false) noexcept;
    Event(const char *type, bool bubbles = false, bool cancelable = false) noexcept;
    Event(const std::string &type, bool bubbles = false, bool cancelable = false) noexcept;
    
    virtual ptr<Event> clone() {
        return object<Event>(_type, _bubbles, _cancelable);
    }

    Object *target() const noexcept {
        return _target;
    }

    Object *currentTarget() const noexcept {
        return _currentTarget;
    }

    UniStr *type() const noexcept {
        return _type;
    }

    bool bubbles() const noexcept {
        return _bubbles;
    }

    bool isDefaultPrevented() const noexcept {
        return _isDefaultPrevented;
    }

    EventPhase eventPhase() const noexcept {
        return _eventPhase;
    }

    bool cancelable() const noexcept {
        return _cancelable;
    }

    void preventDefault() noexcept {
        if (_cancelable) {
            _isDefaultPrevented = true; 
        }
    }

    void stopImmediatePropagation() noexcept {
        _stop = StopType::IMMEDIATE;
    }

    void stopPropagation() noexcept {
        _stop = StopType::STOP;
    }

private:
    enum class StopType {
        NONE,
        STOP,
        IMMEDIATE,
    };

    ptr<Object> _target;
    ptr<Object> _currentTarget;
    ptr<UniStr> _type;
    StopType    _stop;
    bool        _isDefaultPrevented;
    bool        _cancelable;
    bool        _bubbles;
    EventPhase  _eventPhase;
    unsigned    _dispatchNum;

public:
    /**
     * @brief The Event::ACTIVATE defines the unistr of an activate 
     *        event object.
     *  
     * @note This event has neither a "capture phase" nor a "bubble 
     *       phase", which means that event listeners must be added
     *       directly to any potential targets, whether the target
     *       is on the display list or not.
     *  
     * This event has the following properties: 
     *  
     *  | Property      | Value
     *  |:--------------|:------
     *  | bubbles       | false
     *  | cancelable    | false; there is no default behavior to cancel.
     *  | currentTarget | The object that is actively processing the Event object with an event listener.
     *  | target        | Any DisplayObject instance with a listener registered for the activate event.
     *  
     */
    GV_STATIC_UNISTR(ACTIVATE);

    /**
     * @brief The Event::ADDED defines the unistr of an added event 
     *        object.
     *  
     * This event has the following properties: 
     *  
     *  | Property      | Value
     *  |:--------------|:------
     *  | bubbles       | true
     *  | cancelable    | false; there is no default behavior to cancel.
     *  | currentTarget | The object that is actively processing the Event object with an event listener.
     *  | target        | The DisplayObject instance being added to the display list. The target is not always the object in the display list that registered the event listener. Use the currentTarget property to access the object in the display list that is currently processing the event.
     *  
     */
    GV_STATIC_UNISTR(ADDED);
    
    /**
     * @brief The Event::ADD_TO_STAGE defines the unistr of an addedToStage  
     *        event object.
     *  
     * This event has the following properties: 
     *  
     *  | Property      | Value
     *  |:--------------|:------
     *  | bubbles       | true
     *  | cancelable    | false; there is no default behavior to cancel.
     *  | currentTarget | The object that is actively processing the Event object with an event listener.
     *  | target        | The DisplayObject instance being added to the on stage display list, either directly or through the addition of a sub tree in which the DisplayObject instance is contained. If the DisplayObject instance is being directly added, the added event occurs before this event.
     *  
     */
    GV_STATIC_UNISTR(ADD_TO_STAGE);

    /**
     * @brief The Event::CANCEL defines the unistr of an cancel 
     *        event object.
     *  
     * This event has the following properties: 
     *  
     *  | Property      | Value
     *  |:--------------|:------
     *  | bubbles       | true
     *  | cancelable    | false; there is no default behavior to cancel.
     *  | currentTarget | The object that is actively processing the Event object with an event listener.
     *  | target        | A reference to the object on which the operation is canceled.
     *  
     */
    GV_STATIC_UNISTR(CANCEL);
    
    /**
     * @brief The Event::CHANGE defines the unistr of an change 
     *        event object.
     *  
     * This event has the following properties: 
     *  
     *  | Property      | Value
     *  |:--------------|:------
     *  | bubbles       | true
     *  | cancelable    | false; there is no default behavior to cancel.
     *  | currentTarget | The object that is actively processing the Event object with an event listener.
     *  | target        | The object that has had its value modified. The target is not always the object in the display list that registered the event listener. Use the currentTarget property to access the object in the display list that is currently processing the event.
     *  
     */
    GV_STATIC_UNISTR(CHANGE);
    
        
}; 

GV_NS_END

#endif
