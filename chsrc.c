/* --------------------------------------------------------------
* File          : chsrc.c
* Authors       : Aoran Zeng <ccmywish@qq.com>
* Created on    : <2023-08-28>
* Last modified : <2023-08-30>
*
* chsrc:
*
*   Change Source —— 换源命令行工具
* -------------------------------------------------------------*/

#include "chsrc.h"

#define Chsrc_Version "v0.1.0"


/* 检测二进制命令是否存在
 */
bool
does_the_command_exist (char* check_cmd)
{
  char* which = check_cmd;

  int ret = system(which);

  char buf[32] = {0};
  sprintf(buf, "Error code: %d", ret);

  if (0!=ret) {
    xy_warn (xy_strjoin("Not exist with ", buf));
    return false;
  } else {
    return true;
  }
}


/**
 * Python换源
 *
 * 参考：https://mirrors.tuna.tsinghua.edu.cn/help/pypi/
 */
void
pl_python_chsrc (char* option)
{
  int selected = 0;

  for (int i=0;i<sizeof(pl_ruby_sources);i++) {
    // 循环测速
  }

  const char* source_name = pl_ruby_sources[selected].mirror->name;
  const char* source_abbr = pl_ruby_sources[selected].mirror->abbr;
  const char* source_url  = pl_ruby_sources[selected].url;

  xy_info (xy_strjoin("chsrc: 选中镜像站：", source_abbr));

  char* cmd = xy_strjoin("pip config set global.index-url ", source_url);
  system(cmd);
  free(cmd);

  xy_success(xy_strjoin("chsrc: 感谢镜像提供方：", source_name));
}


/**
 * Ruby换源
 *
 * 参考：https://gitee.com/RubyKids/rbenv-cn
 */
void
pl_ruby_chsrc (char* option)
{
  int selected = 0;
  for (int i=0;i<sizeof(pl_ruby_sources);i++) {
    // 循环测速
  }

  const char* source_name = pl_ruby_sources[selected].mirror->name;
  const char* source_abbr = pl_ruby_sources[selected].mirror->abbr;
  const char* source_url  = pl_ruby_sources[selected].url;

  xy_info (xy_strjoin("chsrc: 选中镜像站：", source_abbr));

  xy_info("chsrc: 为 gem 命令换源");
  system("gem source -r https://rubygems.org/");

  char* cmd = xy_strjoin("gem source -a ", source_url);
  system(cmd);
  free(cmd);

  cmd = xy_strjoin("bundle config 'mirror.https://rubygems.org' ", source_url);
  xy_info("chsrc: 为 bundler 命令换源");
  system(cmd);
  free(cmd);

  xy_success(xy_strjoin("chsrc: 感谢镜像提供方：", source_name));
}

#define chsrcfunc(func) (const char const*)func
static const char const
*pl_ruby[]   = {"gem",  "ruby",    "rb",       NULL,  chsrcfunc(pl_ruby_chsrc)},
*pl_python[] = {"pip",  "python",  "py",       NULL,  chsrcfunc(pl_python_chsrc)},
*pl_nodejs[] = {"npm",  "node",    "nodejs",  "js",     NULL},
*pl_perl[]   = {"perl", "cpan",     NULL},
*pl_php[]    = {"php",  "composer", NULL},
*pl_cran[]   = {"r",    "cran",     NULL},
*pl_rust[]   = {"rust", "cargo",   "crate",   "crates",  NULL},
*pl_go[]     = {"go",   "golang",  "goproxy",  NULL},
*pl_dotnet[] = {"nuget","net",     "dotnet",  ".net",    NULL},
*pl_maven[]  = {"maven", NULL},
*pl_gradle[] = {"gradel",NULL},
*pl_julia[]  = {"julia", NULL},
// Java暂时需要直接指定包管理器
// pl_java
**pl_packagers[] = {
  pl_ruby, pl_python, pl_nodejs, pl_perl,  pl_php,    pl_cran,
  pl_rust, pl_go,     pl_dotnet, pl_maven, pl_gradle, pl_julia
};
#undef chsrcfunc

static const char const*
usage[] = {
  "chsrc: Change Source " Chsrc_Version " by RubyMetric\n",

  "维护:  https://gitee.com/RubyMetric/chsrc\n",

  "使用：\n"
  "  chsrc  <要换源的对象>  [所换源名称]\n",

  "选项：\n"
  "  -h, --help           打印该帮助\n",

  "支持：\n"
};


void
call_cmd (void* cmdptr, const char* arg)
{
  void (*cmd_func)(const char*) = cmdptr;
  if (NULL==arg) {
    xy_info("chsrc: 将使用默认镜像");
  }
  cmd_func(arg);
}


int
print_help ()
{
  for (int i=0; i<Array_Size(usage); i++) {
    xy_info(usage[i]);
  }
}


int
main (int argc, char const *argv[])
{
  xy_useutf8();

  // 未提供参数时
  if (argc<=1) {
    print_help(); return 0;
  }

  // 第一个参数
  const char* target = NULL;
  if (0==strcmp("-h",argv[1]) || 0==strcmp("--help",argv[1])) {
    print_help(); return 0;
  } else {
    target = argv[1];
  }

  const char* option = NULL;
  const char* cmdarg = NULL;
  // 第二个参数
  if (argc>=3)
  {
    // printf ("argc = %d\n", argc);
    if (argv[2][0]=='-') {
      option = argv[2];
    } else {
      cmdarg = argv[2];
    }
  }

  int matched = 0;

  for (int i=0; i<Array_Size(pl_packagers); i++) {
    const char const** packager = pl_packagers[i];
    int k = 0;
    const char* alias = packager[k];
    while (NULL!=alias) {
      if (0==strcmp(target, alias)) {
        // printf("matched: %s\n", alias);
        matched = 1; break;
      }
      k++;
      alias = packager[k];
    }
    if (matched) {
      do {
        k++; alias = packager[k];
      } while (NULL!=alias);
      call_cmd ((void*) packager[k+1], cmdarg);
    }
  }

  if (!matched) {
    xy_info("chsrc: 暂不支持的换源类型，请使用-h查看可换源");
  }
  return 0;
}
