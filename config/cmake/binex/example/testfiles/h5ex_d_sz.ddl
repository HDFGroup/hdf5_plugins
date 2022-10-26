HDF5 "h5ex_d_sz.h5" {
GROUP "/" {
   DATASET "DS1" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 8, 8, 128 ) / ( 8, 8, 128 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 8, 8, 128 )
         SIZE XXX (198.XXXX:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32017
            COMMENT SZ compressor/decompressor for floating-point data.
            PARAMS { 3 0 128 8 8 1 1062232653 -755914244 1062232653 -755914244 0 0 0 0 }
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
