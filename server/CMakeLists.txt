set(SERVER_SOURCES
    server.c
    logic.c
    world.c
    snake.c
)

add_executable(server ${SERVER_SOURCES})
target_compile_options(server PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")

