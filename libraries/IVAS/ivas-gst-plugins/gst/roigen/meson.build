gstivas_xroigen = library('gstivas_xroigen', 'gstivas_xroigen.c',
  c_args : gst_plugins_ivas_args,
  include_directories : [configinc, libsinc],
  dependencies : [gstvideo_dep, gst_dep, gstivasmeta_dep, gstivasinfermeta_dep],
  install : true,
  install_dir : plugins_install_dir,
)

pkgconfig.generate(gstivas_xroigen, install_dir : plugins_pkgconfig_install_dir)
plugins += [gstivas_xroigen]
