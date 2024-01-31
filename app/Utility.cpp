#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <ifaddrs.h>
#include <random>
#include <openssl/aes.h>
#include <ctime>
#include <sys/sysinfo.h>
#include <arpa/inet.h>

char* getHwaddr(const char* dev, bool colonEn) {
    int sock, e;
    uint8_t *p;
    static char hwaddr[18];
    struct ifreq ifr;

    *hwaddr = 0;  /* erase string */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (!(sock < 0)) {
        strcpy(ifr.ifr_name, dev);
        e = ioctl(sock, SIOCGIFHWADDR, &ifr);
        if (e < 0) {
            close(sock);
        } else {
            p = (uint8_t *)ifr.ifr_hwaddr.sa_data;
            if (colonEn)
                sprintf(hwaddr, "%02X:%02X:%02X:%02X:%02X:%02X", p[0], p[1], p[2], p[3], p[4], p[5]);
            else
                sprintf(hwaddr, "%02X%02X%02X%02X%02X%02X", p[0], p[1], p[2], p[3], p[4], p[5]);
            close(sock);
        }
    }
    return hwaddr;
}

char* getAddr(const char* dev) {
    int sock, e;
    struct ifreq ifr;
    struct in_addr *saddr;
    static char ip[INET_ADDRSTRLEN];

    *ip = 0;  /* erase string */
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (!(sock < 0)) {
        /* I want to get an IPv4 IP address */
        //ifr.ifr_addr.sa_family = AF_INET;
        strcpy(ifr.ifr_name, dev);
        e = ioctl(sock, SIOCGIFADDR, &ifr);
        if (e < 0) {
            close(sock);
        } else {
            saddr = &((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr;
            inet_ntop(AF_INET, saddr, ip, sizeof(ip));
            close(sock);
        }
    }
    return ip;
}

char* getEthName(int index) {
    struct ifaddrs *addrs,*tmp;
    static char name[128] = "lo";
    getifaddrs(&addrs);
    tmp = addrs;
    int i = 0;
    while (tmp) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET) {
            if (index == i){
                strncpy(name, tmp->ifa_name, 128);
                break;
            }
        }
        tmp = tmp->ifa_next;
        i++;
    }
    freeifaddrs(addrs);
    return name;
}

char* getUuid(const char* path) {
    //FILE* fp = fopen(Spec::instance()->uuidPath().c_str(), "a+");
    FILE* fp = fopen(path, "a+");
    //uuid 8-4-4-12
    static char uuid[36 + 1];
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, 0xFFFF);

    memset(uuid, 0, sizeof(uuid));
    fread(uuid, sizeof(char), sizeof(uuid) - 1, fp);
    if (strlen(uuid) == 36) {
        printf("%s, %s, %d read UUID %s : %s\n", __FILE__, __func__, __LINE__, path, uuid);
    } else {
        memset(uuid, 0, sizeof(uuid));
        sprintf(uuid, "%04x%04x-%04x-%04x-%04x-%04x%04x%04x",
                dis(gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen), dis(gen));
        printf("%s, %s, %d UUID doesn't exist! create: %s\n", __FILE__, __func__, __LINE__, uuid);
        fseek(fp, 0, SEEK_SET);
        fwrite(uuid, sizeof(char), sizeof(uuid) - 1, fp);
    }
    fclose(fp);
    return uuid;
}

static char hexToChar(char hexUp, char hexLow) {
    char digit;
    digit = 16 * (hexUp >= 'A' ? ((hexUp & 0xdf) - 'A') + 10 : (hexUp - '0'));
    digit += (hexLow >= 'A' ? ((hexLow & 0xdf) - 'A') + 10 : (hexLow - '0'));
    return (digit);
}

char *urlDecode(char *str) {
    int i, j;
    if (!str) return NULL;
    for (i = j = 0; str[j]; i++, j++) {
        switch (str[j]) {
            case '+':
                str[i] = ' ';
                break;
            case '%':
                str[i] = hexToChar(str[j + 1], str[j + 2]);
                j += 2;
                break;
            default:
                str[i] = str[j];
                break;
        }
    }
    str[i]='\0';
    return str;
}

char* getTimeStr(time_t rawtime, const char* format) {
    static char timeStr[32] = {0,};
    std::tm* timeinfo = std::localtime(&rawtime);
    if (strncmp(format, "YY/MM/DD hh:mm:ss", strlen(format)) == 0) {
        std::strftime(timeStr, 32, "%y/%m/%d %H:%M:%S ", timeinfo);
    } else if (strncmp(format, "DD/MM/YY hh:mm:ss", strlen(format)) == 0) {
        std::strftime(timeStr, 32, "%d/%m/%y %H:%M:%S ", timeinfo);
    } else {
        std::strftime(timeStr, 32, "%y/%m/%d %H:%M:%S ", timeinfo);
    }
    return timeStr;
}

