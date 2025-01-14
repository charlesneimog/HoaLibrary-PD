cmake_minimum_required(VERSION 3.15)

# set(CMAKE_SUPPRESS_REGENERATION true)
# set(CMAKE_MACOSX_RPATH Off)
# set(CMAKE_OSX_DEPLOYMENT_TARGET 10.4)
# set(CMAKE_OSX_ARCHITECTURES "i386;x86_64")

include(Libraries/pd.build/pd.cmake)
project(hoa)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set_pd_external_path("${PROJECT_SOURCE_DIR}/hoa/")

file(GLOB hoa_library_src ${PROJECT_SOURCE_DIR}/hoa.library/Sources/*.cpp ${PROJECT_SOURCE_DIR}/hoa.library/Sources/*.hpp)
source_group(hoa.library FILES ${hoa_library_src})
include_directories(${PROJECT_SOURCE_DIR}/hoa.library/Sources/)

set(hoa_pd_path "${PROJECT_SOURCE_DIR}/sources")
set(hoa_pd_sources ${hoa_pd_path}/hoa.pd.h ${hoa_pd_path}/hoa.pd.c)

# Common Part
add_pd_external(hoa.pi hoa.pi ${hoa_pd_path}/common/hoa.pi.c)
add_pd_external(hoa.pi_tilde hoa.pi~ ${hoa_pd_path}/common/hoa.pi_tilde.c)
add_pd_external(hoa.dac_tilde hoa.dac~ ${hoa_pd_path}/common/hoa.dac_tilde.c)
add_pd_external(hoa.connect hoa.connect ${hoa_pd_path}/common/hoa.connect.c)

add_pd_external(hoa.in hoa.in ${hoa_pd_path}/common/hoa.in.c)
add_pd_external(hoa.out hoa.out ${hoa_pd_path}/common/hoa.out.c)
add_pd_external(hoa.in_tilde hoa.in~ ${hoa_pd_path}/common/hoa.in_tilde.c)
add_pd_external(hoa.out_tilde hoa.out~ ${hoa_pd_path}/common/hoa.out_tilde.c)
#add_pd_external(hoa.thisprocess_tilde hoa.thisprocess~ ${hoa_pd_path}/common/hoa.thisprocess_tilde.c)

# 2D Part
file(GLOB hoa_pd_2d_encoder_tilde_sources ${hoa_pd_sources} ${hoa_pd_path}/2d/hoa.2d.encoder_tilde.cpp)
add_pd_external(hoa.2d.encoder_tilde hoa.2d.encoder~ "${hoa_pd_2d_encoder_tilde_sources}")
file(GLOB hoa_pd_2d_optim_tilde_sources ${hoa_pd_sources} ${hoa_pd_path}/2d/hoa.2d.optim_tilde.cpp)
add_pd_external(hoa.2d.optim_tilde hoa.2d.optim~ "${hoa_pd_2d_optim_tilde_sources}")
file(GLOB hoa_pd_2d_wider_tilde_sources ${hoa_pd_sources} ${hoa_pd_path}/2d/hoa.2d.wider_tilde.cpp)
add_pd_external(hoa.2d.wider_tilde hoa.2d.wider~ "${hoa_pd_2d_wider_tilde_sources}")
file(GLOB hoa_pd_2d_decoder_tilde_sources ${hoa_pd_sources} ${hoa_pd_path}/2d/hoa.2d.decoder_tilde.cpp)
add_pd_external(hoa.2d.decoder_tilde hoa.2d.decoder~ "${hoa_pd_2d_decoder_tilde_sources}")
file(GLOB hoa_pd_2d_process_tilde_sources ${hoa_pd_sources} ${hoa_pd_path}/common/hoa.process_instance.c ${hoa_pd_path}/2d/hoa.2d.process_tilde.c)
add_pd_external(hoa.2d.process_tilde hoa.2d.process~ "${hoa_pd_2d_process_tilde_sources}")
#file(GLOB hoa_pd_2d_map_tilde_sources ${hoa_pd_sources} ${hoa_pd_path}/2d/hoa.2d.map_tilde.cpp)
#add_pd_external(hoa.2d.map_tilde hoa.2d.map~ "${hoa_pd_2d_map_tilde_sources}")
file(GLOB hoa_pd_2d_rotate_tilde_sources ${hoa_pd_sources} ${hoa_pd_path}/2d/hoa.2d.rotate_tilde.cpp)
add_pd_external(hoa.2d.rotate_tilde hoa.2d.rotate~ "${hoa_pd_2d_rotate_tilde_sources}")
file(GLOB hoa_pd_2d_projector_tilde_sources ${hoa_pd_sources} ${hoa_pd_path}/2d/hoa.2d.projector_tilde.cpp)
add_pd_external(hoa.2d.projector_tilde hoa.2d.projector~ "${hoa_pd_2d_projector_tilde_sources}")
# file(GLOB hoa_pd_2d_recomposer_tilde_sources ${hoa_pd_sources} ${hoa_pd_path}/2d/hoa.2d.recomposer_tilde.cpp)
# add_pd_external(hoa.2d.recomposer_tilde hoa.2d.recomposer~ "${hoa_pd_2d_recomposer_tilde_sources}")

# 3D Part
file(GLOB hoa_pd_3d_encoder_tilde_sources ${hoa_pd_sources} ${hoa_pd_path}/3d/hoa.3d.encoder_tilde.cpp)
add_pd_external(hoa.3d.encoder_tilde hoa.3d.encoder~ "${hoa_pd_3d_encoder_tilde_sources}")
file(GLOB hoa_pd_3d_optim_tilde_sources ${hoa_pd_sources} ${hoa_pd_path}/3d/hoa.3d.optim_tilde.cpp)
add_pd_external(hoa.3d.optim_tilde hoa.3d.optim~ "${hoa_pd_3d_optim_tilde_sources}")
file(GLOB hoa_pd_3d_wider_tilde_sources ${hoa_pd_sources} ${hoa_pd_path}/3d/hoa.3d.wider_tilde.cpp)
add_pd_external(hoa.3d.wider_tilde hoa.3d.wider~ "${hoa_pd_3d_wider_tilde_sources}")
file(GLOB hoa_pd_3d_decoder_tilde_sources ${hoa_pd_sources} ${hoa_pd_path}/3d/hoa.3d.decoder_tilde.cpp)
add_pd_external(hoa.3d.decoder_tilde hoa.3d.decoder~ "${hoa_pd_3d_decoder_tilde_sources}")
file(GLOB hoa_pd_3d_process_tilde_sources ${hoa_pd_sources} ${hoa_pd_path}/common/hoa.process_instance.c ${hoa_pd_path}/3d/hoa.3d.process_tilde.c)
add_pd_external(hoa.3d.process_tilde hoa.3d.process~ "${hoa_pd_3d_process_tilde_sources}")
#file(GLOB hoa_pd_3d_map_tilde_sources ${hoa_pd_sources} ${hoa_pd_path}/3d/hoa.3d.map_tilde.cpp)
#add_pd_external(hoa.3d.map_tilde hoa.3d.map~ "${hoa_pd_3d_map_tilde_sources}")

message(STATUS ${CMAKE_C_FLAGS})
message(STATUS ${CMAKE_CXX_FLAGS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_C_FLAGS}")
