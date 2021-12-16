add_llvm_library(ProfileSLP MODULE	
  ProfileSLP.cpp
  Superblock.cpp
  SLP.cpp
  Vectorize.cpp
  hoist.cpp

  PLUGIN_TOOL
  opt
)
