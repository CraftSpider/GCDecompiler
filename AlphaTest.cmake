

function(copy_test_resources TARGET_NAME)
    file(TO_NATIVE_PATH ${CMAKE_BINARY_DIR} ALPHA_BINARY_DIR)
    file(TO_NATIVE_PATH ${CMAKE_SOURCE_DIR} ALPHA_SOURCE_DIR)

    if(WIN32)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND del /f /s /q ${ALPHA_BINARY_DIR}\\resource 1>nul
                COMMAND if exist \"${ALPHA_BINARY_DIR}\\resources\\\" rmdir /s /q ${ALPHA_BINARY_DIR}\\resources 2>nul
                COMMAND mkdir ${ALPHA_BINARY_DIR}\\resources
                COMMAND copy ${ALPHA_SOURCE_DIR}\\tests\\resources\\ ${ALPHA_BINARY_DIR}\\resources\\)
    else()
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND rm -rf ${ALPHA_BINARY_DIR}/resources
                COMMAND mkdir -p ${ALPHA_BINARY_DIR}/resources
                COMMAND cp -r ${ALPHA_SOURCE_DIR}/tests/resources/ ${ALPHA_BINARY_DIR}/ || :)
    endif()

    unset(ALPHA_BINARY_DIR)
    unset(ALPHA_SOURCE_DIR)
endfunction()
