# journald Ntfy Bot

A simple daemon that connects to
[journald's gateway](https://www.freedesktop.org/software/systemd/man/systemd-journal-gatewayd.html) and listens for
specified patterns of journal entries and relays them to an Ntfy instance.

## Building using docker

```bash
$ docker build . --output=dockerbuild
$ tree dockerbuild 
dockerbuild
├── journald-bot
└── journald-bot-2.0.0-Linux.deb
```
