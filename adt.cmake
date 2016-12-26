# header only library : adt
# -- Headers
# export
set(adt_exp_hdr
    include/adt/bst.hpp;
    include/adt/dlist.hpp;
    include/adt/heap.hpp;
    include/adt/hash_map.hpp;
    include/adt/trie.hpp;
   )

# -- Install!
install_hdr(${adt_exp_hdr})
