add_requires("raylib")

target("raylib-game")
do

 set_kind("binary")
    set_languages("cxx17")

    -- system libs needed on Windows
    add_links("user32", "ole32", "comdlg32", "shell32")

    -- link raylib

    add_packages("raylib")

    add_files("cpp/*.cc", "cpp/*.c", "cpp/*.cpp")

end