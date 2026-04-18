cmake_minimum_required(VERSION 3.25)

###########################################################################################
# Add unit test sources
###########################################################################################
target_sources(
    threading_library_tests
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/AsyncResultTests.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ConcurrentBlockQueueTests.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ConcurrentHashMapTests.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ConcurrentStackTests.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/SynchronizedQueueTests.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/ThreadPoolTests.cpp"
)
