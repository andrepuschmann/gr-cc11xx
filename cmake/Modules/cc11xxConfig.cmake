INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_CC11XX cc11xx)

FIND_PATH(
    CC11XX_INCLUDE_DIRS
    NAMES cc11xx/api.h
    HINTS $ENV{CC11XX_DIR}/include
        ${PC_CC11XX_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    CC11XX_LIBRARIES
    NAMES gnuradio-cc11xx
    HINTS $ENV{CC11XX_DIR}/lib
        ${PC_CC11XX_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CC11XX DEFAULT_MSG CC11XX_LIBRARIES CC11XX_INCLUDE_DIRS)
MARK_AS_ADVANCED(CC11XX_LIBRARIES CC11XX_INCLUDE_DIRS)

