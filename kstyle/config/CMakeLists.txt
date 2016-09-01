### configuration plugin
add_definitions(-DTRANSLATION_DOMAIN=\"teamsquirtle_style_config\")
set(kstyle_teamsquirtle_config_PART_SRCS teamsquirtlestyleconfig.cpp)

if(TEAMSQUIRTLE_USE_KDE4)

  kde4_add_kcfg_files(kstyle_teamsquirtle_config_PART_SRCS ../teamsquirtlestyleconfigdata.kcfgc)
  kde4_add_ui_files(kstyle_teamsquirtle_config_PART_SRCS ui/teamsquirtlestyleconfig.ui)
  kde4_add_plugin(kstyle_teamsquirtle_config ${kstyle_teamsquirtle_config_PART_SRCS})

  target_link_libraries(kstyle_teamsquirtle_config ${KDE4_KDEUI_LIBS} )
  install(TARGETS kstyle_teamsquirtle_config DESTINATION ${PLUGIN_INSTALL_DIR})

else()

  set(kstyle_teamsquirtle_config_PART_SRCS ${kstyle_teamsquirtle_config_PART_SRCS} teamsquirtlestyleconfigmodule.cpp)

  kconfig_add_kcfg_files(kstyle_teamsquirtle_config_PART_SRCS ../teamsquirtlestyleconfigdata.kcfgc)
  set(kstyle_teamsquirtle_config_PART_FORMS ui/teamsquirtlestyleconfig.ui)
  ki18n_wrap_ui(kstyle_teamsquirtle_config_PART_FORMS_HEADERS ${kstyle_teamsquirtle_config_PART_FORMS})

  add_library(kstyle_teamsquirtle_config MODULE
    ${kstyle_teamsquirtle_config_PART_SRCS}
    ${kstyle_teamsquirtle_config_PART_FORMS_HEADERS}
 )

  target_link_libraries(kstyle_teamsquirtle_config Qt5::Core Qt5::Gui Qt5::Widgets Qt5::DBus)
  target_link_libraries(kstyle_teamsquirtle_config KF5::ConfigCore KF5::ConfigWidgets KF5::I18n)
  install(TARGETS kstyle_teamsquirtle_config DESTINATION ${QT_PLUGIN_INSTALL_DIR})
  install(FILES teamsquirtlestyleconfig.desktop DESTINATION  ${SERVICES_INSTALL_DIR})

endif()

########### next target ###############
if(NOT TEAMSQUIRTLE_USE_KDE4)

  find_package(KF5KCMUtils CONFIG)
  set_package_properties(KF5KCMUtils PROPERTIES
      DESCRIPTION "KF5 KCM utils Framework"
      URL "https://projects.kde.org/projects/frameworks/kcmutils"
      TYPE OPTIONAL
      PURPOSE "Required for teamsquirtle-settings5 application to build")

  if( KF5KCMUtils_FOUND )

      set(teamsquirtle_settings_SOURCES main.cpp)
      add_executable(teamsquirtle-settings5 ${teamsquirtle_settings_SOURCES} )
      target_link_libraries(teamsquirtle-settings5 Qt5::Core Qt5::Gui Qt5::Widgets )
      target_link_libraries(teamsquirtle-settings5 KF5::I18n KF5::KCMUtils)

      install(TARGETS teamsquirtle-settings5 ${INSTALL_TARGETS_DEFAULT_ARGS})

      # icon
      ecm_install_icons(ICONS sc-apps-teamsquirtle-settings.svgz
        DESTINATION ${ICON_INSTALL_DIR}
        THEME hicolor )
    endif()
endif()
