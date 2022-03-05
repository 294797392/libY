#include "Yfirstinclude.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Ynet.h"
#include "Yconv.h"
#include "Ytcp.h"

int Y_tcp_receive_packet(Ysocket fd, Ypacket *packet)
{
    char head[32] = {'\0'};
    if(Y_read_socket(fd, head, sizeof(head)) != sizeof(head))
    {
        return -1;
    }

    int seq = Y_conv_bytes2int(head, 0);
    int cmd = Y_conv_bytes2int(head, 4);
    int size = Y_conv_bytes2int(head, 8);
    int code = Y_conv_bytes2int(head, 12);

    // fixme:接收缓冲区管理的逻辑先写的简单点，后续需要写一个缓冲池来对内存进行复用
    if(packet->data != NULL)
    {
        free(packet->data);
    }

    packet->seq = seq;
    packet->cmd = cmd;
    packet->code = code;
    packet->size = size;

    if(size > 0)
    {
        packet->data = (char*)malloc(size);

        // 收数据
        if(Y_read_socket(fd, packet->data, packet->size) != size)
        {
            return -1;
        }
    }

    return 0;
}

int Y_tcp_send_packet(Ysocket fd, int seq, int cmd, int code, char *data, int datasize)
{
    char head[32] = {'\0'};

    // 打包数据包头信息
    Y_conv_int2bytes(seq, head, 0);
    Y_conv_int2bytes(cmd, head, 4);
    Y_conv_int2bytes(datasize, head, 8);
    Y_conv_int2bytes(code, head, 12);
    if(Y_write_socket(fd, head, sizeof(head)) < 0)
    {
        return -1;
    }

    // 打包数据
    if(data != NULL && datasize > 0)
    {
        if(Y_write_socket(fd, data, datasize) < 0)
        {
            return -1;
        }
    }

    return 0;
}
