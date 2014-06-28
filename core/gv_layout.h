#ifndef __GV_LAYOUT_H__
#define __GV_LAYOUT_H__

#include "gv_plantform.h"

GV_NS_BEGIN


/**
 * @biref A value from the Align enum that specifies the 
 *        alignment of the child object in parent container. The
 *        following are valid values:
 *  
 * | Value             | Vertical  | Horizontal |
 * |:------------------|:----------|:-----------|
 * | Align.TOP         | Top       | Center     |
 * | Align.BOTTOM      | Bottom    | Center     |
 * | Align.LEFT        | Center    | Left       |
 * | Align.RIGHT       | Center    | Right      |
 * | Align.CENTER      | Center    | Center     |
 * | Align.TOP_LEFT    | Top       | Left       |
 * | Align.TOP_RIGHT   | Top       | Right      |
 * | Align.BOTTOM_LEFT | Bottom    | Left       |
 * | Align.BOTTOM_RIGHT| Bottom    | Right      |
 * 
 */
enum class Align {
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    CENTER,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
};

GV_NS_END

#endif
