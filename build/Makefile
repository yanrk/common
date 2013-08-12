project_home       = ..
build_dir          = $(project_home)/build
bin_dir            = $(project_home)/lib/linux
object_dir         = $(project_home)/.objs
common_home        = $(project_home)



# includes of common headers
common_inc_path    = $(common_home)/inc
common_includes    = -I$(common_inc_path)/base
common_includes   += -I$(common_inc_path)/net



# all includes that common solution needs
includes           = $(common_includes)



# source files of common solution

# source files of base project
base_src_path      = $(common_home)/src/base
base_source        = $(filter %.cpp, $(shell find $(base_src_path) -depth -name "*.cpp"))

# source files of net project
net_src_path       = $(common_home)/src/net
net_source         = $(filter %.cpp, $(shell find $(net_src_path) -depth -name "*.cpp"))



# objects of common solution
base_objects       = $(base_source:$(project_home)%.cpp=$(object_dir)%.o)
net_objects        = $(net_source:$(project_home)%.cpp=$(object_dir)%.o)



# system librarys
system_libs        = -lpthread

# base depends librarys
base_depend_libs   = $(system_libs)

# net depends librarys
net_depend_libs    = $(system_libs)
net_depend_libs   += -L$(bin_dir) -lyanrk_base



# output librarys
base_output_lib    = $(bin_dir)/libyanrk_base.so
net_output_lib     = $(bin_dir)/libyanrk_net.so



# my g++ not support nullptr and 64bits
# so no -m64, and add a macro -Dnullptr=0

# build flags for objects
build_obj_flags    = -std=c++0x -g -Wall -O1 -pipe -fPIC -Dnullptr=0

# build flags for execution
build_exec_flags   = $(build_obj_flags)

# build flags for dynamic link librarys
build_so_flags     = -std=c++0x -shared



# build targets
targets            = base net

# let 'build' be default target, build all targets
build    : $(targets)

base     : $(base_objects)
	@echo
	@echo "@@@@@  start making base  @@@@@"
	g++ $(build_so_flags) -o $(base_output_lib) $^ $(base_depend_libs)
	@echo "@@@@@  make base success  @@@@@"
	@echo
	
net      : $(net_objects)
	@echo
	@echo "@@@@@  start making net  @@@@@"
	g++ $(build_so_flags) -o $(net_output_lib) $^ $(net_depend_libs)
	@echo "@@@@@  make net success  @@@@@"
	@echo

# build all objects
$(object_dir)/%.o:$(project_home)/%.cpp
	@dir=`dirname $@`;      \
    if [ ! -d $$dir ]; then \
        mkdir -p $$dir;     \
    fi
	g++ -c $(build_obj_flags) $(includes) -o $@ $<

clean    :
	rm -rf $(object_dir) $(bin_dir)/*

rebuild  : clean build
