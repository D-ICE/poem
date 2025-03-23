# Defining a resource dir for POEM
set(POEM_RESOURCE_DIR ${PROJECT_BINARY_DIR}/bin/tests)
file(MAKE_DIRECTORY ${POEM_RESOURCE_DIR})

set(RESOURCE_EXAMPLE_TARGETS 
    poem_v0_example
    poem_v0_example_no_sails
    poem_v0_example_with_sails

    poem_v1_example
    poem_v1_PerfPolarSet
    poem_v1_example_5_polars
)

message(STATUS "******* DOWNLOADING TEST FILES FOR POEM *******")
message(STATUS "TEST FILES will be downloaded in ${POEM_RESOURCE_DIR}")
foreach (TARGET ${RESOURCE_EXAMPLE_TARGETS})
    if (NOT EXISTS ${POEM_RESOURCE_DIR}/${TARGET}.nc)
        message(STATUS "Creating target for ${POEM_RESOURCE_DIR}/${TARGET}.nc")
        add_custom_target(
            ${TARGET}
            aws s3 cp s3://dice-test-data/poem/${TARGET}.nc ${POEM_RESOURCE_DIR}
            BYPRODUCTS ${POEM_RESOURCE_DIR}/${TARGET}.nc
            COMMENT "Downloading ${TARGET}.nc to ${POEM_RESOURCE_DIR}"
        )
        add_dependencies(poem_tests ${TARGET})
    endif ()
endforeach ()

#set(RESOURCE_FILES
#        poem_v0_example.nc
#        poem_v0_example_no_sails.nc
#        poem_v0_example_with_sails.nc

#        poem_v1_example.nc
#        poem_v1_PerfPolarSet.nc
#        poem_v1_example_5_polars.nc
#)

#message(STATUS "******* DOWNLOADING TEST FILES FOR POEM *******")
#message(STATUS "TEST FILES will be downloaded in ${POEM_RESOURCE_DIR}")
#foreach (FILE ${RESOURCE_FILES})
#    if (NOT EXISTS ${POEM_RESOURCE_DIR}/${FILE})
#        message(STATUS "Downloading ${FILE} to ${POEM_RESOURCE_DIR}")
#        execute_process(
#                COMMAND
#                aws s3 cp s3://dice-test-data/poem/${FILE} ${POEM_RESOURCE_DIR}/${FILE}
#        )
#    else ()
#        message(STATUS "Testing file ${FILE} found in ${POEM_RESOURCE_DIR}")
#    endif ()
#    if (NOT EXISTS ${POEM_RESOURCE_DIR}/${FILE})
#        message(STATUS "The Download of ${FILE} did not work properly")
#    endif ()
#endforeach ()
