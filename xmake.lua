set_project("vitexxe")
set_version("0.1.0")

add_rules("mode.debug", "mode.release")
set_languages("cxx23")
set_warnings("allextra")

add_requires("raylib 6.0")
add_requires("raygui 4.0")
add_requires("catch2 3.15", {configs = {main = true}})

add_includedirs("src")

target("vitexxe")
    set_kind("binary")
    add_files("src/vitexxe/*.cpp")
    add_headerfiles("src/vitexxe/*.hpp")
    add_packages("raylib", "raygui")
    set_rundir("$(projectdir)")

target("vitexxe-tests")
    set_kind("binary")
    add_files("tests/*.cpp")
    add_packages("catch2", "raylib", "raygui")
