//
// Created by Heng on 25-6-20.
//

#ifndef UTILS_H
#define UTILS_H
#include <string.h>
#include <time.h>
static inline char *rtrim(char *str)  /*remove the trailing blanks.*/
{
    int n=strlen(str)-1;
    while(n>=0)
    {
        if(*(str+n)!=' ')
        {
            *(str+n+1)='\0';
            break;
        }
        else n--;
    }
    if (n<0) str[0]='\0';
    return str;
}

static inline char *ltrim(char *str) /*remove the heading blanks.*/
{
    strrev(str);
    rtrim(str);
    strrev(str);
    return str;
}

// 将时间戳转换为可读格式
static char * FormatTime(time_t t) {
    static char buf[20];

    // 检查时间是否有效（大于一个合理的最小值）
    if (t < 315532800) { // 1980-01-01 00:00:00
        strcpy(buf, "Invalid Time");
        return buf;
    }

    struct tm *tm_info = localtime(&t);
    if (tm_info == NULL) {
        strcpy(buf, "Time Error");
        return buf;
    }

    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", tm_info);
    return buf;
}



// 将权限值转换为字符串
static const char* PermissionToString(int perm) {
    switch(perm) {
        case 0: return "Read";
        case 1: return "Write";
        case 2: return "Read/Write";
        default: return "Unknown";
    }
}
#endif //UTILS_H
