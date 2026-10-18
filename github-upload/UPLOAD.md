# GitHub 上传步骤

这个目录里的文件是**覆盖到项目根目录**用的。  
先做两件事：

1. 把 `@Jare-Li` 改成你的 GitHub 账号/团队。
2. 把 `README.md` 里的 `<your-name>` 改成你的 GitHub 用户名。

---

## 1. 合并到项目根目录

在 `F:\dshwrok\JiYuTrainer-1.7.6` 执行：

```powershell
Copy-Item .\github-upload\* . -Recurse -Force
Copy-Item .\github-upload\.github . -Recurse -Force
```

合并后根目录应该有：

```text
README.md
LICENSE
NOTICE
SECURITY.md
CONTRIBUTING.md
CODE_SIGNING.md
.gitignore
.github/workflows/build.yml
docs/FIRST-RUN.md
docs/UPSTREAM-README.md
docs/screenshots/*.png
```

> 如果不想覆盖当前 `README.md`，先备份：
> `Copy-Item README.md README.local.md`

---

## 2. 清理不要上传的文件

GitHub 仓库里不要放：

```text
build/
dist/
*.zip
*.pfx
*.cer
password.txt
*.log
*.ini
.vs/
*.user
*.aps
*.res
Release/JiYuTrainer.exe
Release/JiYuTrainerOldForWinXP.exe
```

保留这些构建输入：

```text
Release/sciter.zip
Release/JiYuTrainerDriver.sys
```

检查：

```powershell
git status --short
git ls-files | Select-String -Pattern "\.(pfx|p12|pvk|cer)$|password\.txt"
```

---

## 3. 创建 GitHub 仓库

打开 <https://github.com/new>：

- Repository name：`JiYuTrainer-JareLi`
- 选择 **Public**
- **不要**勾选 “Add a README file”
- **不要**勾选 “Add .gitignore” / “Choose a license”
- 点击 Create repository

---

## 4. 方式 A：Git 推送（推荐）

完整源码有几百个文件，网页上传很慢，推荐 Git。

```powershell
cd F:\dshwrok\JiYuTrainer-1.7.6

git add .
git commit -m "docs: prepare open-source publishing"

git remote add origin https://github.com/<你的用户名>/JiYuTrainer-JareLi.git
git branch -M main
git push -u origin main
```

如果已经存在 `origin`：

```powershell
git remote set-url origin https://github.com/<你的用户名>/JiYuTrainer-JareLi.git
git push -u origin main
```

---

## 5. 方式 B：网页上传

适合只补 README/许可证/工作流，不适合上传整个源码树。

1. 打开仓库首页 → **Add file** → **Upload files**
2. 上传：
   - `README.md`
   - `LICENSE`
   - `NOTICE`
   - `SECURITY.md`
   - `CONTRIBUTING.md`
   - `CODE_SIGNING.md`
   - `.gitignore`
   - `docs/` 整个文件夹
3. `.github/workflows/build.yml` 用 **Add file → Create new file**：
   - 文件名输入：`.github/workflows/build.yml`
   - 粘贴 `github-upload/.github/workflows/build.yml` 的内容
4. 提交：`Add open-source publishing files`

如果源码还没上传，用 GitHub Desktop 或 Git 推送整个项目；网页逐个拖拽几百个文件容易失败。

---

## 6. 检查 GitHub Actions

推送后打开仓库的 **Actions** 页：

- 应该出现 `windows-build`
- 构建命令：`.\build.ps1 -Sign`
- 测试命令：`python tests\run_tests.py`
- 成功后在 Artifacts 下载 `JiYuTrainer-JareLi`

如果失败，重点检查：

- `Release/sciter.zip` 是否已上传
- `Release/JiYuTrainerDriver.sys` 是否已上传
- `build.ps1` 是否在仓库根目录
- 是否误删了 `tests/`

---

## 7. 发布 Release

```powershell
git tag v1.7.6-jareli.1
git push origin v1.7.6-jareli.1
```

在 GitHub → **Releases → Draft a new release**：

- Tag：`v1.7.6-jareli.1`
- Title：`JiYuTrainer 1.7.6 — Jare-Li Refactor`
- 上传：`JiYuTrainer-JareLi-1.7.6.zip`
- 说明中写明 SHA-256、系统要求、SmartScreen 处理方式

---

## 8. 申请 SignPath 免费证书

1. 打开 <https://signpath.org/apply>
2. 填写：
   - 项目名：`JiYuTrainer Jare-Li Refactor`
   - 仓库：`https://github.com/<你的用户名>/JiYuTrainer-JareLi`
   - License：MIT
   - 构建命令：`.\build.ps1 -Sign`
   - 测试命令：`python tests\run_tests.py`
   - 发布包：`JiYuTrainer-JareLi-1.7.6.zip`
3. 确保 README 首页有 **Code signing policy** 链接
4. 通过后安装 SignPath GitHub App
5. 在 GitHub Secrets 添加 `SIGNPATH_API_TOKEN`
6. 按 <https://docs.signpath.io/trusted-build-systems/github> 增加签名步骤

**现实提醒：**

- SignPath 条款明确排除 hacking tools / 绕过安全措施的软件；
- UDP 重放、反控制、Hook/驱动都可能导致申请被拒；
- 即使通过，证书发布者是 **SignPath Foundation**，不是 Jare-Li；
- 每次 Release 需要人工审批。

如果目标是“免费证书”，更现实的路线是拆出不含攻击模块的公开版本；完整版继续自签名。

---

## 9. 常见问题

### GitHub 显示的还是旧 README？

确认根目录 `README.md` 已被覆盖，并且推送成功：

```powershell
git status
git log --oneline -3
```

### Actions 找不到 `sciter.zip`？

确认 `Release/sciter.zip` 没有被 `.gitignore` 忽略：

```powershell
git check-ignore -v Release/sciter.zip
git ls-files Release/sciter.zip
```

### `.github/workflows` 上传不了？

网页上传不支持隐藏目录时，用 **Add file → Create new file**，文件名直接输入完整路径 `.github/workflows/build.yml`。

### 私钥会被上传吗？

`.gitignore` 已忽略 `*.pfx`、`*.cer`、`build/`、`password.txt`。  
仍然建议上传前执行一次：

```powershell
git ls-files | Select-String -Pattern "\.(pfx|p12|pvk|cer)$|password\.txt"
```
