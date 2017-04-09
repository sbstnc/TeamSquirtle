set(TEAMSQUIRTLE_USE_KDE4 ${USE_KDE4})
if(TEAMSQUIRTLE_USE_KDE4)

  ############ Language and toolchain features
  ############ copied from ECM
  if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" AND NOT WIN32)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  endif()

endif()

################# Qt/KDE #################
if(TEAMSQUIRTLE_USE_KDE4)

  find_package(KDE4 REQUIRED)
  include(KDE4Defaults)
  include(MacroLibrary)

  add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
  include_directories(${KDE4_INCLUDES})

  ### XCB
  if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(XCB xcb x11-xcb)
    add_feature_info("x11-xcb" XCB_FOUND "Required to pass style properties to native Windows on X11 Platform")
    set_feature_info("x11-xcb" "Required to pass style properties to native Windows on X11 Platform" "http://xcb.freedesktop.org")
    set(TEAMSQUIRTLE_HAVE_X11 ${XCB_FOUND})
  else()
    set(TEAMSQUIRTLE_HAVE_X11 FALSE)
  endif()

  set(TEAMSQUIRTLE_HAVE_KWAYLAND FALSE)

  ### KStyle
  set(TEAMSQUIRTLE_HAVE_KSTYLE FALSE)

else()

  find_package(Qt5 REQUIRED CONFIG COMPONENTS Widgets DBus Quick)
  find_package(KF5 REQUIRED COMPONENTS
      I18n
      Config
      GuiAddons
      ConfigWidgets
      WindowSystem)

  find_package( KF5FrameworkIntegration CONFIG )
  set_package_properties(KF5FrameworkIntegration PROPERTIES
      DESCRIPTION "KF5 Framework Integration"
      URL "https://projects.kde.org/projects/frameworks/frameworkintegration"
      TYPE OPTIONAL
      PURPOSE "Required to use KStyle convenience functionalities in style")

  set(TEAMSQUIRTLE_HAVE_KSTYLE ${KF5FrameworkIntegration_FOUND})

  find_package(XCB COMPONENTS XCB)
  set_package_properties(XCB PROPERTIES
      DESCRIPTION "X protocol C-language Binding"
      URL "http://xcb.freedesktop.org"
      TYPE OPTIONAL
      PURPOSE "Required to pass style properties to native Windows on X11 Platform"
  )

  find_package(KF5Wayland CONFIG)
  set(TEAMSQUIRTLE_HAVE_KWAYLAND ${KF5Wayland_FOUND})

  if(UNIX AND NOT APPLE)

    set(TEAMSQUIRTLE_HAVE_X11 ${XCB_XCB_FOUND})
    if (XCB_XCB_FOUND)
      find_package(Qt5 REQUIRED CONFIG COMPONENTS X11Extras)
    endif()

  else()

    set(TEAMSQUIRTLE_HAVE_X11 FALSE)

  endif()


endif()

################# includes #################
include_directories(
  animations
  debug
)

################# configuration #################
configure_file(config-teamsquirtle.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-teamsquirtle.h )

########### next target ###############
set(teamsquirtle_PART_SRCS
    animations/teamsquirtleanimation.cpp
    animations/teamsquirtleanimations.cpp
    animations/teamsquirtleanimationdata.cpp
    animations/teamsquirtlebaseengine.cpp
    animations/teamsquirtlebusyindicatordata.cpp
    animations/teamsquirtlebusyindicatorengine.cpp
    animations/teamsquirtledialdata.cpp
    animations/teamsquirtledialengine.cpp
    animations/teamsquirtleenabledata.cpp
    animations/teamsquirtlegenericdata.cpp
    animations/teamsquirtleheaderviewdata.cpp
    animations/teamsquirtleheaderviewengine.cpp
    animations/teamsquirtlescrollbardata.cpp
    animations/teamsquirtlescrollbarengine.cpp
    animations/teamsquirtlespinboxengine.cpp
    animations/teamsquirtlespinboxdata.cpp
    animations/teamsquirtlestackedwidgetdata.cpp
    animations/teamsquirtlestackedwidgetengine.cpp
    animations/teamsquirtletabbarengine.cpp
    animations/teamsquirtletabbardata.cpp
    animations/teamsquirtletoolboxengine.cpp
    animations/teamsquirtletransitiondata.cpp
    animations/teamsquirtletransitionwidget.cpp
    animations/teamsquirtlewidgetstateengine.cpp
    animations/teamsquirtlewidgetstatedata.cpp
    debug/teamsquirtlewidgetexplorer.cpp
    teamsquirtleaddeventfilter.cpp
    teamsquirtleframeshadow.cpp
    teamsquirtlehelper.cpp
    teamsquirtlemdiwindowshadow.cpp
    teamsquirtlemnemonics.cpp
    teamsquirtlepropertynames.cpp
    teamsquirtleshadowhelper.cpp
    teamsquirtlesplitterproxy.cpp
    teamsquirtlestyle.cpp
    teamsquirtlestyleplugin.cpp
    teamsquirtletileset.cpp
    teamsquirtlewindowmanager.cpp
)

if(TEAMSQUIRTLE_USE_KDE4)

  kde4_add_kcfg_files(teamsquirtle_PART_SRCS teamsquirtlestyleconfigdata.kcfgc)
  kde4_add_plugin(teamsquirtle ${teamsquirtle_PART_SRCS} kstylekde4compat.cpp)
  target_link_libraries(teamsquirtle  ${KDE4_KDEUI_LIBS})

  if(TEAMSQUIRTLE_HAVE_X11)
    target_link_libraries(teamsquirtle ${X11_XCB_LIBRARIES})
    target_link_libraries(teamsquirtle ${XCB_LIBRARIES})
  endif()

  install(TARGETS teamsquirtle  DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/styles/)

else()

  kconfig_add_kcfg_files(teamsquirtle_PART_SRCS teamsquirtlestyleconfigdata.kcfgc)
  add_library(teamsquirtle MODULE ${teamsquirtle_PART_SRCS})
  target_link_libraries(teamsquirtle Qt5::Core Qt5::Gui Qt5::Widgets Qt5::DBus Qt5::Quick)
  target_link_libraries(teamsquirtle KF5::ConfigCore KF5::ConfigWidgets KF5::GuiAddons KF5::WindowSystem)

  if( KF5FrameworkIntegration_FOUND )
  target_link_libraries(teamsquirtle KF5::Style)
  endif()

  if (WIN32)
    # As stated in http://msdn.microsoft.com/en-us/library/4hwaceh6.aspx M_PI only gets defined
    # when if _USE_MATH_DEFINES is defined
    target_compile_definitions(teamsquirtle PRIVATE _USE_MATH_DEFINES _BSD_SOURCE)
  endif()

  if(TEAMSQUIRTLE_HAVE_X11)
    target_link_libraries(teamsquirtle ${XCB_LIBRARIES})
    target_link_libraries(teamsquirtle Qt5::X11Extras)
  endif()

  if(TEAMSQUIRTLE_HAVE_KWAYLAND)
    target_link_libraries(teamsquirtle KF5::WaylandClient)
  endif()

  install(TARGETS teamsquirtle DESTINATION ${QT_PLUGIN_INSTALL_DIR}/styles/)

endif()

########### install files ###############
install(FILES teamsquirtle.themerc  DESTINATION  ${DATA_INSTALL_DIR}/kstyle/themes)

########### subdirectories ###############
add_subdirectory(config)
