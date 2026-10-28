HDF5 "out-ud_convert.h5repack_floats.h5" {
GROUP "/" {
   DATASET "data" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 32, 64 ) / ( 32, 64 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 4, 8 )
         SIZE 8192 (1.000:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32032
            COMMENT BitRound filter (doi:10.1038/s43588-021-00156-2) https://github.com/HDFGroup/hdf5_plugins/tree/master/BITROUND
            PARAMS { 7 4 0 0 0 }
         }
      }
      FILLVALUE {
         FILL_TIME H5D_FILL_TIME_IFSET
         VALUE  H5D_FILL_VALUE_DEFAULT
      }
      ALLOCATION_TIME {
         H5D_ALLOC_TIME_INCR
      }
   }
}
}
