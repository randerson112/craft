# Disable MSVC warnings from tomlc17.c
if(MSVC)
    add_compile_options(/wd4244 /wd4267 /wd4996 /wd4129)
endif()