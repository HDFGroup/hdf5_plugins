No all objects to modify layout
 <X> with chunk size 217 
 <Y> with chunk size 217 
 <Z> with chunk size 217 
 <Indexes> with chunk size 217 
 <Indexes2> with chunk size 1517 
 <Pressure> with chunk size 10 20 5 
 <Pressure2> with chunk size 10 20 5 
 <Pressure3> with chunk size 10 20 5 
 <Pressure_2D> with chunk size 10 20 
 <Stress> with chunk size 11 21 1 1 
 <Velocity> with chunk size 11 21 1 1 
 <Stress2> with chunk size 11 21 1 1 
 <Velocity2> with chunk size 11 21 1 1 
 <Stress3> with chunk size 11 21 1 1 
 <Velocity3> with chunk size 11 21 1 1 
 <VelocityZ> with chunk size 11 21 1 
 <VelocityZ2> with chunk size 11 21 1 
 <VelocityZ3> with chunk size 11 21 1 
 <XY> with chunk size 651 1 
 <XYZ> with chunk size 217 1 
 <XYZ2> with chunk size 1617 1 
 <XYZ3> with chunk size 33 1 
All objects to apply filter are...
 User Defined 32013
 <X>...Found
 <Y>...Found
 <Z>...Found
 <Indexes>...Found
 <Indexes2>...Found
 <Pressure>...Found
 <Pressure2>...Found
 <Pressure3>...Found
 <Pressure_2D>...Found
 <Stress>...Found
 <Velocity>...Found
 <Stress2>...Found
 <Velocity2>...Found
 <Stress3>...Found
 <Velocity3>...Found
 <VelocityZ>...Found
 <VelocityZ2>...Found
 <VelocityZ3>...Found
 <XY>...Found
 <XYZ>...Found
 <XYZ2>...Found
 <XYZ3>...Found
Making new file ...
-----------------------------------------
 Type     Filter (Compression)     Name
-----------------------------------------
 group                       /
 dset     UD   (0.889:1)     /DxDy
 dset     UD   (1.333:1)     /DxDyDz
 warning: could not create dataset </Indexes>. Applying original settings
 dset                        /Indexes
 warning: could not create dataset </Indexes2>. Applying original settings
 dset                        /Indexes2
 dset     UD   (12.000:1)    /Origin
 dset     UD   (8.000:1)     /Origin2
 warning: could not create dataset </Pressure>. Applying original settings
 dset                        /Pressure
 dset     UD   (7.722:1)     /Pressure2
 dset     UD   (7.722:1)     /Pressure3
 warning: could not create dataset </Pressure_2D>. Applying original settings
 dset                        /Pressure_2D
 dset     UD   (18.177:1)    /Stress
 dset     UD   (18.177:1)    /Stress2
 dset     UD   (18.177:1)    /Stress3
 dset     UD   (10.662:1)    /Velocity
 dset     UD   (10.662:1)    /Velocity2
 dset     UD   (10.662:1)    /Velocity3
 warning: could not create dataset </VelocityX_2D>. Applying original settings
 dset                        /VelocityX_2D
 warning: could not create dataset </VelocityZ>. Applying original settings
 dset                        /VelocityZ
 dset     UD   (3.638:1)     /VelocityZ2
 dset     UD   (3.638:1)     /VelocityZ3
 dset     UD   (2.052:1)     /X
 dset     UD   (2.396:1)     /XY
 dset     UD   (2.604:1)     /XYZ
 dset     UD   (2.521:1)     /XYZ2
 dset     UD   (2.340:1)     /XYZ3
 dset     UD   (2.000:1)     /X_1D
 dset     UD   (2.815:1)     /Y
 dset     UD   (1.953:1)     /Y_1D
 dset     UD   (3.232:1)     /Z
 dset     UD   (1.833:1)     /Z_1D
