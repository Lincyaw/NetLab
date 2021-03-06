#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#define BUFFER_MAX 2048

int main(int argc, char *argv[])
{
    int fd;
    int proto;
    int str_len;
    char buffer[BUFFER_MAX];
    // 以太网头
    char *ethernet_head;
    char *ip_head;
    unsigned char *p;
    unsigned char *typeOfFrame;
    int Ht, Pt, Hs, Ps, Op;

    // Raw socket套接字用于在 MAC 层上收发原始数据帧，这样就允许用户在用户空间
    // 完成 MAC 之上各个层次的实现，进而可以对整个网络的数据进行监控和控制，给开发
    // 或测试带来了极大的便利性。
    if ((fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
    {
        printf("error create raw socket\n");
        return -1;
    }

    while (1)
    {
        printf("-----------------------------------\n");

        str_len = recvfrom(fd, buffer, 2048, 0, NULL, NULL);

        // 想一想，此处为什么要丢弃小于 42 字节的包？
        if (str_len < 42)
        {
            printf("error when recv msg \n");
            return -1;
        }

        ethernet_head = buffer;
        p = ethernet_head;
        printf("Dst MAC address: %.2x:%02x:%02x:%02x:%02x:%02x\n",
               p[0], p[1], p[2], p[3], p[4], p[5]);
        printf("Src MAC address: %.2x:%02x:%02x:%02x:%02x:%02x\n",
               p[6], p[7], p[8], p[9], p[10], p[11]);
        
        typeOfFrame = ethernet_head + 12;
        printf("type of frame:\t\t\t\t%.2x%02x\n", typeOfFrame[0], typeOfFrame[1]);
        int temp = ((int)typeOfFrame[0] << 8) + (int)(typeOfFrame[1]);
        switch (temp)
        {
        case 0x0800:
            ip_head = ethernet_head + 14;
            p = ip_head;
            // printf("%02x %02x %02x %02x %02x %02x %02x %02x , %02x %02x %02x %02x %02x %02x %02x %02x \n", p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8],
            //        p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
            // printf("%02x %02x %02x %02x %02x %02x %02x %02x , %02x %02x %02x %02x %02x %02x %02x %02x \n", p[16], p[17], p[18], p[19], p[20], p[21], p[22], p[23], p[24],
            //        p[25], p[26], p[27], p[28], p[29], p[30], p[31]);
            int v = p[0] >> 4;
            printf("Version:\t\t\t\t%d\n", v);

            int len = p[0] & 0xf;
            printf("Header length:\t\t\t\t%d\n", len);

            int DSF = p[1];
            printf("Differentiated Services Field:\t\t0x%x\n", DSF);

            int totalLen = ((int)p[2] << 8) + (int)p[3];
            printf("Total length:\t\t\t\t%d\n", totalLen);

            int identification = ((int)p[4] << 8) + (int)p[5];
            printf("Identification:\t\t\t\t0x%x\n", identification);

            int flags = (int)p[6] & 0xf0 >> 4;
            printf("Flags:\t\t\t\t\t0x%x\n", flags);

            int fragmentOffset = ((int)p[6] & 0xf << 8) + (int)p[7];
            printf("Fragment Offset:\t\t\t0x%x\n", fragmentOffset);

            int lifeTime = (int)p[8];
            printf("LifeTime:\t\t\t\t0x%x\n", lifeTime);

            proto = p[9];
            printf("Protocol:\t\t\t\t");
            switch (proto)
            {
            case IPPROTO_ICMP:
                printf("icmp\n");
                break;
            case IPPROTO_TCP:
                printf("tcp\n");
                break;
            case IPPROTO_UDP:
                printf("udp\n");
                break;
            default:
                break;
            }

            int headerCheckSum = ((int)p[10] << 8) + (int)p[11];
            printf("Header Check Sum:\t\t\t0x%x\n", headerCheckSum);

            printf("Src IP:\t\t\t\t\t%d.%d.%d.%d\n", p[12], p[13], p[14], p[15]);
            printf("Dst IP:\t\t\t\t\t%d.%d.%d.%d\n", p[16], p[17], p[18], p[19]);

            break;
        case 0x0806:
            ip_head = ethernet_head + 14;
            p = ip_head;

            Ht = ((int)p[0] << 8) + p[1];
            printf("Hardware type:      \t\t0x%x\n", Ht);

            Pt = ((int)p[2] << 8) + p[3];
            printf("Protocol type:      \t\t0x%x\n", Pt);

            Hs = (int)p[4];
            printf("Hardware size:      \t\t0x%x\n", Hs);

            Ps = (int)p[5];
            printf("Protocol size:      \t\t0x%x\n", Ps);

            Op = ((int)p[6] << 8) + p[7];
            printf("Opcode:             \t\t0x%x\n", Op);

            printf("Sender Mac address: \t\t%.2x:%02x:%02x:%02x:%02x:%02x\n", p[8], p[9], p[10], p[11], p[12], p[13]);
            printf("Sender IP address:  \t\t%d.%d.%d.%d\n", p[14], p[15], p[16], p[17]);
            printf("Target MAC address: \t\t%.2x:%02x:%02x:%02x:%02x:%02x\n", p[18], p[19], p[20], p[21], p[22], p[23]);
            printf("Target IP address:  \t\t%d.%d.%d.%d\n", p[24], p[25], p[26], p[27]);
            break;
        case 0x0835:
            ip_head = ethernet_head + 14;
            p = ip_head;

            Ht = ((int)p[0] << 8) + p[1];
            printf("Hardware type:      \t\t0x%x\n", Ht);

            Pt = ((int)p[2] << 8) + p[3];
            printf("Protocol type:      \t\t0x%x\n", Pt);

            Hs = (int)p[4];
            printf("Hardware size:      \t\t0x%x\n", Hs);

            Ps = (int)p[5];
            printf("Protocol size:      \t\t0x%x\n", Ps);

            int Op = ((int)p[6] << 8) + p[7];
            printf("Opcode:             \t\t0x%x\n", Op);

            printf("Sender Mac address: \t\t%.2x:%02x:%02x:%02x:%02x:%02x\n", p[8], p[9], p[10], p[11], p[12], p[13]);
            printf("Sender IP address:  \t\t%d.%d.%d.%d\n", p[14], p[15], p[16], p[17]);
            printf("Target MAC address: \t\t%.2x:%02x:%02x:%02x:%02x:%02x\n", p[18], p[19], p[20], p[21], p[22], p[23]);
            printf("Target IP address:  \t\t%d.%d.%d.%d\n", p[24], p[25], p[26], p[27]);
            break;
        default:
            printf("Not defined.\n");
            break;
        }
    }
    return -1;
}