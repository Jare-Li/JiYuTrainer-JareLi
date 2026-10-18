# Test Report

- Date: 2026-10-18T21:58:02
- Passed: 68
- Failed: 0
- Total: 68
- Pass rate: 100.0%

| Test | Result | Detail |
|---|---|---|
| `core/ip_valid_basic` | PASS |  |
| `core/ip_valid_bounds` | PASS |  |
| `core/ip_invalid_256` | PASS |  |
| `core/ip_invalid_short` | PASS |  |
| `core/ip_invalid_five` | PASS |  |
| `core/ip_invalid_letters` | PASS |  |
| `core/ip_invalid_empty` | PASS |  |
| `core/ip_invalid_inner_space` | PASS |  |
| `core/ip_trim_ok` | PASS |  |
| `core/ip_leading_zero_ok` | PASS |  |
| `core/split_two` | PASS |  |
| `core/split_spaces_and_empty` | PASS |  |
| `core/split_empty` | PASS |  |
| `core/trim` | PASS |  |
| `core/basepack_header_0` | PASS |  |
| `core/basepack_header_1` | PASS |  |
| `core/basepack_header_2` | PASS |  |
| `core/basepack_header_3` | PASS |  |
| `core/basepack_hash_0` | PASS |  |
| `core/basepack_hash_1` | PASS |  |
| `core/basepack_hash_2` | PASS |  |
| `core/basepack_hash_3` | PASS |  |
| `core/packet_control_offsets` | PASS |  |
| `core/packet_message_offset` | PASS |  |
| `core/packet_message_prefix` | PASS |  |
| `core/packet_message_zero_tail` | PASS |  |
| `core/packet_command_offset` | PASS |  |
| `core/packet_command_prefix` | PASS |  |
| `core/packet_unicode_utf16le` | PASS |  |
| `core/packet_shutdown_ignores_payload` | PASS |  |
| `core/packet_command_max_chars` | PASS |  |
| `core/packet_command_capped` | PASS |  |
| `core/packet_sizes` | PASS |  |
| `core/summary` | PASS | 33 passed, 0 failed |
| `artifact/exe_exists` | PASS | 4792648 bytes |
| `artifact/hook_exists` | PASS | 668488 bytes |
| `artifact/sciter_exists` | PASS | 5955632 bytes |
| `artifact/build_script` | PASS | build.ps1 |
| `artifact/test_script` | PASS | tests/run_tests.py |
| `pe/exe_x86_gui` | PASS | machine=0x014C subsystem=2 |
| `pe/hook_x86` | PASS | machine=0x014C |
| `version/readable` | PASS |  |
| `version/company_jareli` | PASS |  |
| `version/description_jareli` | PASS |  |
| `version/copyright` | PASS |  |
| `version/file_version` | PASS |  |
| `version/product_name` | PASS |  |
| `signature/present` | PASS |  |
| `signature/subject_jareli` | PASS |  |
| `ui/main_ids_complete` | PASS |  |
| `ui/main_no_duplicate_ids` | PASS |  |
| `ui/main_version_label` | PASS |  |
| `ui/main_window_title` | PASS |  |
| `ui/window_16x9_cpp` | PASS |  |
| `ui/window_16x9_html` | PASS |  |
| `ui/main_res_refs` | PASS |  |
| `ui/attack_ids_complete` | PASS |  |
| `ui/attack_warning` | PASS |  |
| `ui/attack_preserved_label` | PASS |  |
| `feature/handlers_present` | PASS |  |
| `attack/packet_calls` | PASS |  |
| `attack/window_methods` | PASS |  |
| `resource/main_html` | PASS |  |
| `resource/attack_html` | PASS |  |
| `legal/mit_license` | PASS |  |
| `legal/readme_attribution` | PASS |  |
| `legal/notice_attribution` | PASS |  |
| `version/current_constant` | PASS |  |
