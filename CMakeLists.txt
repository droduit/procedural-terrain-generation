cmake_minimum_required(VERSION 2.8)
project(icg17)

# load the common ICG configuration
include(common/icg_settings.cmake)

# week 1
#add_subdirectory(lab1_helloworld)

# week 2
#add_subdirectory(lab2_intro_to_glsl)
#add_subdirectory(hw2p1_spiral)
#add_subdirectory(hw2p2_checkerboard)
#add_subdirectory(hw2p3_planets)

# week 3
#add_subdirectory(lab3_opengl3d)
#add_subdirectory(hw3_trackball)

# week 4
#add_subdirectory(hw4_shading)

# week 5
#add_subdirectory(lab5_framebuffer)
# TODO : copy the lab folder and call it hw4p1_fastsmooth, then uncomment line below
#add_subdirectory(hw4p1_fastsmooth)
add_subdirectory(hw4p2_mirrorfloor)
add_subdirectory(hw4p3_motionblur)

