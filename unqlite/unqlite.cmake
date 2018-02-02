# Unqlite library
add_library(unqlite SHARED unqlite/unqlite.c)

# make sure the functions are hidden by default
set_target_properties(unqlite PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN 1)