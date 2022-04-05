#include "packethandle.h"
#include<QDebug>
/**************************************************
 *
 * ProductName:     Teach Pendant v0.3
 * Manufacturer:    BOZHON
 * Author:          JYL
 * Date:            2018.01.24
 *
 * ************************************************/

/**********************************************
 * 线程1: socket发送接收数据, 包括心跳检测
 * 线程2: 处理接收数据队列, 解析接收到的socket数据帧
 * 线程3: 处理发送数据队列, 封装待发送的socket数据帧
 * ********************************************/
