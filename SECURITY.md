# Security Policy

## Supported versions

| Version | Supported |
|---|---|
| 1.7.7.x (Jare-Li Refactor) | ✅ |
| older / upstream builds | ❌ |

## Reporting a vulnerability

Please use GitHub's **private vulnerability reporting** for this repository.
Do not open a public issue for exploitable details, bypass techniques, or
third-party target information.

If private reporting is unavailable, open a minimal issue asking the
maintainer for a private contact channel; do not include reproduction details.

## Scope

This project interacts with the JiYu classroom client (`StudentMain.exe`) and
can modify windows, processes, and network behavior on the local machine.

- Use it only on machines you own or are explicitly authorized to test.
- The UDP replay module can send commands, shutdown, or restart requests to
  target machines. Unauthorized use is prohibited.
- Do not use the LAN scanner or UDP module against third-party networks.

## Out of scope

- Vulnerabilities in JiYu itself or in third-party components (report upstream).
- Damage caused by using the tool on machines you do not own.
- SmartScreen warnings caused by self-signed certificates.

## Disclosure

We aim to acknowledge reports within 7 days and provide a fix or mitigation
within 30 days for valid issues, depending on severity and complexity.