HDF5-DIAG: Error detected in HDF5 (1.11.2) thread 0:
  #000: /home/byrn/HDF_Projects/hdf5/dev/src/H5D.c line 145 in H5Dcreate2(): unable to create dataset
    major: Dataset
    minor: Unable to initialize object
  #001: /home/byrn/HDF_Projects/hdf5/dev/src/H5Dint.c line 474 in H5D__create_named(): unable to create and link to dataset
    major: Dataset
    minor: Unable to initialize object
  #002: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1641 in H5L_link_object(): unable to create new link to object
    major: Links
    minor: Unable to initialize object
  #003: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1885 in H5L_create_real(): can't insert link
    major: Symbol table
    minor: Unable to insert object
  #004: /home/byrn/HDF_Projects/hdf5/dev/src/H5Gtraverse.c line 867 in H5G_traverse(): internal path traversal failed
    major: Symbol table
    minor: Object not found
  #005: /home/byrn/HDF_Projects/hdf5/dev/src/H5Gtraverse.c line 639 in H5G_traverse_real(): traversal operator failed
    major: Symbol table
    minor: Callback failed
  #006: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1688 in H5L_link_cb(): unable to create object
    major: Object header
    minor: Unable to initialize object
  #007: /home/byrn/HDF_Projects/hdf5/dev/src/H5Oint.c line 2166 in H5O_obj_create(): unable to open object
    major: Object header
    minor: Can't open object
  #008: /home/byrn/HDF_Projects/hdf5/dev/src/H5Doh.c line 291 in H5O__dset_create(): unable to create dataset
    major: Dataset
    minor: Unable to initialize object
  #009: /home/byrn/HDF_Projects/hdf5/dev/src/H5Dint.c line 1136 in H5D__create(): I/O filters can't operate on this dataset
    major: Invalid arguments to routine
    minor: Unable to initialize object
  #010: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 847 in H5Z_can_apply(): unable to apply filter
    major: Data filters
    minor: Error from filter 'can apply' callback
  #011: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 807 in H5Z_prepare_prelude_callback_dcpl(): unable to apply filter
    major: Data filters
    minor: Error from filter 'can apply' callback
  #012: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 712 in H5Z_prelude_callback(): filter parameters not appropriate
    major: Data filters
    minor: Error from filter 'can apply' callback
  #013: /home/byrn/HDF_Projects/hdf5pl/dev/ZFP/src/H5Zzfp.c line 173 in H5Z_zfp_can_apply(): requires datatype class of H5T_FLOAT
    major: Data filters
    minor: Inappropriate type
HDF5-DIAG: Error detected in HDF5 (1.11.2) thread 0:
  #000: /home/byrn/HDF_Projects/hdf5/dev/src/H5D.c line 145 in H5Dcreate2(): unable to create dataset
    major: Dataset
    minor: Unable to initialize object
  #001: /home/byrn/HDF_Projects/hdf5/dev/src/H5Dint.c line 474 in H5D__create_named(): unable to create and link to dataset
    major: Dataset
    minor: Unable to initialize object
  #002: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1641 in H5L_link_object(): unable to create new link to object
    major: Links
    minor: Unable to initialize object
  #003: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1885 in H5L_create_real(): can't insert link
    major: Symbol table
    minor: Unable to insert object
  #004: /home/byrn/HDF_Projects/hdf5/dev/src/H5Gtraverse.c line 867 in H5G_traverse(): internal path traversal failed
    major: Symbol table
    minor: Object not found
  #005: /home/byrn/HDF_Projects/hdf5/dev/src/H5Gtraverse.c line 639 in H5G_traverse_real(): traversal operator failed
    major: Symbol table
    minor: Callback failed
  #006: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1688 in H5L_link_cb(): unable to create object
    major: Object header
    minor: Unable to initialize object
  #007: /home/byrn/HDF_Projects/hdf5/dev/src/H5Oint.c line 2166 in H5O_obj_create(): unable to open object
    major: Object header
    minor: Can't open object
  #008: /home/byrn/HDF_Projects/hdf5/dev/src/H5Doh.c line 291 in H5O__dset_create(): unable to create dataset
    major: Dataset
    minor: Unable to initialize object
  #009: /home/byrn/HDF_Projects/hdf5/dev/src/H5Dint.c line 1136 in H5D__create(): I/O filters can't operate on this dataset
    major: Invalid arguments to routine
    minor: Unable to initialize object
  #010: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 847 in H5Z_can_apply(): unable to apply filter
    major: Data filters
    minor: Error from filter 'can apply' callback
  #011: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 807 in H5Z_prepare_prelude_callback_dcpl(): unable to apply filter
    major: Data filters
    minor: Error from filter 'can apply' callback
  #012: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 712 in H5Z_prelude_callback(): filter parameters not appropriate
    major: Data filters
    minor: Error from filter 'can apply' callback
  #013: /home/byrn/HDF_Projects/hdf5pl/dev/ZFP/src/H5Zzfp.c line 173 in H5Z_zfp_can_apply(): requires datatype class of H5T_FLOAT
    major: Data filters
    minor: Inappropriate type
