# cmd_base64 

> 功能: base64命令行工具

## 主要目录和文件说明

```
.    
├── bin/
│   └── base64         #make all 命令生成的可用于命令行base64工具
├── cmd_src/           #命令行工具的源码
├── doc/               #其他说明文件
├── Makefile           #make file 文件
└── src/               #base64接口源码
    ├── libbase64.h
    └── libbase64.c    
```


## 命令行工具base64使用说明

### 使用方法

```shell
[10:40:27@fusky bin]$ls
base64*
[10:40:28@fusky bin]$./base64 -h
Usage: ./base64 [OPTION]... [FILE]
If no FILE is given or is specified as '-', read from standard input.
Options:
  -d, --decode     Decode a base64 stream.
  -h, --help       Print this help text.
  -w, --wrap=COLS  Wrap encoded lines at this column. Default 0, 0 to disable.
```

### 使用举例

#### 1.加密码字符串

```shell
[11:06:33@fusky bin]$echo -n "hello"|./base64 
aGVsbG8=[11:06:46@fusky bin]$
```

#### 2.解密码字符串

```shell
[11:08:28@fusky bin]$echo -n "aGVsbG8="|./base64 -d
hello[11:08:48@fusky bin]$
```


#### 3.加密文件内容到新文件

```shell
[10:48:16@fusky bin]$cat f.txt 
hello
[10:48:20@fusky bin]$./base64 f.txt  > new.txt
[10:48:44@fusky bin]$cat new.txt 
aGVsbG8K[10:48:48@fusky bin]$
```

#### 4.解密文件内容到新文件

```shell
[10:50:24@fusky bin]$./base64 -d new.txt  >new2.txt
[10:50:46@fusky bin]$cat new2.txt
hello

```
