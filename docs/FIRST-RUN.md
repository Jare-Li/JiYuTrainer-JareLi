# 首次运行与 SmartScreen

首次运行从网络下载的 exe 时，Windows 可能显示“Windows 已保护你的电脑”。这是 SmartScreen 对“未知发布者/低信誉文件”的提示，不代表文件损坏。

## 最快处理

1. 点击“更多信息” → “仍要运行”。
2. 如果没有“仍要运行”：右键 `JiYuTrainer.exe` → 属性 → 常规 → 勾选“解除锁定” → 确定。
3. 或者在 PowerShell 中执行：

```powershell
Unblock-File -Path .\JiYuTrainer.exe
```

如果是从 zip 解压的，先对 zip 执行 `Unblock-File` 再解压，或解压后对整个目录执行：

```powershell
Get-ChildItem -Recurse | Unblock-File
```

## 为什么仍会提示

- 文件来自网络，带有 Mark-of-the-Web；
- 当前是 `CN=Jare-Li` 自签名证书，Windows 不信任该根证书；
- 文件没有 Microsoft SmartScreen 信誉。

自签名只能证明文件未被篡改，不能获得微软的发布者信誉。

## 仅在自己的机器上消除“未知发布者”

```powershell
Import-Certificate -FilePath .\Jare-Li.cer -CertStoreLocation Cert:\CurrentUser\Root
Import-Certificate -FilePath .\Jare-Li.cer -CertStoreLocation Cert:\CurrentUser\TrustedPublisher
```

公钥证书位于源码目录 `build\certs\Jare-Li.cer`（发布包中为 `Jare-Li.cer`）。
导入后 `Get-AuthenticodeSignature` 会变成 `Valid`，但 SmartScreen 仍可能提示，直到积累信誉。

不要把来源不明的自签名根证书导入系统。

## 对外分发

- 使用受信任的 OV/EV 代码签名证书；
- 或使用 Microsoft Store 分发；
- 企业环境使用 AppLocker / WDAC 允许规则；
- 不建议全局关闭 SmartScreen。
