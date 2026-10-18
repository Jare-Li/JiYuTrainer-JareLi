# JiYu Trainer 1.7.6 — Jare-Li Refactor

> 基于 [imengyu/JiYuTrainer](https://github.com/imengyu/JiYuTrainer) 1.7.6（MIT License）的重构版本。
> 原版权归 **快乐的梦鱼（imengyu）** 所有；重构、构建与签名由 **Jare-Li** 完成。

- 版本：`1.7.6.0515 (Jare-Li Refactor)`
- 平台：Windows 7 / 8 / 8.1 / 10 / 11，x86（32 位主程序，可在 64 位系统运行）
- 主窗口：`960×540`（16:9），左侧状态/功能导航，右侧内容区
- 许可：MIT（见 `LICENSE`）
- 签名：自签名 Authenticode 证书 `CN=Jare-Li`（默认不受系统信任）

---

## 1. 重构目标

| 目标 | 结果 |
|---|---|
| 保留全部原功能 | 是。主界面、设置、托盘、热键、更新、Hook、驱动、UDP 重放窗口均保留 |
| 保留攻击模块 | 是。`AttackWindow` / `JyUdpAttack` / 包字节布局原样保留，未增加攻击方式，未修改协议 |
| 界面简洁 | 主窗口改为 **960×540（16:9）左右分栏**：左侧状态卡片 + 功能导航，右侧显示当前功能；保留浅色配色与原控件行为 |
| 构建可复现 | 是。VS2022 v143 + Windows SDK，`build.ps1` 一键构建 |
| 测试通过率 ≥ 90% | 是。**68/68 = 100.0%**（2026-10-18 本机实测） |
| Jare-Li 签名 | 版本信息、关于页署名、自签名 Authenticode 证书三处一致 |

## 2. 功能清单（全部保留）

- 全屏广播自动转窗口，保留教师演示画面
- 防教师控制键鼠、防黑屏安静、防强制关机/重启
- 反监视：阻止教师端查看本机屏幕
- 防极域结束进程、防极域关闭窗口
- 监控极域远程执行进程，可选择允许/拒绝/全部禁止
- 强杀 / 重新启动 `StudentMain.exe`
- 读取极域卸载/解锁密码（注册表模式 + 解密模式）
- 卸载极域网络过滤驱动（`TDNetFilter`）
- 托盘图标、全局热键显示/隐藏、紧急全屏
- 自动检查更新（上游更新服务；当前版本与上游 1.7.6 对齐）
- 内置驱动、Hook DLL、Sciter 运行时，单 exe 自解压
- **UDP 重放窗口（原样保留）**：文字消息、CMD、远程关机、远程重启、局域网扫描、端口检测

> 攻击模块的原始用途、协议与字节布局均未改变；本次只做了 UI 重排和“可测试化”抽取，包字节由回归测试锁定。

## 3. 主要代码改动

| 文件 | 改动 |
|---|---|
| `JiYuTrainerUI/MainWindow.html` | 16:9 左右分栏 UI：左侧状态 + 功能导航，右侧内容区；保留 83 个原 DOM id、全部 C++ 事件绑定与 TIScript 行为 |
| `JiYuTrainerUI/AttackWindow.html` | 全新表单式 UI；保留全部原控件 id 与警告文案 |
| `JiYuTrainer/JyPacket.h/.cpp` | 从 `JyUdpAttack` 抽取纯包构造；`DMOC` 头、4 种基础包、偏移 56/100 与 1.7.6 完全一致 |
| `JiYuTrainer/IpUtils.h/.cpp` | IPv4 校验与目标列表解析；支持逗号分隔与首尾空格 |
| `JiYuTrainer/JyUdpAttack.cpp` | 发送函数改用 `JyPacket::Build`；报文语义不变 |
| `JiYuTrainerUI/AttackWindow.cpp` | 改用 `IpUtils`；窗口尺寸 620×640 |
| `JiYuTrainer/JiYuTrainer.rc` | 版本信息、Jare-Li 署名、MIT 归属与协议文案 |
| `tests/CoreTests.cpp` | 33 项 C++ 单元/回归测试（IP、UTF-16、包字节、哈希、边界） |
| `tests/run_tests.py` | 构建产物、版本信息、签名、UI id、资源、许可证、功能保留等回归测试 |
| `build.ps1` | 可复现构建 + 自签名 + 打包 |

### 包字节兼容性

`JyPacket` 的 4 个 128 字节基础包由原 `JyUdpAttack.h` 中的 `basePack` 原样迁移，回归测试用 FNV-1a 锁定：

| 包类型 | FNV-1a |
|---|---|
| 文字消息 | `0x28135a31` |
| CMD | `0x20fab8f6` |
| 重启 | `0xcd22edd4` |
| 关机 | `0xddcac8d6` |

有效载荷偏移：消息 `56`，CMD `100`。CMD 载荷上限为 462 个 UTF-16 字符（1024 字节包的安全上限），避免原实现中超长命令导致的 `memcpy_s` 越界。

## 4. 构建

环境：

- Visual Studio 2022，MSVC v143，Windows 10/11 SDK（本机验证：`10.0.26100.0`）
- PowerShell 5.1+ / 7
- Python 3.9+（仅测试脚本使用）

```powershell
# 在仓库根目录
pwsh -File .\build.ps1 -Sign
# 或者跳过签名 / 跳过测试
pwsh -File .\build.ps1
pwsh -File .\build.ps1 -Sign -SkipTests
```

输出：

- `JiYuTrainer-1.7.6/Release/JiYuTrainer.exe`
- `dist/`：exe、Hook DLL、sciter.dll、LICENSE、NOTICE、README
- `JiYuTrainer-JareLi-1.7.6.zip`

说明：

- 主程序会把 `JiYuTrainerHooks.dll` 与 `sciter.zip` 作为资源嵌入；构建脚本先签名 Hook DLL，再构建主程序，因此嵌入的 Hook 也带签名。
- 驱动 `JiYuTrainerDriver.sys` 保持上游原文件，未重新编译、未替换、未签名，避免破坏其加载兼容性。
- `sciter.dll` 为上游随包运行时，未改动、未签名。

## 5. 测试与通过率

运行：

```powershell
python tests\run_tests.py
```

测试分三层：

1. **C++ 核心测试**：IP 校验、列表解析、UTF-16LE、4 种基础包哈希、偏移、零填充、超长载荷上限。
2. **构建/资源测试**：PE 架构、GUI 子系统、版本信息、HTML 资源嵌入、文件完整性。
3. **界面/功能/许可测试**：83 个主界面 id 完整且无重复、攻击窗口 id、功能处理函数、MIT 归属、签名主体。

```text
RESULT: 68/68 passed, 0 failed, 100.0%
```

通过率按“通过数 / 总数”计算，发布门槛为 **≥ 90%**。本次实测 **68/68 = 100.0%**，结果写入 `test-report.md` 与 `test-results.json`。

## 6. 签名

`build.ps1 -Sign` 会：

1. 在内存中创建自签名代码签名证书 `CN=Jare-Li`（10 年，Code Signing EKU，不写入系统证书存储）。
2. 导出 `build/certs/Jare-Li.pfx`、`build/certs/Jare-Li.cer` 与本地密码文件。
3. 用 SHA-256 签名 `JiYuTrainerHooks.dll` 与 `JiYuTrainer.exe`（无时间戳服务）。

验证：

```powershell
Get-AuthenticodeSignature .\dist\JiYuTrainer.exe | Format-List Status, StatusMessage, SignerCertificate
```

预期：`SignerCertificate.Subject` 为 `CN=Jare-Li`；`Status` 为 `UnknownError` 或 `UntrustedRoot`，因为自签名根证书未安装到“受信任的根证书颁发机构”。这不是签名损坏，而是自签名的预期状态。若需要在测试机上消除该提示，可将 `build/certs/Jare-Li.cer` 导入当前用户的“受信任的根证书颁发机构”，但这会改变系统信任配置，请自行评估。

## 7. 安全与使用边界

- 本软件用于**恢复本机在机房环境中的操作自由**，请遵守学校、单位和实验室的管理规定。
- UDP 重放窗口会向目标主机的极域端口发送原始数据包，可能执行命令、关机或重启。**仅可对你自己拥有或已获得明确书面授权的设备使用。**
- 本次重构**没有**增强攻击成功率、没有增加新的攻击方式、没有做免杀/绕过检测，也没有对真实第三方主机做攻击测试。
- 软件按 MIT 协议“按原样”提供，不附带任何担保；使用造成的后果由使用者承担。

## 8. 上游与第三方

- 上游：<https://github.com/imengyu/JiYuTrainer>（MIT）
- UDP 重放原理：<https://github.com/ht0Ruial/Jiyu_udp_attack>
- Sciter：<https://sciter.com/>（随上游分发）
- curl、mhook、MemoryModule、XZip/XUnZip：见上游 `README.md`
- 原版权声明与完整 MIT 文本见 `LICENSE`、`NOTICE`。
