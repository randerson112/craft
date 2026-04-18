# Disable MSVC warnings from tomlc17.c
if(MSVC)
    add_compile_options(/W0)
endif()