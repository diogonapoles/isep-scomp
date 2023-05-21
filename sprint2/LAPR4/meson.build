project('shared-boards', 'c',
  version : '0.1',
  default_options : ['warning_level=3'])

flags = [ '-D_XOPEN_SOURCE=500', '-D_POSIX_C_SOURCE=200809L' ]

cc = meson.get_compiler('c')

librt = cc.find_library('rt', required: false)

deps = [ librt, dependency('threads') ]

add_project_arguments(flags, language: 'c')

client_src = [ 'client.c', 'helpers.c' ]

client = executable('client', client_src,
  dependencies: deps,
  install : true)


server_src = [ 'server.c', 'helpers.c' ]

server = executable('server', server_src,
  dependencies: deps,
  install : true)


test('basic', client)
