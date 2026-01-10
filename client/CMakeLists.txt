set(CLIENT_SOURCES
    client.c
    input.c
    persistence.c
    render.c
)

add_executable(client ${CLIENT_SOURCES})
target_compile_options(client PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")
