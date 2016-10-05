# TEST: sub module

add_test_exe    (test_bst test_bst.cpp)
test_link_libs  (test_bst )
create_test     (test_bst)

add_test_exe    (test_hash_map test_hash_map.cpp)
test_link_libs  (test_hash_map )
create_test     (test_hash_map)
