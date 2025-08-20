//
// Created by Heng on 25-6-20.
//

#ifndef MACROS_H
#define MACROS_H
#define MAXNAME 25  /* mfd名、ufd名、文件名的最大长度 */
#define MAXCHILD 50 /* 子项（子目录或子文件）的最大数量 */
#define MAX (MAXCHILD*MAXCHILD) /* 文件物理地址数组的大小 */

// 文件分享权限定义
#define SHARE_READ 0
#define SHARE_WRITE 1
#define SHARE_READWRITE 2
#endif //MACROS_H
