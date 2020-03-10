isEmpty(PLUGIN_PRI) {
  exists($$[QT_INSTALL_PREFIX]/include/nymea/plugin.pri) {
    include($$[QT_INSTALL_PREFIX]/include/nymea/plugin.pri)
  } else {
    exists(/usr/include/nymea/plugin.pri) {
    include(/usr/include/nymea/plugin.pri)
  } else {
    message("plugin.pri not found. Either install libnymea1-dev or use the PLUGIN_PRI argument to point to it.")
    message("For building this project without nymea installed system-wide, you will want to export those variables in addition:")
    message("PKG_CONFIG_PATH=/path/to/build-nymea/libnymea/pkgconfig/")
    message("CPATH=/path/to/nymea/libnymea/")
    message("LIBRARY_PATH=/path/to/build-nymea/libnymea/")
    message("PATH=/path/to/build-nymea/tools/nymea-plugininfocompiler:$PATH")
    message("LD_LIBRARY_PATH=/path/to/build-nymea/libnymea/")
    error("plugin.pri not found. Cannot continue")
  }
}
} else {
  include($$PLUGIN_PRI)
}

TARGET = $$qtLibraryTarget(nymea_devicepluginunipi)

LIBS += \
    -li2c \

QT += \
    network \
    serialport \
    serialbus \

SOURCES += \
    devicepluginunipi.cpp \
    neuron.cpp \
    neuronextension.cpp \
    mcp23008.cpp \
    i2cport.cpp \
    unipi.cpp \
    mcp3422.cpp

HEADERS += \
    devicepluginunipi.h \
    neuron.h \
    neuronextension.h \
    mcp23008.h \
    i2cport.h \
    unipi.h \
    i2cport_p.h \
    mcp3422.h

MAP_FILES.files = files(modbus_maps/*)
MAP_FILES.path = [QT_INSTALL_PREFIX]/share/nymea/modbus/
INSTALLS += MAP_FILES

