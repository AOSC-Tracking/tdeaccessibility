##### phrasebooks

foreach( _lang de en nl sv )
    install(
        FILES
            ${_lang}-courteousness.phrasebook
            ${_lang}-greetings.phrasebook
            ${_lang}-howareyou.phrasebook
            ${_lang}-personal.phrasebook

        DESTINATION ${DATA_INSTALL_DIR}/kmouth/books/${_lang}
    )

    tde_create_translated_desktop(
        SOURCE ${_lang}.desktop
        DESTINATION ${DATA_INSTALL_DIR}/kmouth/books/${_lang}
        PO_DIR kmouth-desktops
    )
endforeach()
