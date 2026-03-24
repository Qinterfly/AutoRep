
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets LinguistTools)
find_package(magic_enum CONFIG REQUIRED)

set(FRONTEND_HEADERS
    uiconstants.h
    uiutility.h
    mainwindow.h
    customstatusbar.h
    customtable.h
    customtabwidget.h
    customlineedit.h
    customplot.h
)

set(FRONTEND_SOURCES
    uiutility.cpp
    mainwindow.cpp
    customstatusbar.cpp
    customtable.cpp
    customtabwidget.cpp
    customlineedit.cpp
    customplot.cpp
)

qt_add_library(frontend STATIC
    ${FRONTEND_HEADERS}
    ${FRONTEND_SOURCES}
    ${RESOURCES_FILE}
)

target_link_libraries(frontend PRIVATE
    Qt::Core
    Qt::Gui
    Qt::Widgets
    magic_enum::magic_enum
    ${TESTLAB_LIB}
    qtpropertybrowser
    qcustomplot
)

target_include_directories(frontend PUBLIC ${CMAKE_BINARY_DIR})
target_include_directories(frontend INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

qt_add_translations(frontend
    TS_FILES "${TRANSLATIONS_DIR}/application_ru.ts" "${TRANSLATIONS_DIR}/application_en.ts"
    RESOURCE_PREFIX "/translations"
    -no-obsolete
)

