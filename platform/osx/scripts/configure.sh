#!/usr/bin/env bash

BOOST_VERSION=1.59.0
BOOST_LIBPROGRAM_OPTIONS_VERSION=1.59.0
LIBCURL_VERSION=system
GLFW_VERSION=3.1.2
if [ "$PLATFORM" == "qt" ]; then
    LIBJPEG_TURBO_VERSION=1.4.2
fi
SQLITE_VERSION=3.9.1
LIBUV_VERSION=1.7.5
ZLIB_VERSION=system
NUNICODE_VERSION=1.6
GEOJSONVT_VERSION=3.1.0
VARIANT_VERSION=1.1.0
RAPIDJSON_VERSION=1.0.2
GTEST_VERSION=1.7.0
PIXELMATCH_VERSION=0.9.0

function print_opengl_flags {
    CONFIG+="    'opengl_cflags%': [],"$LN
    CONFIG+="    'opengl_ldflags%': ['-framework OpenGL', '-framework CoreFoundation'],"$LN
}

function print_qt_flags {
    mason install Qt system
    CONFIG+="    'qt_cflags%': $(quote_flags $(mason cflags Qt system "QtCore QtGui QtOpenGL QtNetwork QtSql")),"$LN
    CONFIG+="    'qt_ldflags%': $(quote_flags $(mason ldflags Qt system "QtCore QtGui QtOpenGL QtNetwork QtSql")),"$LN

    QT_VERSION_MAJOR=$(qmake -query QT_VERSION | cut -d. -f1)
    if [ ${QT_VERSION_MAJOR} -gt 4 ] ; then
        CONFIG+="    'qt_moc%': '$(pkg-config Qt${QT_VERSION_MAJOR}Core --variable=host_bins)/moc',"$LN
        CONFIG+="    'qt_rcc%': '$(pkg-config Qt${QT_VERSION_MAJOR}Core --variable=host_bins)/rcc',"$LN
    else
        CONFIG+="    'qt_moc%': '$(pkg-config QtCore --variable=moc_location)',"$LN
        CONFIG+="    'qt_rcc%': '$(pkg-config QtCore --variable=rcc_location)',"$LN
    fi
}
