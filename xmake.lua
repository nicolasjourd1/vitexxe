set_project("vitexxe")
set_version("0.1.0")

set_languages("cxx23")
set_warnings("all", "extra")

if is_plat("windows") then
    add_cxflags("/utf-8")
end

set_policy("build.warning", true)

add_requires("raylib 6.0")
add_requires("catch2 3.15")

add_includedirs("src")

-- Libraries

target("vxcore")
    set_kind("static")
    add_files("src/vxcore/**.cpp")

target("vxmodel")
    set_kind("static")
    add_deps("vxcore")
    add_files("src/vxmodel/**.cpp")

target("vxlayout")
    set_kind("static")
    add_deps("vxcore", "vxmodel")
    add_files("src/vxlayout/**.cpp")

-- TBD


-- target("vxrender")
--     set_kind("static")
--     add_files("src/vxrender/**.cpp")

-- Binaries

target("vitexxe")
    set_kind("binary")
    add_deps("vxcore", "vxmodel", "vxlayout")
    add_packages("raylib")
    add_files("src/vitexxe/**.cpp")

target("vitexxe-tests")
    set_kind("binary")
    add_deps("vxcore", "vxmodel", "vxlayout") -- should be the same as vitexxe deps above
    add_packages("catch2")
    add_files("tests/**.cpp")
