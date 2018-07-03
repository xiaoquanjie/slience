
#proto_library(
#)

cc_library(
    name = 'slience',
    srcs = [
	   'base/buffer.cpp',
	   'base/condition.cpp',
	   'base/mutexlock.cpp',
	   'base/logger.cpp',
	   'base/string_util.cpp',
	   'base/timer.cpp',
	   'base/thread.cpp',
	   'socket/error.cpp',
	   'netio/sync_connector.cpp',
	   'netio/http_connector.cpp',
	   'netio/tcp_connector.cpp',
	   'netio/netio.cpp',
    ],

    deps = [
		'#pthread',
    ],

    #link_all_symbols = True,
)


