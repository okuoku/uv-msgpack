set(CMAKE_LEGACY_CYGWIN_WIN32 0)
PROJECT(uv)
cmake_minimum_required(VERSION 2.8)

include_directories(include include/uv-private src src/ares)
add_definitions(-DHAVE_CONFIG_H)

set(UV_COMMON_SRCS
    include/ares.h
    include/ares_version.h
    include/uv.h
    include/uv-private/ngx-queue.h
    include/uv-private/tree.h
    src/cares.c
    src/fs-poll.c
    src/uv-common.c
    src/uv-common.h
    src/ares/ares_cancel.c
    src/ares/ares__close_sockets.c
    src/ares/ares_data.c
    src/ares/ares_data.h
    src/ares/ares_destroy.c
    src/ares/ares_dns.h
    src/ares/ares_expand_name.c
    src/ares/ares_expand_string.c
    src/ares/ares_fds.c
    src/ares/ares_free_hostent.c
    src/ares/ares_free_string.c
    src/ares/ares_getenv.h
    src/ares/ares_gethostbyaddr.c
    src/ares/ares_gethostbyname.c
    src/ares/ares__get_hostent.c
    src/ares/ares_getnameinfo.c
    src/ares/ares_getopt.c
    src/ares/ares_getopt.h
    src/ares/ares_getsock.c
    src/ares/ares_init.c
    src/ares/ares_ipv6.h
    src/ares/ares_library_init.c
    src/ares/ares_library_init.h
    src/ares/ares_llist.c
    src/ares/ares_llist.h
    src/ares/ares_mkquery.c
    src/ares/ares_nowarn.c
    src/ares/ares_nowarn.h
    src/ares/ares_options.c
    src/ares/ares_parse_aaaa_reply.c
    src/ares/ares_parse_a_reply.c
    src/ares/ares_parse_mx_reply.c
    src/ares/ares_parse_ns_reply.c
    src/ares/ares_parse_naptr_reply.c
    src/ares/ares_parse_ptr_reply.c
    src/ares/ares_parse_soa_reply.c
    src/ares/ares_parse_srv_reply.c
    src/ares/ares_parse_txt_reply.c
    src/ares/ares_platform.h
    src/ares/ares_private.h
    src/ares/ares_process.c
    src/ares/ares_query.c
    src/ares/ares__read_line.c
    src/ares/ares_rules.h
    src/ares/ares_search.c
    src/ares/ares_send.c
    src/ares/ares_setup.h
    src/ares/ares_strcasecmp.c
    src/ares/ares_strcasecmp.h
    src/ares/ares_strdup.c
    src/ares/ares_strdup.h
    src/ares/ares_strerror.c
    src/ares/ares_timeout.c
    src/ares/ares__timeval.c
    src/ares/ares_version.c
    src/ares/ares_writev.c
    src/ares/ares_writev.h
    src/ares/bitncmp.c
    src/ares/bitncmp.h
    src/ares/inet_net_pton.c
    src/ares/inet_net_pton.h
    src/ares/inet_ntop.c
    src/ares/inet_ntop.h
    src/ares/nameser.h
    src/ares/setup_once.h
    src/ares/windows_port.c
    )


set(UV_WIN32_SRCS
    include/uv-private/uv-win.h
    src/ares/config_win32/ares_config.h
    src/ares/windows_port.c
    src/ares/ares_getenv.c
    src/ares/ares_iphlpapi.h
    src/ares/ares_platform.c
    src/win/async.c
    src/win/core.c
    src/win/dl.c
    src/win/error.c
    src/win/fs.c
    src/win/fs-event.c
    src/win/getaddrinfo.c
    src/win/handle.c
    src/win/handle-inl.h
    src/win/internal.h
    src/win/loop-watcher.c
    src/win/pipe.c
    src/win/thread.c
    src/win/poll.c
    src/win/process.c
    src/win/process-stdio.c
    src/win/req.c
    src/win/req-inl.h
    src/win/stream.c
    src/win/stream-inl.h
    src/win/tcp.c
    src/win/tty.c
    src/win/threadpool.c
    src/win/timer.c
    src/win/udp.c
    src/win/util.c
    src/win/winapi.c
    src/win/winapi.h
    src/win/winsock.c
    src/win/winsock.h
    )

