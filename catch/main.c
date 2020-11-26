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
    char *ethernet_head;
    char *ip_head;
    unsigned char *p;
    unsigned char *typeOfFrame;
    if ((fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0)
    {
        printf("error create raw socket\n");
        return -1;
    }

    while (1)
    {
        printf("---------------------------------\n");
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
               p[6], p[7], p[8], p[9], p[10], p[11]);

        printf("Src MAC address: %.2x:%02x:%02x:%02x:%02x:%02x\n",
               p[0], p[1], p[2], p[3], p[4], p[5]);
        typeOfFrame = ethernet_head + 12;
        printf("type of frame: %.2x%02x\n", typeOfFrame[0], typeOfFrame[1]);
        int temp = (int)typeOfFrame[0] << 8 + (int)(typeOfFrame[1]);
        switch (temp)
        {
        case 0x0800:
            ip_head = ethernet_head + 14;
            p = ip_head + 12;
            printf("Dst IP: %d.%d.%d.%d\n", p[0], p[1], p[2], p[3]);
            printf("Src IP: %d.%d.%d.%d\n", p[4], p[5], p[6], p[7]);
            proto = (ip_head + 9)[0];
            p = ip_head + 12;
            unsigned char *version = ip_head;
            int v = version[0] >> 4;
            int len = version[0] & 0xf;
            unsigned char *totalLen = version + 2;
            int tl = ((int)totalLen[0]) << 8 + (int)totalLen[1];
            unsigned char *identification = totalLen;
            int id = ((int)identification[0]) << 8 + (int)identification[1];
            unsigned char *flag = identification + 2;
            int fg = ((int)flag[0]) << 8 + (int)flag[1];
            unsigned char *fragmentOffset = flag + 2;
            unsigned char *timeTolive = fragmentOffset + 2;
            int ttl = timeTolive[0];

            printf("Version: %d\n", v);
            printf("Header length: %d\n", len);
            printf("Total length: %d\n", tl);
            printf("identification: 0x%x\n", id);
            printf("flag: 0x%x\n", fg);
            printf("Time to live: %d\n", ttl);

            printf("Protocol:");
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
            break;
        case 0x0806:
            printf("ARP\n");
            break;
        case 0x0835:
            printf("RARP\n");
            break;
        default:
            printf("Not defined.\n");
            break;
        }
    }
    return -1;
}