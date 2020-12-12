
ivas_xboundingbox = library('ivas_xboundingbox',
  'src/ivas_xboundingbox.cpp',
  cpp_args : [gst_plugins_ivas_args, '-std=c++11'],
  include_directories : [configinc],
  dependencies : [gst_dep, jansson_dep, ivasutils_dep, gstivasinfermeta_dep, opencv_dep],
  install : true,
)
