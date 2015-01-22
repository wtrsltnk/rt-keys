set(rt_keys_SRCS
    rt-piano.cpp
    mainwindow.cpp
    piano.cpp
    RtMidi.cpp
    )

set(rt_keys_HEADERS
    mainwindow.h
    piano.h
    RtMidi.h
    RtError.h
    )

set(rt_keys_FORMS
    mainwindow.ui
    piano.ui
    )

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
qt5_wrap_cpp(rt_keys_HEADERS_MOC ${rt_keys_HEADERS})
qt5_wrap_ui(rt_keys_FORMS_HEADERS ${rt_keys_FORMS})

include_directories(
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

add_definitions(-D__WINDOWS_MM__)

link_libraries (winmm)
add_executable(rt-keys ${rt_keys_SRCS} ${rt_keys_HEADERS_MOC} ${rt_keys_FORMS_HEADERS})

qt5_use_modules(rt-keys Widgets Network)
