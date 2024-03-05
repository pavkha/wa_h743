add_library(freertos_addons INTERFACE)

set(freertos_addons_dir ${CMAKE_SOURCE_DIR}/../freertos-addons)

target_sources(freertos_addons INTERFACE
	${freertos_addons_dir}/c++/Source/ccondition_variable.cpp
	${freertos_addons_dir}/c++/Source/cevent_groups.cpp
	${freertos_addons_dir}/c++/Source/cmem_pool.cpp
	${freertos_addons_dir}/c++/Source/cmutex.cpp
	${freertos_addons_dir}/c++/Source/cqueue.cpp
	${freertos_addons_dir}/c++/Source/cread_write_lock.cpp
	${freertos_addons_dir}/c++/Source/csemaphore.cpp
	${freertos_addons_dir}/c++/Source/ctasklet.cpp
	${freertos_addons_dir}/c++/Source/cthread.cpp
	${freertos_addons_dir}/c++/Source/ctickhook.cpp
	${freertos_addons_dir}/c++/Source/ctimer.cpp
	${freertos_addons_dir}/c++/Source/cworkqueue.cpp
)

target_include_directories(freertos_addons INTERFACE
	${freertos_addons_dir}/c++/Source/include
)

target_compile_definitions(freertos_addons INTERFACE
	CPP_FREERTOS_NO_CPP_STRINGS
	CPP_FREERTOS_NO_EXCEPTIONS
)
