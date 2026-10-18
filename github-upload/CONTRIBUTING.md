# Contributing

## Build

Requirements:

- Windows 10/11
- Visual Studio 2022 with MSVC v143
- Windows 10/11 SDK
- PowerShell 5.1+ / 7
- Python 3.9+

```powershell
.\build.ps1 -Sign
python tests\run_tests.py
```

`build.ps1 -Sign` creates a local self-signed `CN=Jare-Li` certificate for
testing. Do not commit `build/certs/*.pfx` or `password.txt`.

## Pull requests

- Keep the MIT license and original copyright notices.
- Add or update tests for behavior changes.
- Run `python tests\run_tests.py`; the release gate is >= 90%.
- Do not commit build outputs, logs, local `.ini` files, or signing keys.
- For changes to the UDP replay module, describe the authorized test setup and
  keep the original packet layout unless the change is explicitly justified.
- For UI changes, include before/after screenshots and keep the existing DOM
  ids used by the C++ event handlers.

## Code style

- C++: match the existing file style, avoid unrelated refactors.
- TIScript/HTML/CSS: keep the existing 16:9 layout and color scheme.
- Python: standard library only for the test runner.

## Security

Do not open public issues with working exploit details. See
[SECURITY.md](SECURITY.md).
