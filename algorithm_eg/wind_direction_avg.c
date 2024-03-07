#include <stdio.h>
#include <math.h> // 包含 sin 和 cos 函数

int main()
{
    int nums[10]; // 声明一个数组，用于存储输入的风向值（单位为度）
    double sum_sin = 0; // 初始化 sin 值和为 0
    double sum_cos = 0; // 初始化 cos 值和为 0
    int i; // 循环变量
    double average_x, average_y; // 声明两个变量，用于存储平均值的 x 和 y 分量
    double average_dir; // 声明一个变量，用于存储平均风向

    printf("Enter 10 wind directions: ");
    for (i = 0; i < 10; i++)
    {
        scanf("%d", &nums[i]); // 读入风向值
        sum_sin += sin(nums[i] * M_PI / 180); // 累加 sin 值
        sum_cos += cos(nums[i] * M_PI / 180); // 累加 cos 值
    }

    average_x = sum_cos / 10; // 计算平均值的 x 分量
    average_y = sum_sin / 10; // 计算平均值的 y 分量
    average_dir = atan2(average_y, average_x) * 180 / M_PI; // 计算平均风向

    //atan2输出值为-pi到pi 那么average_dir的值(角度)为 -180 到180
    //将风向值(角度)从-180至180修正到熟悉的0至360
    if ( average_dir < 0 ) average_dir += 360; 

    printf("Average wind direction: %f\n", average_dir); // 输出平均风向

    return 0;
}

//
//这个程序首先读入 10 个风向值，然后累加它们的 sin 值和和 cos 值和，最后计算平均风向并输出。
//注意：这里假设风向值是以东为 0 度，顺时针方向为正的。如果你使用的是其他参考系，可能需要进
//
