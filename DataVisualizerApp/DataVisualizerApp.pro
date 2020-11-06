QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
#DEFINES += -DCURL_STATICLIB
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += C:\Users\Lenovo\Documents\QtCreator\praksa\DataVisualizerApp\libs\curl-curl-7_73_0\builds\libcurl-vc-x64-release-dll-ipv6-sspi-schannel\include

#libs/curl-curl-7_73_0/builds/libcurl-vc-x64-release-dll-ipv6-sspi-schannel/include
#    libs/brotli-1.0.9-win64-mingw/include/brotli" \
#    libs/libssh2-1.9.0-win64-mingw/include/" \
#    libs/nghttp2-1.41.0-win64-mingw/include/nghttp2" \
#    libs/openssl-1.1.1h-win64-mingw/include/openssl" \
#    libs/zlib-1.2.11-win64-mingw" \
#    libs/zstd-1.4.5-win64-mingw/include/"

LIBS += -L"C:\Users\Lenovo\Documents\QtCreator\praksa\DataVisualizerApp\libs\curl-curl-7_73_0\builds\libcurl-vc-x64-release-dll-ipv6-sspi-schannel-obj-lib" -llibcurl
#    -L"libs/brotli-1.0.9-win64-mingw/lib" -lbrotli \
#    -L"libs/libssh2-1.9.0-win64-mingw/lib" -llibssh2 \
#    -L"libs/nghttp2-1.41.0-win64-mingw/lib" -lnghttp \
#    -L"libs/openssl-1.1.1h-win64-mingw/lib" -lopenssl \
#    -L"libs/zstd-1.4.5-win64-mingw/lib" -lzstd


SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
