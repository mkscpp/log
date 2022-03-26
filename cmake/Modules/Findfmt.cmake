find_path(fmt_INCLUDE_DIR NAMES fmt/format.h)

if (fmt_INCLUDE_DIR)
    set(_fmt_version_file "${fmt_INCLUDE_DIR}/fmt/core.h")
    if (NOT EXISTS "${_fmt_version_file}")
        set(_fmt_version_file "${fmt_INCLUDE_DIR}/fmt/format.h")
    endif ()
    if (EXISTS "${_fmt_version_file}")
        # parse "#define FMT_VERSION 40100" to 4.1.0
        file(STRINGS "${_fmt_version_file}" fmt_VERSION_LINE
                REGEX "^#define[ \t]+FMT_VERSION[ \t]+[0-9]+$")
        string(REGEX REPLACE "^#define[ \t]+FMT_VERSION[ \t]+([0-9]+)$"
                "\\1" fmt_VERSION "${fmt_VERSION_LINE}")
        foreach (ver "fmt_VERSION_PATCH" "fmt_VERSION_MINOR" "fmt_VERSION_MAJOR")
            math(EXPR ${ver} "${fmt_VERSION} % 100")
            math(EXPR fmt_VERSION "(${fmt_VERSION} - ${${ver}}) / 100")
        endforeach ()
        set(fmt_VERSION
                "${fmt_VERSION_MAJOR}.${fmt_VERSION_MINOR}.${fmt_VERSION_PATCH}")
    endif ()
endif ()

set(fmt_LIB_NAME)
# vcpkg has some problesm with find_package and fallbacks to our script
if(_VCPKG_INSTALLED_DIR)
    set(VCPKG_PATH ${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET})
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        # Debug libs
        message(STATUS "fmt type Debug")
        set(fmt_LIB_PATH ${fmt_LIB_PATH} ${VCPKG_PATH}/debug/lib)
        set(fmt_LIB_NAME ${fmt_LIB_NAME} fmtd)
    else()
        # Release libs
        set(fmt_LIB_PATH ${fmt_LIB_PATH} ${VCPKG_PATH}/lib)
        set(fmt_LIB_NAME ${fmt_LIB_NAME} fmt)
    endif (CMAKE_BUILD_TYPE STREQUAL "Debug")
else()
    set(fmt_LIB_NAME fmt)
endif()

message(STATUS "fmt_LIB_NAME: ${fmt_LIB_NAME}")
message(STATUS "fmt_LIB_PATH: ${fmt_LIB_PATH}")

find_library(fmt_LIBRARY NAMES ${fmt_LIB_NAME}
        HINTS
        /usr/lib
        /usr/local/lib
        /opt/lib
        /opt/local/lib
        ${fmt_LIB_PATH}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(fmt
        REQUIRED_VARS fmt_INCLUDE_DIR fmt_LIBRARY
        VERSION_VAR fmt_VERSION)
mark_as_advanced(
        fmt_INCLUDE_DIR
        fmt_LIBRARY
        fmt_VERSION_MAJOR
        fmt_VERSION_MINOR
        fmt_VERSION_PATCH
        fmt_VERSION_STRING)
if (fmt_FOUND AND NOT (TARGET fmt::fmt))
    add_library(fmt::fmt UNKNOWN IMPORTED)
    set_target_properties(fmt::fmt PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${fmt_INCLUDE_DIR}"
            IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
            IMPORTED_LOCATION "${fmt_LIBRARY}")
endif ()