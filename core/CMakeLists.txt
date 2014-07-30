
add_library(opengv STATIC
    codecs/gv_png.cpp
    gv_displayobject.cpp
    gv_displayobjectcontainer.cpp
    gv_env.cpp
    gv_event.cpp
    gv_eventdispatcher.cpp
    gv_file.cpp
    gv_graphics.cpp
    gv_image.cpp
    gv_log.cpp
    gv_math.cpp
    gv_object.cpp
    gv_path.cpp
    gv_pixel.cpp
    gv_primitive.cpp
    gv_rbtree.cpp
    gv_stage.cpp
    gv_texture.cpp
    gv_unistr.cpp
    gv_interactiveobject.cpp
    gv_renderer.cpp
    gv_shape.cpp
    gv_xml.cpp
    opengxv.cpp
)
	
add_definitions(-D__LIBOPENGXV__)

target_link_libraries(opengv
    ${OPENGV_LIBDEPS}
)

