# msg — FIFO messaging and buffered delivery

`msg` defines structured messages, per-process message identity, named-FIFO
transport, shared message buffering, and internal control messages. It uses
[addr](https://github.com/smythereen/addr) for address/name conversion. It is
not, by itself, a multi-host network transport: the network bridge is in
[near](https://github.com/smythereen/near).

It builds with the [base](https://github.com/smythereen/base) unit make system, which handles C, C++, and Rust, including large mixed-language projects.

## Public interface and message contract

`msg.h` is the application header and includes `addr.h`. It exports
`MsgHead_t`, `MsgBody_t`, `Msg_t`, `Msg_p`, `MsgInit`, `MsgInitFake`, `MsgSend`,
`MsgRead`, `MsgCopy`, `MsgPath`, `MsgFile`, and process identity globals.
`msglocal.h` is private implementation/control-message detail.

`MAX_MSG_LEN` is 1024 bytes including the header, not 1024 bytes of payload.
`MsgSend(address, function, buffer, size, expire)` expects a writable buffer
starting with `MsgHead_t`; **do not pass just the payload**. Its size is the
complete message size, and it fills header fields in place. The final argument
is seconds until expiration. `MsgRead(&message, timeout_in_seconds)` returns a
function code, including `MSG_TIMEOUT` (-1); use the returned code rather than
assuming every read delivered an application message.

A real send call, after initializing and provisioning the local FIFO namespace:

```c
Msg_t message = {0};
message.body.data[0] = 'x';
MsgSend(Numbers_To_Address(1, 2, 3, 4), 7,
        &message, sizeof(MsgHead_t) + 1, 30);
```

This is a call-shape example, not a claim that a receiver already exists.
`MsgInit(local_address, quit_callback, print_callback)` establishes process
identity and local messaging resources. See `msgstub.c` for the actual read loop.

## Dependencies and build

Place `msg/` beside [base](https://github.com/smythereen/base)'s shared files.
Direct headers include `addr`, `far`, `map`, `atomic` (`cmpxchg.h`), `mio`
(`mem_io.h`), and `sys`; the makefile links `addr map sem far now sys`.
The original snapshot has no `UNIT.deps`, so source fetching does not promise
a complete dependency closure in a custom minimal project.

```sh
export UNITS_CATALOG_URL=https://unit-catalog.dejavu-archive.workers.dev
./repo/repo.sh add addr
./repo/repo.sh add msg
HOME=$PWD make -C msg header library
HOME=$PWD make -C msg msgstub
```

Build selected companion headers/libraries first. The archive is
`lib/libmsg.a`. The `msg` command additionally includes `miomsg.h` and its
makefile target links `-lmiomsg -lmio -lmsg ...`. **The separate `miomsg`
adapter is not included in this repository or published by this snapshot.**
Supply that unit before building the `msg` command or the full `product` target;
core `mio` alone is not a replacement for `MioMsgInit`/MSGADDR registration.

## C and C++

This unit is usable from **C and C++**. Its public headers already supply
`extern "C"` guards, including the exported shared-memory header where present.
Compile the implementation `.c` files with GCC and link that C library into
your C++ application with G++; do not reinterpret the whole legacy C source
as C++. GNU language extensions and the POSIX/Linux environment are assumed.
No additional caller-side linkage wrapper is needed. Mutable string arguments
are represented by arrays, not C++ string literals cast to `char *`.

The following example is valid C and intended for GNU C++ as well:

```c
#include <msg.h>
int main(void) {
    Msg_t source = {0}, copy = {0};
    source.head.function = 7;
    source.head.length = sizeof(MsgHead_t);
    MsgCopy(&copy, &source);
    return copy.head.function != 7;
}
```

Save it as `smoke.c` or use [`examples/smoke.c`](examples/smoke.c). From a
project root with headers in `cmn/` and built libraries in `lib/`:

```sh
gcc -std=gnu11 -Icmn msg/examples/smoke.c -Llib \
  -Wl,--start-group -lmsg -laddr -lmap -lsem -lfar -lnow -lsys -Wl,--end-group -o msg-smoke
g++ -std=gnu++17 -x c++ -Icmn -c msg/examples/smoke.c -o msg-caller.o
g++ msg-caller.o -Llib \
  -Wl,--start-group -lmsg -laddr -lmap -lsem -lfar -lnow -lsys -Wl,--end-group -o msg-smoke-cpp
```

Caller objects precede libraries; the GNU linker group handles cyclic static
unit dependencies. A narrower caller may need fewer libraries.

## Programs, runtime, and operations

`msgstub local.address [timeout]` is the actual diagnostic reader. It calls
`MsgInit`, then loops on `MsgRead`; it is long-running, not a one-shot command:

```sh
bin/msgstub 1.2.3.4 1
```

Use a provisioned, isolated project runtime with the base `FILE_PATH`,
`DATA_PATH`, `PIPE_PATH`, and `MEMORY_PATH` conventions. Do not point test
processes at production pipes or name tables. The `msg` command and shipped
`.msg` tables are legacy diagnostic/configuration material; inspect and adapt
them rather than treating them as a generic multi-server deployment recipe.

FIFO buffering is not a durable distributed queue. Applications must own
message validation, bounded load, timeout policy, restart behavior, and
cross-host transport. Internal message functions can alter tracing and process
behavior; keep that control plane trusted.

## Verification

All 17 C files compiled individually with GCC 14 when the separate adapter
header was available. The publication audit did not publish that header here.
The `MsgCopy` API smoke example compiled, linked, and ran. This is not proof
of end-to-end FIFO delivery or a complete public-only product build. C++
`extern "C"` coverage was inspected, but the publication environment lacked
a C++ frontend, so no C++ compile/run is claimed.

## Source and license

Published from the current Guardian unit directory as a clean snapshot, not
its private Git history. C/header/makefile bytes are preserved; build outputs,
backups, generated dependency files, and host deployment initialization are
excluded. New examples and this guide are documentation additions. No release
tag is created by this publication. See `LICENSE` and `LICENSES/` for the
existing Apache-2.0 OR GPL-2.0-or-later licensing.
