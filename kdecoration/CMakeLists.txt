add_definitions(-DTRANSLATION_DOMAIN="teamsquirtle_kwin_deco")

find_package(KF5 REQUIRED COMPONENTS CoreAddons GuiAddons ConfigWidgets WindowSystem I18n)
find_package(Qt5 CONFIG REQUIRED COMPONENTS DBus)

### XCB
find_package(XCB COMPONENTS XCB)
set_package_properties(XCB PROPERTIES
  DESCRIPTION "X protocol C-language Binding"
  URL "http://xcb.freedesktop.org"
  TYPE OPTIONAL
  PURPOSE "Required to pass style properties to native Windows on X11 Platform"
)

if(UNIX AND NOT APPLE)

  set(TEAMSQUIRTLE_HAVE_X11 ${XCB_XCB_FOUND})
  if (XCB_XCB_FOUND)
    find_package(Qt5 REQUIRED CONFIG COMPONENTS X11Extras)
  endif()

else()

  set(TEAMSQUIRTLE_HAVE_X11 FALSE)

endif()

################# configuration #################
configure_file(config-teamsquirtle.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-teamsquirtle.h )

################# newt target #################
### plugin classes
set(teamsquirtledecoration_SRCS
    teamsquirtlebutton.cpp
    teamsquirtledecoration.cpp
    teamsquirtleexceptionlist.cpp
    teamsquirtlesettingsprovider.cpp
    teamsquirtlesizegrip.cpp)

kconfig_add_kcfg_files(teamsquirtledecoration_SRCS teamsquirtlesettings.kcfgc)

### config classes
### they are kept separately because they might move in a separate library in the future
set(teamsquirtledecoration_config_SRCS
    config/teamsquirtleconfigwidget.cpp
    config/teamsquirtledetectwidget.cpp
    config/teamsquirtleexceptiondialog.cpp
    config/teamsquirtleexceptionlistwidget.cpp
    config/teamsquirtleexceptionmodel.cpp
    config/teamsquirtleitemmodel.cpp
)

set(teamsquirtledecoration_config_PART_FORMS
   config/ui/teamsquirtleconfigurationui.ui
   config/ui/teamsquirtledetectwidget.ui
   config/ui/teamsquirtleexceptiondialog.ui
   config/ui/teamsquirtleexceptionlistwidget.ui
)

ki18n_wrap_ui(teamsquirtledecoration_config_PART_FORMS_HEADERS ${teamsquirtledecoration_config_PART_FORMS})

### build library
add_library(teamsquirtledecoration MODULE
    ${teamsquirtledecoration_SRCS}
    ${teamsquirtledecoration_config_SRCS}
    ${teamsquirtledecoration_config_PART_FORMS_HEADERS})

target_link_libraries(teamsquirtledecoration
    PUBLIC
        Qt5::Core
        Qt5::Gui
        Qt5::DBus
    PRIVATE
        KDecoration2::KDecoration
        KF5::ConfigCore
        KF5::CoreAddons
        KF5::ConfigWidgets
        KF5::GuiAddons
        KF5::I18n
        KF5::WindowSystem)

if(TEAMSQUIRTLE_HAVE_X11)
  target_link_libraries(teamsquirtledecoration
    PUBLIC
      Qt5::X11Extras
      XCB::XCB)
endif()


install(TARGETS teamsquirtledecoration DESTINATION ${PLUGIN_INSTALL_DIR}/org.kde.kdecoration2)
install(FILES config/teamsquirtledecorationconfig.desktop DESTINATION  ${SERVICES_INSTALL_DIR})
