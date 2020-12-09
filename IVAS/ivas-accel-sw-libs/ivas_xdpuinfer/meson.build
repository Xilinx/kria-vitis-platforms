sources = [
  'src/ivas_xdpuinfer.cpp'
]

# ADD YOLOV3
if get_option('YOLOV3') != '0'
  add_project_arguments('-DENABLE_YOLOV3', language : 'c')
  add_project_arguments('-DENABLE_YOLOV3', language : 'cpp')
  yolov3_dep = cc.find_library('vitis_ai_library-yolov3', dirs : ['/usr/lib/'])
  sources += [
    'src/ivas_xyolov3.cpp',
  ]
else
  yolov3_dep = []
endif

# ADD CLASSIFICATION
if get_option('CLASSIFICATION') != '0'
  add_project_arguments('-DENABLE_CLASSIFICATION', language : 'c')
  add_project_arguments('-DENABLE_CLASSIFICATION', language : 'cpp')
  classi_dep = cc.find_library('vitis_ai_library-classification', dirs : ['/usr/lib/'])
  sources += [
    'src/ivas_xclassification.cpp',
  ]
else
  classi_dep = []
endif

# ADD FACEDETECT
if get_option('FACEDETECT') != '0'
  add_project_arguments('-DENABLE_FACEDETECT', language : 'c')
  add_project_arguments('-DENABLE_FACEDETECT', language : 'cpp')
  facedetect_dep = cc.find_library('vitis_ai_library-facedetect', dirs : ['/usr/lib/'])
  sources += [
    'src/ivas_xfacedetect.cpp',
  ]
else
  facedetect_dep = []
endif

#ADD SSD
if get_option('SSD') != '0'
  add_project_arguments('-DENABLE_SSD', language : 'c')
  add_project_arguments('-DENABLE_SSD', language : 'cpp')
  ssd_dep = cc.find_library('vitis_ai_library-ssd', dirs : ['/usr/lib/'])
  sources += [
    'src/ivas_xssd.cpp',
  ]
else
  ssd_dep = []
endif

#ADD REFINEDET
if get_option('REFINEDET') != '0'
  add_project_arguments('-DENABLE_REFINEDET', language : 'c')
  add_project_arguments('-DENABLE_REFINEDET', language : 'cpp')
  refinedet_dep = cc.find_library('vitis_ai_library-refinedet', dirs : ['/usr/lib/'])
  sources += [
    'src/ivas_xrefinedet.cpp',
  ]
else
  refinedet_dep = []
endif

#ADD TFSSD
if get_option('TFSSD') != '0'
  add_project_arguments('-DENABLE_TFSSD', language : 'c')
  add_project_arguments('-DENABLE_TFSSD', language : 'cpp')
  tfssd_dep = cc.find_library('vitis_ai_library-tfssd', dirs : ['/usr/lib/'])
  sources += [
    'src/ivas_xtfssd.cpp',
  ]
else
  tfssd_dep = []
endif

#ADD YOLOV2
if get_option('YOLOV2') != '0'
  add_project_arguments('-DENABLE_YOLOV2', language : 'c')
  add_project_arguments('-DENABLE_YOLOV2', language : 'cpp')
  yolov2_dep = cc.find_library('vitis_ai_library-yolov2', dirs : ['/usr/lib/'])
  sources += [
    'src/ivas_xyolov2.cpp',
  ]
else
  yolov2_dep = []
endif

vartutil_dep = cc.find_library('vart-util', dirs : ['/usr/lib/'])
xnnpp_dep = cc.find_library('xnnpp-xnnpp', dirs : ['/usr/lib/'])
vitisconfig_dep = cc.find_library('vitis_ai_library-model_config', dirs : ['/usr/lib/'])
dputask_dep = cc.find_library('vitis_ai_library-dpu_task', dirs : ['/usr/lib/'])

#vitisinc_dir = include_directories('/proj/ipeng3/saurabhs/nobkup/2020_1_sysroot/sysroots/aarch64-xilinx-linux/usr/include/vitis')

ivas_xdpuinfer = library('ivas_xdpuinfer',
  sources,
  cpp_args : [gst_plugins_ivas_args, '-std=c++11'],
  include_directories : [configinc],
  dependencies : [gstvideo_dep, gst_dep, xrt_dep, jansson_dep, ivasutils_dep, gstivasinfermeta_dep, ivasinputmeta_dep, opencv_dep, vartutil_dep, xnnpp_dep, vitisconfig_dep, dputask_dep, classi_dep, yolov3_dep, facedetect_dep, ssd_dep, tfssd_dep, yolov2_dep, refinedet_dep],
  install : true,
)

ivas_xdpuinfer = library('ivas_xdputest',
  'src/ivas_xdputest.cpp',
  cpp_args : [gst_plugins_ivas_args, '-std=c++11'],
  include_directories : [configinc],
  dependencies : [gstvideo_dep, gst_dep, xrt_dep, jansson_dep, ivasutils_dep, ivasinputmeta_dep, opencv_dep],
  install : true,
)
