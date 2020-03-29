tde_conditional_add_subdirectory( WITH_ARTS      artsplayer  )
tde_conditional_add_subdirectory( WITH_AKODE     akodeplayer )
tde_conditional_add_subdirectory( WITH_ALSA      alsaplayer  )
tde_conditional_add_subdirectory( WITH_GSTREAMER gstplayer   )


##### other data

tde_create_translated_desktop(
    SOURCE kttsd_audioplugin.desktop
    DESTINATION ${SERVICETYPES_INSTALL_DIR}
)
