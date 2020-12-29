Ubpa_GetTargetName(Geometry "${PROJECT_SOURCE_DIR}/src/Geometry")
Ubpa_GetTargetName(Tools "${PROJECT_SOURCE_DIR}/src/Tools")
Ubpa_AddTarget(
  TEST 
  MODE EXE
  LIB ${Geometry} ${Tools}
) 