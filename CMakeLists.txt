#rights reserved
cmake_minimum_required(VERSION 2.8)

#project name
set(PROJECT_NAME "libxcom")
option(LIBXCOM_TEST "test mode" ON)
set(CMAKE_CXX_FLAGS "-fPIC -std=gnu++0x")

# get git branch
macro(get_git_branch _git_branch)
    find_package(Git QUIET)
    if(GIT_FOUND)
      execute_process(
        COMMAND ${GIT_EXECUTABLE} symbolic-ref -q HEAD
        OUTPUT_VARIABLE ${_git_branch}
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        WORKING_DIRECTORY
          ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif()
endmacro()

# 获取当前的分支
set(GIT_BRANCH "")
get_git_branch(GIT_BRANCH)
string(REGEX REPLACE ".*/(.*)" "\\1" _git_branch "${GIT_BRANCH}")
message(STATUS "Git branch is ${_git_branch}")                     # 宏的结束

add_definitions(-DLIBXCOM_VERSION=\"${_git_branch}\")
add_definitions(-D__STDC_FORMAT_MACROS)

#设置相对路径
set(CMAKE_SUPPRESS_REGENERATION 1)
set(CMAKE_USE_RELATIVE_PATHS ON)
project(${PROJECT_NAME})

add_subdirectory(utils)
add_subdirectory(libxcom)
if(LIBXCOM_TEST)
    enable_testing()
    add_subdirectory(test)
endif()

