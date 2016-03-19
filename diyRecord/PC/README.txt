resort.c 双声道数据重组
    原录音文件为左右通道分别按块(DCT_LENGTH)存储
    该程序将每个采样点交叉存放，并将 big-endian 转成 little-endian

split.c 编码数据左右通道分离

