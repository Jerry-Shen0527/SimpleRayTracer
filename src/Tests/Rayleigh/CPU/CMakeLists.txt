Ubpa_GetTargetName(Tools "${PROJECT_SOURCE_DIR}/src/Tools")


Ubpa_AddTarget(
  TEST  
  MODE EXE
  LIB ${Tools}
) 