file(GLOB HEADERS "include/eosio/dice_plugin/*.hpp")
include_directories(${CMAKE_CURRENT_SOURCE_DIR} include db)

add_library(dice_plugin
    dice_plugin.cpp
    ${HEADERS}
    )

target_link_libraries(dice_plugin
    chain_plugin
    eosio_chain
    appbase
    )
target_include_directories( dice_plugin
                            PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include" )

