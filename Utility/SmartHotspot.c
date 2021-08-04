#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

int main() {
    int countdown = 5 * 60;
    const char cmd[][256] = {
        "powershell -Command",
        "$profile = [Windows.Networking.Connectivity.NetworkInformation,Windows.Networking.Connectivity,ContentType=WindowsRuntime]::GetInternetConnectionProfile();",
        "$wifi = [Windows.Networking.NetworkOperators.NetworkOperatorTetheringManager,Windows.Networking.NetworkOperators,ContentType=WindowsRuntime]::CreateFromConnectionProfile($profile);"
    };
    char content[64] = {0};
    char cmd_shutdown[64] = {0};
    char cmd_wifi_on[512] = {0};
    char cmd_wifi_stat[512] = {0};
    FILE *pp = NULL;
    sprintf(cmd_shutdown, "%s %d > nul", "shutdown -s -t", countdown);
    sprintf(cmd_wifi_on, "%s \"%s %s %s\" > nul", cmd[0], cmd[1], cmd[2], "$wifi.StartTetheringAsync();");
    sprintf(cmd_wifi_stat, "%s \"%s %s %s\"", cmd[0], cmd[1], cmd[2], "Write-Host($wifi.TetheringOperationalState);");
    printf("Turning on hotspot...\n");
    system(cmd_wifi_on);
    while (1) {
        pp = popen(cmd_wifi_stat, "r");
        fread(content, sizeof(char), 2, pp);
        pclose(pp);
        if (!strcmp(content, "On")) break;
        Sleep(50);
    }
    printf("Hotspot is on.\n");
    while (1) {
        pp = popen(cmd_wifi_stat, "r");
        fread(content, sizeof(char), 3, pp);
        pclose(pp);
        if (!strcmp(content, "Off")) {
            system(cmd_shutdown);
            printf(
                "\n\nThe computer has been set to automatically shut down in %d min. "
                "\nIf you need to stop the automatic shutdown, please enter \"shutdown -a\" in cmd.\n",
                countdown / 60
            );
            system("PAUSE > nul");
            exit(0);
        } else {
            printf("\rDetecting: hotspot is on...");
            Sleep(1000);
        }
    }
}
