
ivas_xpp_pipeline = library('ivas_xpp_pipeline',
  'src/ivas_xpp_pipeline.c',
  c_args : [gst_plugins_ivas_args],
  include_directories : [configinc],
  dependencies : [jansson_dep, ivasutils_dep],
  install : true,
  install_dir : ivas_videolib_install_path,
)

pkgconfig.generate(ivas_xpp_pipeline, install_dir : plugins_pkgconfig_install_dir)