HDF5-DIAG: Error detected in HDF5 (1.11.2) thread 0:
  #000: /home/byrn/HDF_Projects/hdf5/dev/src/H5D.c line 145 in H5Dcreate2(): unable to create dataset
    major: Dataset
    minor: Unable to initialize object
  #001: /home/byrn/HDF_Projects/hdf5/dev/src/H5Dint.c line 474 in H5D__create_named(): unable to create and link to dataset
    major: Dataset
    minor: Unable to initialize object
  #002: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1641 in H5L_link_object(): unable to create new link to object
    major: Links
    minor: Unable to initialize object
  #003: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1885 in H5L_create_real(): can't insert link
    major: Symbol table
    minor: Unable to insert object
  #004: /home/byrn/HDF_Projects/hdf5/dev/src/H5Gtraverse.c line 867 in H5G_traverse(): internal path traversal failed
    major: Symbol table
    minor: Object not found
  #005: /home/byrn/HDF_Projects/hdf5/dev/src/H5Gtraverse.c line 639 in H5G_traverse_real(): traversal operator failed
    major: Symbol table
    minor: Callback failed
  #006: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1688 in H5L_link_cb(): unable to create object
    major: Object header
    minor: Unable to initialize object
  #007: /home/byrn/HDF_Projects/hdf5/dev/src/H5Oint.c line 2166 in H5O_obj_create(): unable to open object
    major: Object header
    minor: Can't open object
  #008: /home/byrn/HDF_Projects/hdf5/dev/src/H5Doh.c line 291 in H5O__dset_create(): unable to create dataset
    major: Dataset
    minor: Unable to initialize object
  #009: /home/byrn/HDF_Projects/hdf5/dev/src/H5Dint.c line 1136 in H5D__create(): I/O filters can't operate on this dataset
    major: Invalid arguments to routine
    minor: Unable to initialize object
  #010: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 847 in H5Z_can_apply(): unable to apply filter
    major: Data filters
    minor: Error from filter 'can apply' callback
  #011: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 807 in H5Z_prepare_prelude_callback_dcpl(): unable to apply filter
    major: Data filters
    minor: Error from filter 'can apply' callback
  #012: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 712 in H5Z_prelude_callback(): filter parameters not appropriate
    major: Data filters
    minor: Error from filter 'can apply' callback
  #013: /home/byrn/HDF_Projects/hdf5pl/dev/ZFP/src/H5Zzfp.c line 173 in H5Z_zfp_can_apply(): requires datatype class of H5T_FLOAT
    major: Data filters
    minor: Inappropriate type
