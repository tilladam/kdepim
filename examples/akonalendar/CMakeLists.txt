include_directories(${Boost_INCLUDE_DIRS})

set(calendar_bin_SRCS
    calendar.cpp
    mainwindow.cpp
    mainwidget.cpp
)


kde4_add_executable(akonalendar_bin ${calendar_bin_SRCS})
set_target_properties(akonalendar_bin PROPERTIES OUTPUT_NAME akonalendar)

target_link_libraries(akonalendar_bin ${KDEPIMLIBS_AKONADI_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KCMUTILS_LIBS} calendarsupport)

#install(TARGETS akonalendar_bin ${INSTALL_TARGETS_DEFAULT_ARGS})
