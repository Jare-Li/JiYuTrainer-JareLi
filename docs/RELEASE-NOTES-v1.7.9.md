# JiYu Trainer 1.7.9 更新文档

> 程序版本：`1.7.9.0 (Jare-Li Refactor)`  
> 基线版本：`imengyu/JiYuTrainer 1.7.6`（MIT License）  
> 发布标签：`v1.7.9`  
> 发布包：`JiYuTrainer-JareLi-1.7.9.zip`

## 一、本次更新内容

### 1. 关闭 UDP 不再退出主程序

- 关闭 UDP 窗口改为**隐藏**，不再销毁窗口
- 主界面、托盘、Hook/驱动状态继续运行
- 再次点击主界面的 **“UDP 重放”** 即可重新打开
- 如果扫描窗口还开着，会和 UDP 窗口一起隐藏
- 修复了关闭 UDP 后再次打开可能使用失效窗口指针的问题

### 2. 关闭 / 最小化移到最右上角

- 关闭 `×`、最小化 `−` 固定在窗口**最右上角**
- 控件移出圆角 `window-box`，不会再被圆角裁切
- 标题、图标仍在左上侧，不再和窗口按钮抢位置

### 3. 主窗口可移动

- 无边框主窗口支持标题栏拖动
- 按住标题、图标或标题栏空白处均可移动
- 右上角 80px 保留给关闭/最小化，不会误拖
- 同时保留原生 `WM_NCHITTEST` 处理

### 4. UDP 主界面重做

- 放弃容易错位的 flex / 绝对定位混用
- 改为稳定的表格双栏布局：
  - 左栏：目标 IP、端口、发送结果
  - 右栏：文字消息、CMD 命令、远程操作
- 输入框、按钮改为整行宽度，不再互相覆盖
- 操作按钮改为原生 `<button>`，确保 `BUTTON_CLICK` 生效
- 保留原配色：`#f4f7fb`、白色卡片、`#2ea4f0`
- 按 DPI 缩放，125% / 150% 下保持可读

### 5. 扫描窗口补完整

- 窗口改为 **640×760**
- 本机地址、开始地址、结束地址改为整行控件
- 增加扫描状态：
  - 正在扫描
  - 扫描完成，发现 N 台主机
- 结果列表和底部说明完整显示
- 打开扫描窗口后 450ms 自动开始扫描
- 保留 `isScanning` 防重复
- 保留扫描范围、`SendARP`、线程计数修复

### 6. 自动更新（C2 独立 helper）

- 检查 GitHub Releases 最新版本
- 下载 `JiYuTrainer-JareLi-1.7.9.zip`
- 下载 `JiYuTrainerUpdateHelper.exe`
- 下载 `SHA256SUMS.txt` 并校验
- 启动独立 helper（管理员权限）
- helper 等待主程序退出，备份旧文件，解压替换，失败回滚，最后重启

> 1.7.8 及更早版本没有 GitHub 更新逻辑，需要手动安装一次 1.7.9；之后可从 1.7.9 自动更新到更新版本。

## 二、升级方法

1. 从托盘退出旧版 JiYuTrainer
2. 备份旧目录中的 `JiYuTrainer.ini`（可选）
3. 下载 `JiYuTrainer-JareLi-1.7.9.zip`
4. 解压到新目录，或覆盖旧目录
5. 覆盖时保留原来的 `JiYuTrainer.ini`
6. 运行 `JiYuTrainer.exe`，首次运行建议管理员权限
7. 出现 SmartScreen 时按下方说明处理

> 不要同时运行旧版和新版，Hook/驱动会冲突。

## 三、SHA-256

```text
JiYuTrainer.exe                  E869E6CB93D643CD218E99C37CC7CD2A19E235A0BC1874A6764B4E49A581A052
JiYuTrainer-JareLi-1.7.9.zip     8305CAF7368016C4326A8462145245DF911B379C2C87A09F9E0F652468ED9AE7
JiYuTrainerUpdateHelper.exe      3905954CD44BC2BE687F9E9829821A9944251062EA3DA4A68D63428676969901
```

校验命令：

```powershell
Get-FileHash .\JiYuTrainer-JareLi-1.7.9.zip -Algorithm SHA256
Get-FileHash .\JiYuTrainer.exe -Algorithm SHA256
```

哈希不一致时不要运行，请重新下载。

## 四、SmartScreen 说明

首次运行可能提示“Windows 已保护你的电脑”。这是自签名证书 `CN=Jare-Li` 没有 SmartScreen 信誉导致的，不是病毒报毒。

1. 点击 **更多信息** → **仍要运行**
2. 如果没有该按钮：
   - 右键 zip → 属性 → 通用 → 勾选 **解除锁定** → 确定
   - 或对解压后的 exe 执行：

```powershell
Unblock-File -Path .\JiYuTrainer.exe
```

3. `Get-AuthenticodeSignature` 显示 `UnknownError` / `UntrustedRoot` 属于自签名正常状态

## 五、测试结果

```text
74/74 passed, 0 failed, 100.0%
```

覆盖：

- IP 校验、扫描范围、目标列表解析
- UDP 包字节、偏移、UTF-16LE、超长载荷
- PE 架构、版本信息、HTML 资源
- 主界面 DOM id、UDP/扫描窗口布局
- MIT 归属、签名主体、构建产物

## 六、已知限制

- 仍使用自签名证书，不能消除 SmartScreen 首次提示
- UDP 重放模块仅限自己拥有或已获授权的设备
- 驱动 `JiYuTrainerDriver.sys` 保持上游原文件
- 自动测试 74/74 不代表所有机房环境 100% 兼容
- 1.7.8 及更早版本需要手动安装一次 1.7.9；1.7.9 起才会自动发现后续 GitHub Release

## 七、发布信息

```text
Tag: v1.7.9
Title: JiYuTrainer 1.7.9 — Jare-Li Refactor
Assets:
  JiYuTrainer-JareLi-1.7.9.zip
  JiYuTrainerUpdateHelper.exe
  SHA256SUMS.txt
```

`v1.7.7-JGg` 已发布，保留旧 Release；本版使用新 tag `v1.7.9`。
