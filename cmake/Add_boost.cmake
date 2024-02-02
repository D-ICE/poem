if (NOT BUILD_SHARED_LIBS)
    set(Boost_USE_STATIC_LIBS ON CACHE BOOL "Use static library for Boost")
endif ()

message(STATUS "******* FINDING BOOST dependency on the system from ${PROJECT_NAME} (requested minimal version: ${boost_FIND_TAG}) *******")
find_package(Boost ${boost_FIND_TAG})

if (Boost_FOUND)
    message(STATUS "Boost found with version ${Boost_VERSION_STRING}")



##    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
##    MESSAGE(STATUS "Boost found with version ${Boost_VERSION_STRING}")
#ELSE (Boost_FOUND)
##    MESSAGE(FATAL_ERROR "Cannot build Boost project without Boost. Please set Boost_DIR.")
#ENDIF (Boost_FOUND)

else()
    message(STATUS "Boost ${boost_TAG} is going to be built from scratch !")

    include(FetchContent)

    # TODO: construire l'URL a partir de ${boost_TAG}
    FetchContent_Declare(boost URL ${boost_URL})

    FetchContent_GetProperties(boost)
    if (NOT boost_POPULATED)
        message(STATUS "******* FETCHING boost dependency from ${PROJECT_NAME} (requested version: ${boost_TAG}) *******")
        FetchContent_Populate(boost)

        set(boost_LIBS_TO_BUILD "filesystem,math") # TODO: placer dans le URL.conf

        # generate b2
        if (NOT EXISTS ${boost_SOURCE_DIR}/b2)
            message(STATUS "Bootstrapping boost to generate b2")
            execute_process(
                    COMMAND ./bootstrap.sh --with-libraries=${boost_LIBS_TO_BUILD}
                    WORKING_DIRECTORY "${boost_SOURCE_DIR}"
                    )
        endif ()

        add_custom_target(boost
                COMMAND ./b2 headers
                --build-dir=${boost_BINARY_DIR}
                -j6

                WORKING_DIRECTORY "${boost_SOURCE_DIR}"
                )

        set(Boost_INCLUDE_DIRS ${boost_SOURCE_DIR})

        add_library(Boost::headers INTERFACE IMPORTED)
        target_include_directories(Boost::headers INTERFACE ${Boost_INCLUDE_DIRS})

        add_library(Boost::boost INTERFACE IMPORTED)
        target_link_libraries(Boost::boost INTERFACE Boost::headers)

#
#    else ()
#        message(STATUS "BOOST ALREADY POPULATED")
    endif ()


#else ()
#    message(STATUS "******* FINDING BOOST dependency on the system from ${PROJECT_NAME} (requested minimal version: ${boost_TAG}) *******")
#
#    FIND_PACKAGE(Boost ${boost_TAG} REQUIRED)
#    IF (Boost_FOUND)
#        INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
#        MESSAGE(STATUS "Boost found with version ${Boost_VERSION_STRING}")
#    ELSE (Boost_FOUND)
#        MESSAGE(FATAL_ERROR "Cannot build Boost project without Boost. Please set Boost_DIR.")
#    ENDIF (Boost_FOUND)

endif ()