HDF5-DIAG: Error detected in HDF5 (1.11.2) thread 0:
  #000: /home/byrn/HDF_Projects/hdf5/dev/src/H5D.c line 145 in H5Dcreate2(): unable to create dataset
    major: Dataset
    minor: Unable to initialize object
  #001: /home/byrn/HDF_Projects/hdf5/dev/src/H5Dint.c line 474 in H5D__create_named(): unable to create and link to dataset
    major: Dataset
    minor: Unable to initialize object
  #002: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1641 in H5L_link_object(): unable to create new link to object
    major: Links
    minor: Unable to initialize object
  #003: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1885 in H5L_create_real(): can't insert link
    major: Symbol table
    minor: Unable to insert object
  #004: /home/byrn/HDF_Projects/hdf5/dev/src/H5Gtraverse.c line 867 in H5G_traverse(): internal path traversal failed
    major: Symbol table
    minor: Object not found
  #005: /home/byrn/HDF_Projects/hdf5/dev/src/H5Gtraverse.c line 639 in H5G_traverse_real(): traversal operator failed
    major: Symbol table
    minor: Callback failed
  #006: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1688 in H5L_link_cb(): unable to create object
    major: Object header
    minor: Unable to initialize object
  #007: /home/byrn/HDF_Projects/hdf5/dev/src/H5Oint.c line 2166 in H5O_obj_create(): unable to open object
    major: Object header
    minor: Can't open object
  #008: /home/byrn/HDF_Projects/hdf5/dev/src/H5Doh.c line 291 in H5O__dset_create(): unable to create dataset
    major: Dataset
    minor: Unable to initialize object
  #009: /home/byrn/HDF_Projects/hdf5/dev/src/H5Dint.c line 1136 in H5D__create(): I/O filters can't operate on this dataset
    major: Invalid arguments to routine
    minor: Unable to initialize object
  #010: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 847 in H5Z_can_apply(): unable to apply filter
    major: Data filters
    minor: Error from filter 'can apply' callback
  #011: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 807 in H5Z_prepare_prelude_callback_dcpl(): unable to apply filter
    major: Data filters
    minor: Error from filter 'can apply' callback
  #012: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 712 in H5Z_prelude_callback(): filter parameters not appropriate
    major: Data filters
    minor: Error from filter 'can apply' callback
  #013: /home/byrn/HDF_Projects/hdf5pl/dev/ZFP/src/H5Zzfp.c line 173 in H5Z_zfp_can_apply(): requires datatype class of H5T_FLOAT
    major: Data filters
    minor: Inappropriate type
HDF5-DIAG: Error detected in HDF5 (1.11.2) thread 0:
  #000: /home/byrn/HDF_Projects/hdf5/dev/src/H5D.c line 145 in H5Dcreate2(): unable to create dataset
    major: Dataset
    minor: Unable to initialize object
  #001: /home/byrn/HDF_Projects/hdf5/dev/src/H5Dint.c line 474 in H5D__create_named(): unable to create and link to dataset
    major: Dataset
    minor: Unable to initialize object
  #002: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1641 in H5L_link_object(): unable to create new link to object
    major: Links
    minor: Unable to initialize object
  #003: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1885 in H5L_create_real(): can't insert link
    major: Symbol table
    minor: Unable to insert object
  #004: /home/byrn/HDF_Projects/hdf5/dev/src/H5Gtraverse.c line 867 in H5G_traverse(): internal path traversal failed
    major: Symbol table
    minor: Object not found
  #005: /home/byrn/HDF_Projects/hdf5/dev/src/H5Gtraverse.c line 639 in H5G_traverse_real(): traversal operator failed
    major: Symbol table
    minor: Callback failed
  #006: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1688 in H5L_link_cb(): unable to create object
    major: Object header
    minor: Unable to initialize object
  #007: /home/byrn/HDF_Projects/hdf5/dev/src/H5Oint.c line 2166 in H5O_obj_create(): unable to open object
    major: Object header
    minor: Can't open object
  #008: /home/byrn/HDF_Projects/hdf5/dev/src/H5Doh.c line 291 in H5O__dset_create(): unable to create dataset
    major: Dataset
    minor: Unable to initialize object
  #009: /home/byrn/HDF_Projects/hdf5/dev/src/H5Dint.c line 1136 in H5D__create(): I/O filters can't operate on this dataset
    major: Invalid arguments to routine
    minor: Unable to initialize object
  #010: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 847 in H5Z_can_apply(): unable to apply filter
    major: Data filters
    minor: Error from filter 'can apply' callback
  #011: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 807 in H5Z_prepare_prelude_callback_dcpl(): unable to apply filter
    major: Data filters
    minor: Error from filter 'can apply' callback
  #012: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 712 in H5Z_prelude_callback(): filter parameters not appropriate
    major: Data filters
    minor: Error from filter 'can apply' callback
  #013: /home/byrn/HDF_Projects/hdf5pl/dev/ZFP/src/H5Zzfp.c line 173 in H5Z_zfp_can_apply(): requires datatype class of H5T_FLOAT
    major: Data filters
    minor: Inappropriate type
