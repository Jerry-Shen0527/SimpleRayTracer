Ubpa_GetTargetName(Integrator "${PROJECT_SOURCE_DIR}/src/Integrators")
Ubpa_GetTargetName(Geometry "${PROJECT_SOURCE_DIR}/src/Geometry")
Ubpa_GetTargetName(Tools "${PROJECT_SOURCE_DIR}/src/Tools")
Ubpa_GetTargetName(BRDF "${PROJECT_SOURCE_DIR}/src/BRDF")

Ubpa_AddTarget(
  TEST  
  MODE EXE
  LIB ${Integrator} ${Geometry} ${Tools} ${BRDF}
)