set(UV_POSIX_SRCS
    include/uv-private/eio.h
    include/uv-private/ev.h
    include/uv-private/uv-unix.h
    src/unix/async.c
    src/unix/core.c
    src/unix/dl.c
    src/unix/error.c
    src/unix/thread.c
    src/unix/process.c
    src/unix/internal.h
    src/unix/eio/ecb.h
    src/unix/eio/eio.c
    src/unix/eio/xthread.h
    src/unix/error.c
    src/unix/ev/ev.c
    src/unix/ev/ev_vars.h
    src/unix/ev/ev_wrap.h
    src/unix/ev/event.h
    src/unix/fs.c
    src/unix/internal.h
    src/unix/loop.c
    src/unix/loop-watcher.c
    src/unix/pipe.c
    src/unix/poll.c
    src/unix/process.c
    src/unix/tcp.c
    src/unix/thread.c
    src/unix/timer.c
    src/unix/tty.c
    src/unix/udp.c
    src/unix/uv-eio.c
    src/unix/uv-eio.h
    )

set(UV_TEST_SRCS
    test/blackhole-server.c
    test/echo-server.c
    test/run-tests.c
    test/runner.c
    test/runner.h
    test/test-get-loadavg.c
    test/task.h
    test/test-util.c
    test/test-async.c
    test/test-error.c
    test/test-callback-stack.c
    test/test-callback-order.c
    test/test-connection-fail.c
    test/test-cwd-and-chdir.c
    test/test-delayed-accept.c
    test/test-fail-always.c
    test/test-fs.c
    test/test-fs-event.c
    test/test-get-currentexe.c
    test/test-get-memory.c
    test/test-getaddrinfo.c
    test/test-gethostbyname.c
    test/test-getsockname.c
    test/test-hrtime.c
    test/test-idle.c
    test/test-ipc.c
    test/test-ipc-send-recv.c
    test/test-list.h
    test/test-loop-handles.c
    test/test-walk-handles.c
    test/test-multiple-listen.c
    test/test-pass-always.c
    test/test-ping-pong.c
    test/test-pipe-bind-error.c
    test/test-pipe-connect-error.c
    test/test-platform-output.c
    test/test-poll.c
    test/test-poll-close.c
    test/test-process-title.c
    test/test-ref.c
    test/test-run-once.c
    test/test-semaphore.c
    test/test-shutdown-close.c
    test/test-shutdown-eof.c
    test/test-spawn.c
    test/test-fs-poll.c
    test/test-stdio-over-pipes.c
    test/test-tcp-bind-error.c
    test/test-tcp-bind6-error.c
    test/test-tcp-close.c
    test/test-tcp-close-while-connecting.c
    test/test-tcp-connect-error-after-write.c
    test/test-tcp-shutdown-after-write.c
    test/test-tcp-flags.c
    test/test-tcp-connect-error.c
    test/test-tcp-connect-timeout.c
    test/test-tcp-connect6-error.c
    test/test-tcp-write-error.c
    test/test-tcp-write-to-half-open-connection.c
    test/test-tcp-writealot.c
    test/test-threadpool.c
    test/test-mutexes.c
    test/test-thread.c
    test/test-timer-again.c
    test/test-timer.c
    test/test-tty.c
    test/test-udp-dgram-too-big.c
    test/test-udp-ipv6.c
    test/test-udp-options.c
    test/test-udp-send-and-recv.c
    test/test-udp-multicast-join.c
    test/test-counters-init.c
    test/test-dlerror.c
    test/test-udp-multicast-ttl.c
    )

