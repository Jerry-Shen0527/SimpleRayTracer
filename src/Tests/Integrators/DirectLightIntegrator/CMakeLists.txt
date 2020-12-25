Ubpa_GetTargetName(Integrator "${PROJECT_SOURCE_DIR}/src/Integrators")
Ubpa_AddTarget(
  TEST  
  MODE EXE
  LIB ${Integrator}
)  