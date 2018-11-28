        db "Character set: "
        IF LOCALE_CHSET = LOCAL_CHSET_JP
                        db      "JP"
        ENDIF
        IF LOCALE_CHSET = LOCAL_CHSET_US
                db      "US"
                IF LOCALE_CHSET_VAR = LOCAL_CHSET_VAR_BR
                        db      " (var.: BR)"
                ENDIF
        ENDIF
        db      $0D,$0A
        IF VDP = TMS99X8
                db      "Interrupt frequency: "
        ELSE
                db      "Default interrupt freq.: "
        ENDIF
        IF LOCALE_INT = LOCAL_INT_60HZ
                db     "6"
        ELSE
                db     "5"
        ENDIF
        db      "0Hz"
        ; the text fits exactly a line for non-MSX1 VDP
        IF VDP = TMS99X8
                db      $0D,$0A
        ENDIF
        db      "Keyboard type: "
        IF LOCALE_KBD = LOCAL_KBD_US
                db      "US"
        ENDIF
        IF LOCALE_KBD = LOCAL_KBD_UK
                db      "UK"
        ENDIF
        IF LOCALE_KBD = LOCAL_KBD_FR
                db      "FR"
        ENDIF
        IF LOCALE_KBD = LOCAL_KBD_DE
                db      "DE"
        ENDIF
        IF LOCALE_KBD = LOCAL_KBD_JP
                db      "JP"
        ENDIF
        db      $0D,$0A
        ;BASIC type is not very interesting without BASIC
        ;db      "BASIC type: "
        ;IF LOCALE_BASIC = LOCAL_BASIC_US
        ;        db      "US"
        ;ENDIF
        ;IF LOCALE_BASIC = LOCAL_BASIC_JP
        ;        db      "JP"
        ;ENDIF
        db      $0D,$0A
