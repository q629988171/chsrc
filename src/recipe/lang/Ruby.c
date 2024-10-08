/** ------------------------------------------------------------
 * SPDX-License-Identifier: GPL-3.0-or-later
 * -------------------------------------------------------------
 * File Authors  : Aoran Zeng <ccmywish@qq.com>
 * Contributors  :  Nil Null  <nil@null.org>
 * Created On    : <2023-08-29>
 * Last Modified : <2024-08-17>
 * ------------------------------------------------------------*/

static MirrorSite
RubyChina = {"rubychina",    "RubyChina",    "Ruby China 社区",    "https://gems.ruby-china.com/",
             "https://gems.ruby-china.com/rubygems/gems/nokogiri-1.15.0-java.gem"}; // 9.9 MB

/**
 * @time 2024-05-25 更新
 * @note {
 *   下面的源，并非都实现正确，
 *   BFSU 和 Tuna 以及 阿里的镜像都有问题，会循环遍历一个gem的所有版本，导致安装时间相当长
 *
 *   若实现正确:
 *     1. 目前北外最快最稳定
 *     2. 腾讯挺快，个别时候不稳定，但稳定时能达到4.5MB甚至以上
 *     3. tuna 有时快有时慢，不稳定，一般在3MB以下
 *     4. rubychina 有时快有时慢，快时一般在 2MB以下，慢时一般在1MB以下
 *     5. 阿里云最不稳定，经常最慢
 * }
 */
static SourceInfo
pl_ruby_sources[] = {
  {&Upstream,  "https://rubygems.org"},
  {&Huawei,    "https://mirrors.huaweicloud.com/repository/rubygems/"},
  {&Tencent,   "https://mirrors.tencent.com/rubygems/"},
  {&RubyChina, "https://gems.ruby-china.com/"}

  // {&Bfsu,      "https://mirrors.bfsu.edu.cn/rubygems/"},
  // {&Tuna,      "https://mirrors.tuna.tsinghua.edu.cn/rubygems/"},
  // {&Ali,       "https://mirrors.aliyun.com/rubygems/"},
};
def_sources_n(pl_ruby);


void
pl_ruby_getsrc (char *option)
{
  chsrc_run ("gem sources", RunOpt_Default);
  chsrc_run ("bundle config get mirror.https://rubygems.org", RunOpt_Default);
}

void
pl_ruby_remove_gem_source (const char *source)
{
  char *cmd = NULL;
  if (is_url (source))
    {
      cmd = xy_str_delete_suffix (source, "\n");
      cmd = xy_2strjoin ("gem sources -r ", cmd);
      chsrc_run (cmd, RunOpt_Default);
    }
}

/**
 * Ruby换源，参考：https://gitee.com/RubyMetric/rbenv-cn
 */
void
pl_ruby_setsrc (char *option)
{
  char *chsrc_type = xy_streql (option, ChsrcTypeReset) ? ChsrcTypeReset : ChsrcTypeAuto;

  chsrc_ensure_program ("gem");

  chsrc_yield_source_and_confirm (pl_ruby);

  char *cmd = NULL;

  xy_run ("gem sources -l", 0, pl_ruby_remove_gem_source);

  cmd = xy_2strjoin ("gem source -a ", source.url);
  chsrc_run (cmd, RunOpt_Default);


  chsrc_ensure_program ("bundle");

  char *where = " --global ";
  if (CliOpt_Locally==true)
    {
      where = " --local ";
    }

  cmd = xy_strjoin (4, "bundle config", where, "'mirror.https://rubygems.org' ", source.url);
  chsrc_run (cmd, RunOpt_No_Last_New_Line);

  chsrc_conclude (&source, chsrc_type);

  // chsrc_note2 ("Ruby的镜像源目前仅有 腾讯软件源，RubyChina 实现正确");
  // chsrc_note2 ("而其它如Tuna,Bfsu,Ali目前都实现的有问题，请勿使用");
}

void
pl_ruby_resetsrc (char *option)
{
  pl_ruby_setsrc (ChsrcTypeReset);
}


FeatInfo
pl_ruby_feat (char *option)
{
  FeatInfo fi = {0};

  fi.can_get = true;
  fi.can_reset = true;

  fi.stcan_locally = CanSemi;
  fi.locally = "gem 不支持; bundler 支持 (From v0.1.6)";
  fi.can_english = false;
  fi.can_user_define = true;

  fi.note = "华为开源镜像站提供的源可能存在问题";
  return fi;
}


def_target_gsrf(pl_ruby);
