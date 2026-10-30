# v1.7.7-JGg 更新说明

> 程序版本：`1.7.7.0 (Jare-Li Refactor)`  
> 基线：`imengyu/JiYuTrainer 1.7.6`（MIT License）  
> 发布标签：`v1.7.7-JGg`  
> 发布包：`JiYuTrainer-JareLi-1.7.7-JGg.zip`

## 本次更新

### 1. 关闭 / 最小化移到右上角

- 关闭 `×`、最小化 `−` 不再放在左上角
- 现在位于标题栏右上角
- 标题、图标从左侧开始，不再被窗口按钮挤压

### 2. 主窗口可移动

- 无边框主窗口增加标题栏拖动
- 按住标题栏空白处、图标或标题即可移动窗口
- 右上角 80px 保留给关闭/最小化，不会误拖
- 同时保留原生 `WM_NCHITTEST` 处理，拖动更稳定

### 3. UDP 界面完全重做

- 放弃之前容易错位的绝对定位/flex 混用布局
- 改为稳定的 **表格双栏布局**：左栏目标与结果，右栏消息与命令
- 所有输入框、按钮改为整行宽度，不再出现按钮压住输入框
- 目标卡、结果卡、消息卡、CMD 卡、远程操作卡重新设计
- 去掉了底部重复提示卡，窗口更干净
- 保留原配色：`#f4f7fb` 背景、白色卡片、`#2ea4f0` 强调色
- UDP 操作按钮全部改为原生 `<button>`，确保 C++ `BUTTON_CLICK` 能收到事件
- 窗口按 DPI 缩放：100% / 125% / 150% 下保持可读

### 4. IP 扫描

- 扫描窗口打开后自动开始扫描（450ms 后），不依赖按钮点击
- “扫描局域网主机”改为全宽按钮，点击后打开扫描窗口
- 保留 `isScanning` 防重复
- 保留扫描范围、`SendARP`、线程计数修复
- 扫描结果实时加入列表，点击 IP 可复制

日志位于 exe 同目录的 `JiYuTrainer.log`，关键标记：

```text
[UDP] open scan window
[ScanIP] scan link clicked, scanning=0
[ScanIP] start 192.168.x.0 - 192.168.x.255
[ScanIP] range 192.168.x.0-255, count=256
[ScanIP] finish, hosts=N
```

## 从旧版升级

1. 从托盘退出旧版 JiYuTrainer（不要只关闭窗口）
2. 备份旧目录中的 `JiYuTrainer.ini`（可选，保留设置）
3. 下载 `JiYuTrainer-JareLi-1.7.7-JGg.zip`
4. 解压到新目录，或覆盖旧目录
5. 如果覆盖，保留原来的 `JiYuTrainer.ini`
6. 运行 `JiYuTrainer.exe`，首次运行建议管理员权限
7. 如果出现 SmartScreen，按下方说明处理

> 不要同时运行旧版和新版；两边的 Hook/驱动会冲突。

## 校验

```powershell
Get-FileHash .\JiYuTrainer-JareLi-1.7.7-JGg.zip -Algorithm SHA256
Get-FileHash .\JiYuTrainer.exe -Algorithm SHA256
```

```text
JiYuTrainer.exe                  6BE86CE82AD8A09416C93F4BD036649E640EEB9C4E07210AEAE2FBD8BCCE8160
JiYuTrainer-JareLi-1.7.7-JGg.zip D7A52E19BEEEE39D4414489472485E269701AF2EB8A1940FECDF94C56A9B63C3
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

程序内置的“检查更新”仍指向原项目更新服务，不会自动发现 GitHub Release `v1.7.7-JGg`。  
本次更新需要手动下载 zip 覆盖。

## 已知限制

- 本版本仍使用自签名证书，不能消除 SmartScreen 首次提示
- UDP 重放模块仅限自己拥有或已获授权的设备
- 驱动 `JiYuTrainerDriver.sys` 保持上游原文件
- 自动测试 74/74 通过，但不代表所有机房环境 100% 兼容
