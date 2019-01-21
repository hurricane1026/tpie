macro(find_simple_library FLI_NAME)
  cmake_parse_arguments(FLI "" "" "HEADER;LIBRARY" ${ARGN})
  string(TOUPPER "${FLI_NAME}" FLI_UNAME)

  find_path(${FLI_UNAME}_INCLUDE_DIR NAMES ${FLI_HEADER} )
  find_library(${FLI_UNAME}_LIBRARY NAMES ${FLI_LIBRARY} )
  
  find_package_handle_standard_args(${FLI_UNAME} REQUIRED_VARS ${FLI_UNAME}_LIBRARY ${FLI_UNAME}_INCLUDE_DIR)
  mark_as_advanced(${FLI_UNAME}_LIBRARY ${FLI_UNAME}_INCLUDE_DIR)

  if (${${FLI_UNAME}_FOUND})
    set(FLI_INAME ${FLI_NAME}::${FLI_NAME})
    add_library(${FLI_INAME} UNKNOWN IMPORTED)
    set_target_properties(${FLI_INAME} PROPERTIES
      INTERFACE_INCLUDE_DIRECTORIES "${${FLI_UNAME}_INCLUDE_DIR}"
      IMPORTED_LOCATION "${${FLI_UNAME}_LIBRARY}")
   endif()
endmacro()
  

