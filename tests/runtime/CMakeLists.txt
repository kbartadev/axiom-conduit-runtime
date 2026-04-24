# tests/runtime/CMakeLists.txt
file(GLOB RUNTIME_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# 1. Individual Runtime Targets
foreach(RT_FILE ${RUNTIME_SOURCES})
    get_filename_component(RT_NAME ${RT_FILE} NAME_WE)
    add_executable(${RT_NAME} ${RT_FILE})
    target_link_libraries(${RT_NAME} PRIVATE conduit gtest gtest_main)
    target_compile_features(${RT_NAME} PRIVATE cxx_std_20)
    gtest_discover_tests(${RT_NAME} DISCOVERY_TIMEOUT 30)
    set_property(TARGET ${RT_NAME} PROPERTY FOLDER "Tests/Runtime/Individual")
endforeach()

# 2. Unified Runtime Target
set(RUNTIME_UNIFIED conduit_runtime_all)
add_executable(${RUNTIME_UNIFIED} ${RUNTIME_SOURCES})
target_link_libraries(${RUNTIME_UNIFIED} PRIVATE conduit gtest gtest_main)
target_compile_features(${RUNTIME_UNIFIED} PRIVATE cxx_std_20)
gtest_discover_tests(${RUNTIME_UNIFIED} DISCOVERY_TIMEOUT 60)
set_property(TARGET ${RUNTIME_UNIFIED} PROPERTY FOLDER "Tests/Runtime")