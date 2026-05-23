#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <sys/stat.h>

#define FFMPEG "D:\\迅雷下载\\ffmpeg-2026-05-21-git-0857141823-full_build\\ffmpeg-2026-05-21-git-0857141823-full_build\\bin\\ffmpeg.exe"
#define BASE_DIR "D:\\迅雷下载"
#define OUTPUT_DIR "D:\\迅雷下载\\mp4_output"

int main(void) {
    // システムロケールを UTF-8 に設定（日本語パス対応）
    SetConsoleOutputCP(CP_UTF8);

    // 出力フォルダ作成
    CreateDirectoryA(OUTPUT_DIR, NULL);

    // *.m3u8 フォルダを検索
    char searchPath[MAX_PATH];
    snprintf(searchPath, sizeof(searchPath), "%s\\*.m3u8", BASE_DIR);

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(searchPath, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        printf("フォルダが見つかりません\n");
        return 1;
    }

    do {
        // フォルダのみ対象
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) continue;
        if (strcmp(fd.cFileName, ".") == 0 || strcmp(fd.cFileName, "..") == 0) continue;

        // index.m3u8 のサイズチェック（0 バイトはスキップ）
        char m3u8Path[MAX_PATH];
        snprintf(m3u8Path, sizeof(m3u8Path), "%s\\%s\\index.m3u8", BASE_DIR, fd.cFileName);
        struct _stat st;
        if (_stat(m3u8Path, &st) != 0 || st.st_size == 0) {
            printf("スキップ (未完了): %s\n", fd.cFileName);
            continue;
        }

        // 出力ファイル名生成 (.m3u8 → .mp4)
        char outName[MAX_PATH];
        strncpy(outName, fd.cFileName, sizeof(outName));
        char *ext = strstr(outName, ".m3u8");
        if (ext) strcpy(ext, ".mp4");

        // ffmpeg コマンド組み立て
        char cmd[2048];
        snprintf(cmd, sizeof(cmd),
            "cd /d \"%s\\%s\" && \"%s\" -y -allowed_extensions ALL -i \"index.m3u8\" -c copy \"%s\\%s\"",
            BASE_DIR, fd.cFileName, FFMPEG, OUTPUT_DIR, outName);

        printf("\n変換中: %s\n", fd.cFileName);
        int ret = system(cmd);
        printf("結果: %s (exit=%d)\n", outName, ret);

    } while (FindNextFileA(h, &fd));

    FindClose(h);
    printf("\n全変換完了\n");
    return 0;
}