{
  'includes':[
  ],

  'variables': {
    'project_path':'.',
    'boost_path':'/usr/include/boost/',
  },

  'target_defaults': {
    'cflags':['-g', '-O2', '-Wall', '-fPIC'],
    'include_dirs':[
      '<(project_path)/',
      '<(project_path)/src',
    ],
  },

  'targets': [
     {
      'target_name' : 'kit',
      'type' : 'static_library',
      'include_dirs':[
        '<(boost_path)/',
      ],
      'link_settings': {
        'libraries' : [
          '-lpthread',
          '-lboost_thread',
          '-lboost_system',
        ]
      },
      'defines' : [],
      'dependencies' : [
      ],
      'sources' : [
        '<(project_path)/src/util/log.cc',
        '<(project_path)/src/util/config.cc',
        '<(project_path)/src/util/str_util.cc',
        '<(project_path)/src/util/thread_pool.cc',
        '<(project_path)/src/util/file_util.cc',
        '<(project_path)/src/util/process.cc',
        '<(project_path)/src/net/poller.cc',
        '<(project_path)/src/net/timer.cc',
        '<(project_path)/src/net/event_loop.cc',
        '<(project_path)/src/net/socket.cc',
        '<(project_path)/src/net/buffer.cc',
        '<(project_path)/src/net/status.cc',
        '<(project_path)/src/net/connection.cc',
        '<(project_path)/src/net/server_connection.cc',
        '<(project_path)/src/net/client_connection.cc',
        '<(project_path)/src/net/acceptor.cc',
        '<(project_path)/src/net/server.cc',
        '<(project_path)/src/net/connector.cc',
        '<(project_path)/src/net/client.cc',
      ],
    },
     {
      'target_name' : 'log_test',
      'type' : 'executable',
      'include_dirs':[
      ],
      'link_settings': {
        'libraries' : [
        ]
      },
      'defines' : [],
      'dependencies' : [
         'kit',
      ],
      'sources' : [
        '<(project_path)/test/log_test.cc',
      ],
    },
    {
      'target_name' : 'config_test',
      'type' : 'executable',
      'include_dirs':[
      ],
      'link_settings': {
        'libraries' : [
        ]
      },
      'defines' : [],
      'dependencies' : [
         'kit',
      ],
      'sources' : [
        '<(project_path)/test/config_test.cc',
      ],
    },
    {
      'target_name' : 'echo_server',
      'type' : 'executable',
      'include_dirs':[
      ],
      'link_settings': {
        'libraries' : [
        ]
      },
      'defines' : [],
      'dependencies' : [
         'kit',
      ],
      'sources' : [
        '<(project_path)/example/echo/echo_server.cc',
      ],
    },
  ],
}

