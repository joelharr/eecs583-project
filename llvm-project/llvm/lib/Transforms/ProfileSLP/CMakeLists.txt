add_llvm_library(ProfileSLP MODULE	
  ProfileSLP.cpp
  Superblock.cpp
  SLP.cpp

  PLUGIN_TOOL
  opt
)
