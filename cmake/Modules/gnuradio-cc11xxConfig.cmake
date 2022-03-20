find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_CC11XX gnuradio-cc11xx)

FIND_PATH(
    GR_CC11XX_INCLUDE_DIRS
    NAMES gnuradio/cc11xx/api.h
    HINTS $ENV{CC11XX_DIR}/include
        ${PC_CC11XX_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_CC11XX_LIBRARIES
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

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-cc11xxTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_CC11XX DEFAULT_MSG GR_CC11XX_LIBRARIES GR_CC11XX_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_CC11XX_LIBRARIES GR_CC11XX_INCLUDE_DIRS)
