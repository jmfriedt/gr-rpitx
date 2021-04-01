INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_RPITX rpitx)

FIND_PATH(
    RPITX_INCLUDE_DIRS
    NAMES rpitx/api.h
    HINTS $ENV{RPITX_DIR}/include
        ${PC_RPITX_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    RPITX_LIBRARIES
    NAMES gnuradio-rpitx
    HINTS $ENV{RPITX_DIR}/lib
        ${PC_RPITX_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/rpitxTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(RPITX DEFAULT_MSG RPITX_LIBRARIES RPITX_INCLUDE_DIRS)
MARK_AS_ADVANCED(RPITX_LIBRARIES RPITX_INCLUDE_DIRS)
