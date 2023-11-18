#include "./shell.h"

void shell_cmd_hello_world(const struct shell *shell, size_t argc, char *argv[])
{
    int cnt = shell_strtol(argv[1], NULL, NULL);

    if (cnt == 0)
    {
        shell_fprintf(shell, SHELL_VT100_COLOR_CYAN, "WORLD (%d)\n", cnt);
    }
    else if (cnt > 100)
    {
        shell_fprintf(shell, SHELL_ERROR, "Cannot print >100 times (you entered %d)...\n", cnt);
    }
    else
    {
        for (int i = 0; i < cnt; i++)
        {
            shell_fprintf(shell, SHELL_VT100_COLOR_CYAN, "WORLD %d\n", i);
        }
    }
}

SHELL_CMD_REGISTER(hello, NULL, "A simple Hello World", shell_cmd_hello_world);