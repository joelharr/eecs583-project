add_llvm_library(TestPass MODULE	
  TestPass.cpp

  PLUGIN_TOOL
  opt
)
add_dependencies(TestPass intrinsics_gen)
