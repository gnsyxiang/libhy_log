dnl ===============================================================
dnl 
dnl Release under GPLv-3.0.
dnl 
dnl @file    select_vender.m4
dnl @brief   
dnl @author  gnsyxiang <gnsyxiang@163.com>
dnl @date    10/03 2022 14:49
dnl @version v0.0.1
dnl 
dnl @since    note
dnl @note     note
dnl 
dnl     change log:
dnl     NO.     Author              Date            Modified
dnl     00      zhenquan.qiu        10/03 2022      create the file
dnl 
dnl     last modified: 10/03 2022 14:49
dnl ===============================================================

# SELECT_VENDER()
# --------------------------------------------------------------
# select vender

AC_DEFUN([SELECT_VENDER],
    [
        vender=""
        run_os=""

        AC_ARG_WITH([vender],
                    [AS_HELP_STRING([--with-vender=@<:@pc|nxp|rock-chips|esp32|stm32|fullhan|eeasytech|arterytek|hdhc@:>@],
                                    [select vender about @<:@pc|nxp|esp32|stm32|rock-chips|fullhan|eeasytech|arterytek|hdhc@:>@ @<:@default=pc@:>@])],
                    [],
                    [with_vender=pc])

        case "$with_vender" in
            pc)
                AC_DEFINE(HAVE_SELECT_VENDER_PC,  1, [select pc vender])
                AC_DEFINE(HAVE_SELECT_OS_LINUX,  1, [select linux os])
                vender="pc"
                run_os="linux"
            ;;
            eeasytech)
                AC_DEFINE(HAVE_SELECT_VENDER_EEASYTECH,  1, [select eeasytech vender])
                AC_DEFINE(HAVE_SELECT_OS_LINUX,  1, [select linux os])
                vender="eeasytech"
                run_os="linux"
            ;;
            fullhan)
                AC_DEFINE(HAVE_SELECT_VENDER_FULLHAN,  1, [select fullhan vender])
                AC_DEFINE(HAVE_SELECT_OS_LINUX,  1, [select linux os])
                vender="fullhan"
                run_os="linux"
            ;;
            rock-chips)
                AC_DEFINE(HAVE_SELECT_VENDER_ROCK_CHIPS,  1, [select rock-chips vender])
                AC_DEFINE(HAVE_SELECT_OS_LINUX,  1, [select linux os])
                vender="rock-chips"
                run_os="linux"
            ;;
            nxp)
                AC_DEFINE(HAVE_SELECT_VENDER_NXP,  1, [select nxp vender])
                AC_DEFINE(HAVE_SELECT_OS_LINUX,  1, [select linux os])
                vender="nxp"
                run_os="linux"
            ;;
            stm32)
                AC_DEFINE(HAVE_SELECT_VENDER_STM32,  1, [select stm32 vender])
                AC_DEFINE(HAVE_SELECT_OS_MCU,  1, [select mcu os])
                vender="stm32"
                run_os="mcu"
            ;;
            esp32)
                AC_DEFINE(HAVE_SELECT_VENDER_ESP32,  1, [select esp32 vender])
                AC_DEFINE(HAVE_SELECT_OS_MCU,  1, [select mcu os])
                vender="esp32"
                run_os="mcu"
            ;;
            arterytek)
                AC_DEFINE(HAVE_SELECT_VENDER_ARTERYTEK,  1, [select arterytek vender])
                AC_DEFINE(HAVE_SELECT_OS_MCU,  1, [select mcu os])
                vender="arterytek"
                run_os="mcu"
            ;;
            hdhc)
                AC_DEFINE(HAVE_SELECT_VENDER_HDHC,  1, [select hdhc vender])
                AC_DEFINE(HAVE_SELECT_OS_MCU,  1, [select mcu os])
                vender="hdhc"
                run_os="mcu"
            ;;
            *)
                AC_MSG_ERROR([bad value ${with_vender} for --with-vender=@<:@pc|nxp|esp32|stm32|rock-chips|fullhan|eeasytech|arterytek|hdhc@:>@])
            ;;
        esac

        AC_SUBST(vender)
        AC_SUBST(run_os)

        AM_CONDITIONAL([COMPILE_SELECT_VENDER_PC],          [test "x$with_vender" = "xpc"])
        AM_CONDITIONAL([COMPILE_SELECT_VENDER_EEASYTECH],   [test "x$with_vender" = "xeeasytech"])
        AM_CONDITIONAL([COMPILE_SELECT_VENDER_FULLHAN],     [test "x$with_vender" = "xfullhan"])
        AM_CONDITIONAL([COMPILE_SELECT_VENDER_ROCK_CHIPS],  [test "x$with_vender" = "xrock-chips"])
        AM_CONDITIONAL([COMPILE_SELECT_VENDER_NXP],         [test "x$with_vender" = "xnxp"])
        AM_CONDITIONAL([COMPILE_SELECT_VENDER_STM32],       [test "x$with_vender" = "xstm32"])
        AM_CONDITIONAL([COMPILE_SELECT_VENDER_ESP32],       [test "x$with_vender" = "xesp32"])
        AM_CONDITIONAL([COMPILE_SELECT_VENDER_ARTERYTEK],   [test "x$with_vender" = "xarterytek"])
        AM_CONDITIONAL([COMPILE_SELECT_VENDER_HDHC],        [test "x$with_vender" = "xhdhc"])
    ])

