# JiYuTrainer 1.7.6 — Jare-Li Refactor

> 基于 [imengyu/JiYuTrainer](https://github.com/imengyu/JiYuTrainer) 1.7.6（MIT License）的重构版。
> 原版权归 **快乐的梦鱼（imengyu）** 所有；重构、UI、构建与测试由 **Jare-Li** 维护。

## 本次更新

- 主界面改为 **960×540（16:9）**：左侧状态卡片 + 功能导航，右侧功能内容
- UDP 重放窗口放大到 **896×760**，左右分栏，输入框与按钮不再重叠
- 局域网扫描窗口放大到 **580×700**，重做表单、结果列表与状态显示
- 修复 IP 扫描：
  - 扫描范围尾部地址不再丢失（256 个地址完整覆盖）
  - `SendARP` 失败不再误判为在线主机
  - 修复扫描线程计数竞态
  - 增加起始地址不能大于结束地址的校验
- 保留原 1.7.6 UDP 协议字节、攻击模块与全部原有功能
- 回归测试：**74/74 通过，100.0%**

## 资产

| 文件 | SHA-256 |
|---|---|
| `JiYuTrainer-JareLi-1.7.6.zip` | `F3641DA6092D3176EF199335ADD57CF10C621209EDD995284EA1C07E779755F1` |
| `JiYuTrainer.exe` | `16422577523A62D9A6CECBF4C1F1061013C2FC03E563974B4BA788634F069124` |

> 如果你上传的是 GitHub Actions 重新构建的 zip，哈希会不同；请用下面的命令重新计算并替换。

## 校验 SHA-256

```powershell
Get-FileHash .\JiYuTrainer-JareLi-1.7.6.zip -Algorithm SHA256
Get-FileHash .\dist\JiYuTrainer.exe -Algorithm SHA256
```

或：

```cmd
certutil -hashfile JiYuTrainer-JareLi-1.7.6.zip SHA256
```

哈希一致只说明文件未被篡改；首次运行仍可能触发 SmartScreen。

## 系统要求

- Windows 10 / 11（x86 主程序，可在 64 位系统运行）
- 首次运行建议以管理员身份启动，以便安装 Hook/驱动
- 不支持在未授权的第三方设备上使用 UDP 重放或扫描功能

## 安装

1. 下载并解压 `JiYuTrainer-JareLi-1.7.6.zip`
2. 运行 `JiYuTrainer.exe`
3. 首次运行同意用户协议
4. 主界面可最小化到托盘；关闭窗口默认隐藏到托盘

## Windows SmartScreen 说明

首次运行从网络下载的 exe 时，Windows 可能显示：

> Windows 已保护你的电脑

这是 SmartScreen 对“未知发布者/低信誉文件”的提示，不是病毒报毒，也不代表文件损坏。

### 最快处理

1. 点击 **“更多信息”** → **“仍要运行”**
2. 如果没有“仍要运行”：
   - **先对 zip 解锁**：右键 `JiYuTrainer-JareLi-1.7.6.zip` → 属性 → 常规 → 勾选 **“解除锁定”** → 确定
   - 再解压
3. 已经解压的话，对 exe 执行：

```powershell
Unblock-File -Path .\JiYuTrainer.exe
```

或对整个目录执行：

```powershell
Get-ChildItem -Recurse | Unblock-File
```

### 为什么仍会提示

- 文件从网络下载，带有 Mark-of-the-Web
- 当前使用 `CN=Jare-Li` 的**自签名**证书，Windows 不信任该根证书
- 文件没有 Microsoft SmartScreen 信誉

自签名可以证明文件未被篡改，但不能获得微软的发布者信誉。  
`Get-AuthenticodeSignature` 显示 `UnknownError` / `UntrustedRoot` 属于自签名的正常状态。

### 如果连“仍要运行”都没有

可能是 Windows 11 **Smart App Control** 或组织策略：

- 优先使用受信任的代码签名证书
- 企业环境使用 AppLocker / WDAC 允许规则
- 不建议为了一个程序全局关闭 SmartScreen
- Smart App Control 关闭通常不可逆，请谨慎

更多说明见 [CODE_SIGNING.md](CODE_SIGNING.md) 与 [docs/FIRST-RUN.md](docs/FIRST-RUN.md)。

## 安全与授权

- 本软件用于恢复本机在机房环境中的操作自由，请遵守学校、单位和实验室规定
- UDP 重放模块可向目标主机发送命令、关机或重启请求；请仅对自己拥有或已获明确授权的设备使用
- 不要对未授权的第三方主机使用扫描或重放功能
- 本项目与极域电子教室、imengyu 官方无隶属关系

## 许可与归属

- 原项目：<https://github.com/imengyu/JiYuTrainer>（MIT）
- UDP 重放原理：<https://github.com/ht0Ruial/Jiyu_udp_attack>
- 重构：Jare-Li
- 完整许可证见 [LICENSE](LICENSE)，第三方归属见 [NOTICE](NOTICE)
- 上游 README：[docs/UPSTREAM-README.md](docs/UPSTREAM-README.md)