set(UV_BENCHMARK_SRCS
    test/benchmark-ares.c
    test/benchmark-async.c
    test/benchmark-async-pummel.c
    test/benchmark-fs-stat.c
    test/benchmark-getaddrinfo.c
    test/benchmark-list.h
    test/benchmark-loop-count.c
    test/benchmark-million-timers.c
    test/benchmark-ping-pongs.c
    test/benchmark-pound.c
    test/benchmark-pump.c
    test/benchmark-sizes.c
    test/benchmark-spawn.c
    test/benchmark-thread.c
    test/benchmark-tcp-write-batch.c
    test/benchmark-udp-packet-storm.c
    test/dns-server.c
    test/echo-server.c
    test/blackhole-server.c
    test/run-benchmarks.c
    test/runner.c
    test/runner.h
    test/task.h
    )

# set UV_SRCS and UV_LIBS

if(WIN32)
    include_directories(src/ares/config_win32)
    add_definitions(-D_WIN32_WINNT=0x0600 -DEIO_STACKSIZE=262144 -D_GNU_SOURCE)
    set(UV_SRCS ${UV_WIN32_SRCS} ${UV_COMMON_SRCS})
    set(UV_TESTRUNNER_SRCS test/runner-win.c test/runner-win.h)
    set(UV_LIBS ws2_32 psapi iphlpapi)
else()
    # POSIX common
    include_directories(src/unix/ev)
    add_definitions(-D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -D_GNU_SOURCE -DEIO_STACKSIZE=262144)
    set(UV_TESTRUNNER_SRCS test/runner-unix.c test/runner-unix.h)

    # OS specific thingy
    # FIXME: solaris here
    if(APPLE)
        add_definitions(
            -D_DARWIN_USE_64_BIT_INODE=1
            -DEV_CONFIG_H=\"config_darwin.h\"
            -DEIO_CONFIG_H=\"config_darwin.h\")
        include_directories(src/ares/config_darwin)
        set(UV_SRCS
            ${UV_POSIX_SRCS}
            ${UV_COMMON_SRCS}
            src/unix/darwin.c
            src/unix/kqueue.c)
        set(UV_LIBS m)
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
        add_definitions(-DEV_CONFIG_H=\"config_linux.h\" -DEIO_CONFIG_H=\"config_linux.h\")
        include_directories(src/ares/config_linux)
        set(UV_SRCS
            ${UV_POSIX_SRCS}
            ${UV_COMMON_SRCS}
            src/unix/linux/linux-core.c
            src/unix/linux/inotify.c
            src/unix/linux/syscalls.c
            # src/unix/linux/syscalls.h
            )
        set(UV_LIBS rt m dl)
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
        add_definitions(-DEV_CONFIG_H=\"config_freebsd.h\" -DEIO_CONFIG_H=\"config_freebsd.h\")
        include_directories(src/ares/config_freebsd)
        set(UV_SRCS
            ${UV_POSIX_SRCS}
            ${UV_COMMON_SRCS}
            src/unix/freebsd.c
            src/unix/kqueue.c)
        set(UV_LIBS m pthread kvm)
    elseif(${CMAKE_SYSTEM_NAME} STREQUAL "CYGWIN")
        add_definitions(-DEV_CONFIG_H=\"config_cygwin.h\" 
            -DEIO_CONFIG_H=\"config_cygwin.h\")
        include_directories(src/ares/config_cygwin)
        set(UV_SRCS
            ${UV_POSIX_SRCS}
            ${UV_COMMON_SRCS}
            src/unix/cygwin.c
            )
        set(UV_LIBS rt m dl)
    else()
        message(FATAL_ERROR
            "We don't support this system for now: ${CMAKE_SYSTEM_NAME}")
    endif()
endif()

add_library(uv ${UV_SRCS})


# Tests

option(UV_BUILD_TESTS "Build libuv tests" OFF)
option(UV_BUILD_BENCHMARKS "Build libuv benchmarks" OFF)

if(UV_BUILD_TESTS)
    add_executable(uv-tests ${UV_TEST_SRCS} ${UV_TESTRUNNER_SRCS})
    target_link_libraries(uv-tests uv ${UV_LIBS})
endif()

if(UV_BUILD_BENCHMARKS)
    add_executable(uv-benchmarks 
        ${UV_BENCHMARK_SRCS}
        ${UV_TESTRUNNER_SRCS} test/${nam})
    target_link_libraries(uv-benchmarks uv ${UV_LIBS})
endif()