char* getUptimeStr(void) {
    static char timeStr[32] = {0,};
    const long minute = 60;
    const long hour = minute * 60;
    const long day = hour * 24;
    struct sysinfo sInfo;
    sysinfo(&sInfo);
    sprintf(timeStr, "%ld days, %ld hours, %ld minutes",
            sInfo.uptime / day, (sInfo.uptime % day) / hour, (sInfo.uptime % hour) / minute);

    return timeStr;
}

int decodeBase64(char* input, char* output) {
    unsigned char counts = 0;
    char buffer[4];
    int i = 0, p = 0;
    char base46_map[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
    };

    for (i = 0; input[i] != '\0'; i++) {
        unsigned char k;
        for (k = 0 ; k < 64 && base46_map[k] != input[i]; k++);
        buffer[counts++] = k;
        if (counts == 4) {
            output[p++] = (buffer[0] << 2) + (buffer[1] >> 4);
            if (buffer[2] != 64)
                output[p++] = (buffer[1] << 4) + (buffer[2] >> 2);
            if (buffer[3] != 64)
                output[p++] = (buffer[2] << 6) + buffer[3];
            counts = 0;
        }
    }
    output[p] = '\0';/* string padding character */
    return p;
}

/*int decryptAes128Base64UrlEncoding(const char* input, char* output, const char* key, const char* iv) {
    AES_KEY decryptKey;
    char decryptIv[16 + 1] = {0,};
    char inputText[256] = {0,};
    char cipherText[256] = {0,};
    char plainText[256] = {0,};
    unsigned int len = 0;
    unsigned int padding = 0;

    strncat(decryptIv, iv, 16);
    decryptIv[16] = '\0';
    strncpy(inputText, input, sizeof(inputText));
    urlDecode(inputText);
    len = decodeBase64(inputText, cipherText);

    AES_set_decrypt_key((const unsigned char*)key, 128, &decryptKey);
    AES_cbc_encrypt((const unsigned char*)cipherText, (unsigned char*)plainText,
            len, &decryptKey, (unsigned char*)decryptIv, AES_DECRYPT);

    len = strlen(plainText);
    padding = plainText[len - 1];
    if (padding > 15) padding = 0;
    len = len - padding;
    plainText[len] = '\0';
    printf("%s, %s, %d, [%s](%ld)\n", __FILE__, __func__, __LINE__, plainText, strlen(plainText));
    strncpy(output, plainText, len);
    return 0;
}*/

const char* generateKey(int bits) {
    static std::string key;
    std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, alphabet.length() - 1);
    int len = bits / 8;
    key.clear();
    for (int i = 0; i < len; i++) {
        int idx = dis(gen);
        key.push_back(alphabet.at(idx));
    }
    return key.c_str();
}

unsigned int createCrc(unsigned int crc, const char* pBuffer, unsigned int len) {
    unsigned int crc32Table[256] = {
        0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
        0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
        0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
        0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
        0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
        0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
        0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
        0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
        0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
        0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
        0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
        0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
        0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
        0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
        0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
        0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
        0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
        0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
        0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
        0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
        0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
        0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
        0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
        0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
        0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
        0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
        0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
        0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
        0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
        0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
        0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
        0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
        0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
        0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
        0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
        0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
        0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
        0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
        0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
        0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
        0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
        0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
        0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
        0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
        0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
        0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
        0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
        0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
        0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
        0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
        0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
        0x2d02ef8dL
    };

    const char *s = (const char *)pBuffer;
    if (s == NULL) {
        return crc;
    }

    while (len) {
        crc = crc32Table[(crc ^ *s++) & 0xff] ^ (crc >> 8);
        len --;
    }
    return crc;
}

bool checkInetAddr(char* addr) {
    if (inet_addr(addr) == INADDR_NONE)
        return false;
    else
        return true;
}

#define PRINT_HEX_DUMP_ASCII 0
void printHexDump(unsigned char* data, unsigned int size, unsigned int width) {
    unsigned int i;
    unsigned char* ptr = data;
    unsigned char count = 0;
#if PRINT_HEX_DUMP_ASCII
    unsigned int j;
    unsigned char temp;
    int space;
#endif
    for (i = 0; i < size; i++, count++) {
        if (i == 0) {
            printf("           ");
        } else if (i % width == 0) {
#if PRINT_HEX_DUMP_ASCII
            printf("           ");
            j = i - width;
            for ( ; j<i; j++) {
                temp = *(ptr + j);
                if (temp == '\r' || temp == '\n') temp='.';
                printf("%c", temp);
            }
#endif
            printf("\n           ");
            count = 0;
        }
        printf(" %02X", *(ptr + i));
    }

#if PRINT_HEX_DUMP_ASCII
    space = width - count;
    if (space < 0) space = 0;
    for (i = 0; i < (unsigned)space; i++) printf("   ");
    printf("           ");
    j = size - count;
    for (; j < size; j++) {
        temp = *(ptr + j);
        if (temp == '\r' || temp == '\n') temp='.';
        printf("%c", temp);
    }
#endif
    printf("\n");
}
