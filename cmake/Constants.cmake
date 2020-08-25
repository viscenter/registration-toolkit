# The prefix under which includes will be installed
set(proj_prefix "rt")

# Header install directory. Lowercase so includes look like:
#
#     #include <proj/module-1/ClassExample.h>
#
set(include_install_dir "include/${proj_prefix}")

# CMake config files
set(config_install_dir "lib/cmake/${PROJECT_NAME}")

# Extra resources
set(share_install_dir "share/${PROJECT_NAME}")

# Targets export name (Config)
set(targets_export_name "${PROJECT_NAME}Targets")
set(namespace "${proj_prefix}::")

# Get Git hash
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)
if(GIT_SHA1)
    string(SUBSTRING ${GIT_SHA1} 0 7 GIT_SHA1_SHORT)
else()
    set(GIT_SHA1 UNTRACKED)
    set(GIT_SHA1_SHORT UNTRACKED)
endif()