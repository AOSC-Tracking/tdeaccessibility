FILE( GLOB EPS_FILES *.eps)

INSTALL(
    FILES ${EPS_FILES}
    DESTINATION ${HTML_INSTALL_DIR}/en/kmouth
)

tde_create_handbook( DESTINATION kmouth )
