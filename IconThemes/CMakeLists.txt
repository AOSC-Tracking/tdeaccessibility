##### index theme

tde_create_translated_desktop(
    SOURCE mono/index.theme
    DESTINATION ${ICON_INSTALL_DIR}/mono
    PO_DIR icon-themes
)


##### svgz icons

set( _icon_type actions apps categories devices mimetypes places )

foreach( icon_type ${_icon_type} )
     file( GLOB _files ${CMAKE_CURRENT_SOURCE_DIR}/mono/scalable/${icon_type}/*.svgz )
     install( FILES ${_files} DESTINATION ${ICON_INSTALL_DIR}/mono/scalable/${icon_type} )
endforeach( icon_type )


##### png icons

set( _icon_size 16x16 22x22 32x32 48x48 64x64 96x96 128x128 192x192 256x256 )

foreach( icon_size ${_icon_size} )
     file( GLOB _files ${CMAKE_CURRENT_SOURCE_DIR}/mono/png/${icon_size}/actions/*.png )
     install( FILES ${_files} DESTINATION ${ICON_INSTALL_DIR}/mono/png/${icon_size}/actions )
endforeach( icon_size )
