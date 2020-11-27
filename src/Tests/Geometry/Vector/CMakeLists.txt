Ubpa_GetTargetName(Geometry "${PROJECT_SOURCE_DIR}/src/Geometry")
Ubpa_AddTarget(
  TEST
  MODE EXE
  LIB ${Geometry}
)  