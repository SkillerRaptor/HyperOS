#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

#-------------------------------------------------------------------------------------------
# ThirdParty
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# echfs
#-------------------------------------------------------------------------------------------
add_custom_target(
        echfs
        COMMAND make
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/echfs)

#-------------------------------------------------------------------------------------------
# limine
#-------------------------------------------------------------------------------------------
add_library(limine INTERFACE)
target_include_directories(limine INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/limine)

#-------------------------------------------------------------------------------------------
# stivale
#-------------------------------------------------------------------------------------------
add_library(stivale INTERFACE)
target_include_directories(stivale INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/stivale)
