# Defining a resource dir for POEM
set(POEM_RESOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/resources)
message(STATUS "Resources stored at ${POEM_RESOURCE_DIR}")
file(MAKE_DIRECTORY ${POEM_RESOURCE_DIR})


set(RESOURCE_FILES
        poem_v0_example.nc
        poem_v1_example.nc
        poem_v1_PerfPolarSet.nc
        poem_v1_example_5_polars.nc
)

foreach (FILE ${RESOURCE_FILES})
    if (NOT EXISTS ${POEM_RESOURCE_DIR}/${FILE})
        message(STATUS "Downloading ${FILE} to ${POEM_RESOURCE_DIR}")
        execute_process(
                COMMAND
                aws s3 cp s3://dice-test-data/poem/${FILE} ${POEM_RESOURCE_DIR}/${FILE}
        )
    endif ()
    if (NOT EXISTS ${POEM_RESOURCE_DIR}/${FILE})
        message(STATUS "The Download of ${FILE} did not work properly")
    endif ()
endforeach ()
