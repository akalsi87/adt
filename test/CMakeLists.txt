# TEST: sub module

add_test_exe    (test_bst test_bst.cpp)
test_link_libs  (test_bst )
create_test     (test_bst)

add_test_exe    (test_hash_map test_hash_map.cpp)
test_link_libs  (test_hash_map )
create_test     (test_hash_map)

add_test_exe    (towers_of_hanoi towers_of_hanoi.cpp)
test_link_libs  (towers_of_hanoi )
create_test     (towers_of_hanoi)

add_test_exe    (test_lru_cache test_lru_cache.cpp)
test_link_libs  (test_lru_cache )
create_test     (test_lru_cache)

add_test_exe    (test_heap test_heap.cpp)
test_link_libs  (test_heap )
create_test     (test_heap)
