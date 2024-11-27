#pragma once
#include <stdint.h>
#include <fs/lxfs.h>

#define __CMD_FUNC(func) int func(const char* command, size_t argc, uint16_t* tokens) // func(command, argc, tokens)
#define __CMD_NOAR_FUNC(func) int func() // no arguments

// commands
__CMD_NOAR_FUNC(cmd_clear);
__CMD_FUNC(cmd_vol);
__CMD_FUNC(cmd_part);
__CMD_FUNC(cmd_fs);
__CMD_FUNC(cmd_ls);
__CMD_FUNC(cmd_cd);
__CMD_FUNC(cmd_mkdir);
__CMD_FUNC(cmd_rmdir);
__CMD_FUNC(cmd_rm);
__CMD_FUNC(cmd_finfo);
__CMD_FUNC(cmd_led);
__CMD_FUNC(cmd_run);
__CMD_FUNC(cmd_shut);
__CMD_FUNC(cmd_runa);
__CMD_FUNC(cmd_fsz);

