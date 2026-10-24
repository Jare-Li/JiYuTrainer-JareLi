# v1.7.7 更新说明

> 版本：`1.7.7.0 (Jare-Li Refactor)`  
> 基线：`imengyu/JiYuTrainer 1.7.6`（MIT License）  
> 发布标签：`v1.7.7`  
> 发布包：`JiYuTrainer-JareLi-1.7.7.zip`

## 本次修复

### 1. 关闭 / 最小化按钮位置

- 关闭 `×`、最小化 `−` 移到窗口**左上角**
- 顺序：关闭在左，最小化在右
- 不再与标题、状态卡片争抢右上角空间

### 2. UDP 重放窗口

- 窗口尺寸保持放大后的 `896×760`
- 屏幕高度不足时自动缩小到 `screenHeight - 80`，避免被任务栏裁掉
- 内容区域支持纵向滚动
- IP 输入框与“扫描局域网”按钮不再重叠

### 3. IP 扫描

- “开始扫描主机 IP”同时支持 `HYPERLINK_CLICK` 和 `BUTTON_CLICK`
- 增加 `isScanning` 防重复点击
- 保留并确认以下修复：
  - 扫描范围尾部地址不再丢失（256 个地址完整覆盖）
  - `SendARP` 失败不再误判为在线主机
  - 修复扫描线程计数竞态
  - 起始地址不能大于结束地址
- 增加日志，便于定位：

```text
[UDP] open scan window
[ScanIP] scan link clicked, scanning=0
[ScanIP] start 192.168.x.0 - 192.168.x.255
[ScanIP] range 192.168.x.0-255, count=256
[ScanIP] finish, hosts=N
```

日志位于 exe 同目录的 `JiYuTrainer.log`。

## 从旧版升级

1. 从托盘退出旧版 JiYuTrainer（不要只关闭窗口）
2. 备份旧目录中的 `JiYuTrainer.ini`（可选，保留设置）
3. 下载 `JiYuTrainer-JareLi-1.7.7.zip`
4. 解压到新目录，或覆盖旧目录
5. 如果覆盖，保留原来的 `JiYuTrainer.ini`
6. 运行 `JiYuTrainer.exe`，首次运行建议管理员权限
7. 如果出现 SmartScreen，按下方说明处理

> 不要同时运行旧版和新版；两边的 Hook/驱动会冲突。

## 校验

```powershell
Get-FileHash .\JiYuTrainer-JareLi-1.7.7.zip -Algorithm SHA256
Get-FileHash .\JiYuTrainer.exe -Algorithm SHA256
```

```text
JiYuTrainer.exe            C0A45E345FA6664EC4505CFB9F860CFB672E5B879CFAC4CC0C97CBDA4C94D4F1
JiYuTrainer-JareLi zip     5476AEC454068DACF78904F5DF40263CDA1C79ED304E79B03C2DAED34E012243
```

如果哈希不一致，不要运行，请重新下载。

## SmartScreen

首次运行可能提示“Windows 已保护你的电脑”。这是自签名证书 `CN=Jare-Li` 没有 SmartScreen 信誉导致的，不是病毒报毒。

1. 点击 **更多信息** → **仍要运行**
2. 如果没有该按钮：
   - 右键 zip → 属性 → 通用 → 勾选 **解除锁定** → 确定
   - 或被解压后的 exe 执行 `Unblock-File -Path .\JiYuTrainer.exe`
3. `Get-AuthenticodeSignature` 显示 `UnknownError` / `UntrustedRoot` 属于自签名正常状态

详见 [FIRST-RUN.md](FIRST-RUN.md) 与 [CODE_SIGNING.md](../CODE_SIGNING.md)。

## 内置更新说明

程序内置的“检查更新”仍指向原项目更新服务，不会自动发现 GitHub Release `v1.7.7`。  
本次更新需要手动下载 zip 覆盖。

## 已知限制

- 本版本仍使用自签名证书，不能消除 SmartScreen 首次提示
- UDP 重放模块仅限自己拥有或已获授权的设备
- 驱动 `JiYuTrainerDriver.sys` 保持上游原文件
- 自动测试 74/74 通过，但不代表所有机房环境 100% 兼容