HDF5-DIAG: Error detected in HDF5 (1.11.2) thread 0:
  #000: /home/byrn/HDF_Projects/hdf5/dev/src/H5D.c line 145 in H5Dcreate2(): unable to create dataset
    major: Dataset
    minor: Unable to initialize object
  #001: /home/byrn/HDF_Projects/hdf5/dev/src/H5Dint.c line 474 in H5D__create_named(): unable to create and link to dataset
    major: Dataset
    minor: Unable to initialize object
  #002: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1641 in H5L_link_object(): unable to create new link to object
    major: Links
    minor: Unable to initialize object
  #003: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1885 in H5L_create_real(): can't insert link
    major: Symbol table
    minor: Unable to insert object
  #004: /home/byrn/HDF_Projects/hdf5/dev/src/H5Gtraverse.c line 867 in H5G_traverse(): internal path traversal failed
    major: Symbol table
    minor: Object not found
  #005: /home/byrn/HDF_Projects/hdf5/dev/src/H5Gtraverse.c line 639 in H5G_traverse_real(): traversal operator failed
    major: Symbol table
    minor: Callback failed
  #006: /home/byrn/HDF_Projects/hdf5/dev/src/H5L.c line 1688 in H5L_link_cb(): unable to create object
    major: Object header
    minor: Unable to initialize object
  #007: /home/byrn/HDF_Projects/hdf5/dev/src/H5Oint.c line 2166 in H5O_obj_create(): unable to open object
    major: Object header
    minor: Can't open object
  #008: /home/byrn/HDF_Projects/hdf5/dev/src/H5Doh.c line 291 in H5O__dset_create(): unable to create dataset
    major: Dataset
    minor: Unable to initialize object
  #009: /home/byrn/HDF_Projects/hdf5/dev/src/H5Dint.c line 1136 in H5D__create(): I/O filters can't operate on this dataset
    major: Invalid arguments to routine
    minor: Unable to initialize object
  #010: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 847 in H5Z_can_apply(): unable to apply filter
    major: Data filters
    minor: Error from filter 'can apply' callback
  #011: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 807 in H5Z_prepare_prelude_callback_dcpl(): unable to apply filter
    major: Data filters
    minor: Error from filter 'can apply' callback
  #012: /home/byrn/HDF_Projects/hdf5/dev/src/H5Z.c line 712 in H5Z_prelude_callback(): filter parameters not appropriate
    major: Data filters
    minor: Error from filter 'can apply' callback
  #013: /home/byrn/HDF_Projects/hdf5pl/dev/ZFP/src/H5Zzfp.c line 173 in H5Z_zfp_can_apply(): requires datatype class of H5T_FLOAT
    major: Data filters
    minor: Inappropriate type
H5tools-DIAG: Error detected in HDF5:tools (1.11.2) thread 0:
  #000: /home/byrn/HDF_Projects/hdf5/dev/tools/src/h5repack/h5repack_copy.c line 899 in do_copy_objects(): H5Dcreate2 failed
    major: Failure in tools library
    minor: error in function
  #001: /home/byrn/HDF_Projects/hdf5/dev/tools/src/h5repack/h5repack_copy.c line 899 in do_copy_objects(): H5Dcreate2 failed
    major: Failure in tools library
    minor: error in function
  #002: /home/byrn/HDF_Projects/hdf5/dev/tools/src/h5repack/h5repack_copy.c line 899 in do_copy_objects(): H5Dcreate2 failed
    major: Failure in tools library
    minor: error in function
  #003: /home/byrn/HDF_Projects/hdf5/dev/tools/src/h5repack/h5repack_copy.c line 899 in do_copy_objects(): H5Dcreate2 failed
    major: Failure in tools library
    minor: error in function
  #004: /home/byrn/HDF_Projects/hdf5/dev/tools/src/h5repack/h5repack_copy.c line 899 in do_copy_objects(): H5Dcreate2 failed
    major: Failure in tools library
    minor: error in function
  #005: /home/byrn/HDF_Projects/hdf5/dev/tools/src/h5repack/h5repack_copy.c line 899 in do_copy_objects(): H5Dcreate2 failed
    major: Failure in tools library
    minor: error in function
