include_directories(
    ${CMAKE_SOURCE_DIR}/incidenceeditor-ng
    ${CMAKE_SOURCE_DIR}/calendarsupport
    ${CMAKE_BINARY_DIR}/calendarsupport
    ${CMAKE_SOURCE_DIR}/libkdepim/
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${Boost_INCLUDE_DIR}
)

set(mobileui_calendar_SRCS
    calendarhelper.cpp
    clockhelper.cpp
    declarativeeditors.cpp
    groupwareuidelegate.cpp
    incidenceview.cpp
    kcalitembrowseritem.cpp
)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}" )

kde4_add_ui_files(mobileui_calendar_SRCS
  ${CMAKE_SOURCE_DIR}/incidenceeditor-ng/dialogmobile.ui
  ${CMAKE_SOURCE_DIR}/incidenceeditor-ng/dialogmoremobile.ui
)

add_definitions( -DKDEPIMMOBILEUICALENDAR_STATIC_LIB )
kde4_add_library(kdepimmobileuicalendar STATIC ${mobileui_calendar_SRCS})

target_link_libraries(kdepimmobileuicalendar
  ${KDEPIMLIBS_AKONADI_LIBS}
  ${KDEPIMLIBS_KCALCORE_LIBS}
  akonadi-calendar
  akonadi_next
  incidenceeditorsngmobile
  calendarsupport
  kdepimmobileui
  ${QT_QTDECLARATIVE_LIBRARY}
)

install(FILES incidence-editor.qml DESTINATION ${DATA_INSTALL_DIR}/mobileui)
install(FILES
  CalendarDialog.qml
  ClockDialog.qml
  MorePanel.qml
  DESTINATION ${PLUGIN_INSTALL_DIR}/imports/org/kde/pim/mobileui)
