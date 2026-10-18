# Code signing policy

Free code signing provided by [SignPath.io](https://about.signpath.io),
certificate by [SignPath Foundation](https://signpath.org).

- Committers and reviewers: Jare-Li ([@Jare-Li](https://github.com/Jare-Li))
- Approvers: Jare-Li ([@Jare-Li](https://github.com/Jare-Li))

## Scope

This policy applies to release binaries built from this repository. Every
release candidate is built from the tagged source in CI, reviewed, and approved
by an approver before a signing request is submitted.

## Privacy

This program does not collect telemetry and does not sell or share user data.

- The updater contacts the configured update service only to check for a new
  version or download an update. This can be disabled in the settings.
- The LAN scanner sends ARP requests to addresses the user enters.
- The UDP replay module sends packets only to targets the user explicitly
  enters, and only when the user clicks the send button.

No information is transferred to other networked systems unless specifically
requested by the user or the person installing or operating the program.

## Release verification

- Source: this repository at the release tag.
- Build: `build.ps1` on Windows with Visual Studio 2022 (v143) and the
  Windows 10/11 SDK.
- Tests: `python tests/run_tests.py` (the release gate is >= 90%).
- Signed binaries are published as GitHub Release assets together with their
  SHA-256 hashes.

## Note

This project contains a UDP replay module that can send commands, shutdown, or
restart requests to target machines. It is intended only for machines the user
owns or is explicitly authorized to test. SignPath Foundation's terms exclude
hacking tools and software that circumvents security measures; acceptance of
this project is at SignPath Foundation's sole discretion.
