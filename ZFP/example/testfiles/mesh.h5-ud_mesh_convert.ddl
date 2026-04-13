HDF5 "out-ud_mesh_convert.mesh.h5" {
GROUP "/" {
   DATASET "DxDy" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 2 ) / ( 2 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 2 )
         SIZE 9 (0.889:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "DxDyDz" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 3 ) / ( 3 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 3 )
         SIZE 9 (1.333:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Indexes" {
      DATATYPE  H5T_STD_I32LE
      DATASPACE  SIMPLE { ( 7161 ) / ( 7161 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 217 )
         SIZE 11113 (2.578:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Indexes2" {
      DATATYPE  H5T_STD_I32LE
      DATASPACE  SIMPLE { ( 4851 ) / ( 4851 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 1517 )
         SIZE 8549 (2.270:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Origin" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 3 ) / ( 3 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 3 )
         SIZE 1 (12.000:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Origin2" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 2 ) / ( 2 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 2 )
         SIZE 1 (8.000:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Pressure" {
      DATATYPE  H5T_STD_U32LE
      DATASPACE  SIMPLE { ( 10, 20, 30 ) / ( 10, 20, 30 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 10, 20, 5 )
         SIZE 990 (24.242:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Pressure2" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 10, 20, 20 ) / ( 10, 20, 20 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 10, 20, 5 )
         SIZE 2072 (7.722:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Pressure3" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 10, 20, 25 ) / ( 10, 20, 25 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 10, 20, 5 )
         SIZE 2590 (7.722:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Pressure_2D" {
      DATATYPE  H5T_STD_U8LE
      DATASPACE  SIMPLE { ( 20, 30 ) / ( 20, 30 ) }
      STORAGE_LAYOUT {
         CONTIGUOUS
         SIZE 600
         OFFSET 54449
      }
      FILTERS {
         NONE
      }
      FILLVALUE {
         FILL_TIME H5D_FILL_TIME_IFSET
         VALUE  H5D_FILL_VALUE_DEFAULT
      }
      ALLOCATION_TIME {
         H5D_ALLOC_TIME_LATE
      }
   }
   DATASET "Stress" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 11, 21, 31, 6 ) / ( 11, 21, 31, 6 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 11, 21, 1, 1 )
         SIZE 9455 (18.177:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Stress2" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 11, 21, 21, 6 ) / ( 11, 21, 21, 6 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 11, 21, 1, 1 )
         SIZE 6405 (18.177:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Stress3" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 11, 21, 26, 6 ) / ( 11, 21, 26, 6 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 11, 21, 1, 1 )
         SIZE 7930 (18.177:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Velocity" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 11, 21, 31, 3 ) / ( 11, 21, 31, 3 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 11, 21, 1, 1 )
         SIZE 8060 (10.662:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Velocity2" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 11, 21, 21, 3 ) / ( 11, 21, 21, 3 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 11, 21, 1, 1 )
         SIZE 5460 (10.662:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Velocity3" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 11, 21, 26, 3 ) / ( 11, 21, 26, 3 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 11, 21, 1, 1 )
         SIZE 6760 (10.662:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "VelocityX_2D" {
      DATATYPE  H5T_STD_I8LE
      DATASPACE  SIMPLE { ( 21, 31 ) / ( 21, 31 ) }
      STORAGE_LAYOUT {
         CONTIGUOUS
         SIZE 651
         OFFSET 171792
      }
      FILTERS {
         NONE
      }
      FILLVALUE {
         FILL_TIME H5D_FILL_TIME_IFSET
         VALUE  H5D_FILL_VALUE_DEFAULT
      }
      ALLOCATION_TIME {
         H5D_ALLOC_TIME_LATE
      }
   }
   DATASET "VelocityZ" {
      DATATYPE  H5T_STD_I32LE
      DATASPACE  SIMPLE { ( 11, 21, 31 ) / ( 11, 21, 31 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 11, 21, 1 )
         SIZE 2883 (9.935:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "VelocityZ2" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 11, 21, 21 ) / ( 11, 21, 21 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 11, 21, 1 )
         SIZE 5334 (3.638:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "VelocityZ3" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 11, 21, 26 ) / ( 11, 21, 26 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 11, 21, 1 )
         SIZE 6604 (3.638:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "X" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 7161 ) / ( 7161 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 217 )
         SIZE 13959 (2.052:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "XY" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 651, 2 ) / ( 651, 2 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 651, 1 )
         SIZE 2174 (2.396:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "XYZ" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 7161, 3 ) / ( 7161, 3 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 217, 1 )
         SIZE 32996 (2.604:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "XYZ2" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 4851, 3 ) / ( 4851, 3 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 1617, 1 )
         SIZE 23091 (2.521:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "XYZ3" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 6006, 3 ) / ( 6006, 3 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 33, 1 )
         SIZE 30805 (2.340:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "X_1D" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 31 ) / ( 31 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 31 )
         SIZE 62 (2.000:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Y" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 7161 ) / ( 7161 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 217 )
         SIZE 10175 (2.815:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Y_1D" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 21 ) / ( 21 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 21 )
         SIZE 43 (1.953:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Z" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 7161 ) / ( 7161 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 217 )
         SIZE 8862 (3.232:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
   DATASET "Z_1D" {
      DATATYPE  H5T_IEEE_F32LE
      DATASPACE  SIMPLE { ( 11 ) / ( 11 ) }
      STORAGE_LAYOUT {
         CHUNKED ( 11 )
         SIZE 24 (1.833:1 COMPRESSION)
      }
      FILTERS {
         USER_DEFINED_FILTER {
            FILTER_ID 32013
            COMMENT H5Z-ZFP-1.1.1 (ZFP-1.0.1)
            PARAMS { XXXX }
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
