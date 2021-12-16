add_llvm_library(ProfileSLP MODULE	
  ProfileSLP.cpp
  Superblock.cpp
  SLP.cpp
  Vectorize.cpp
  Reorder.cpp

  PLUGIN_TOOL
  opt
)
