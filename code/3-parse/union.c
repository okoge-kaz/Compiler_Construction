#include <stdio.h>
#include <stdlib.h>

typedef union {
    uint32_t ipv4_address;
    uint8_t ipv4_address_bytes[4];
} IP_ADDRESS;

// 共用体は各メンバーのサイズが一致している必要はない
typedef union {
    uint64_t ipv6_address;
    uint8_t ipv6_address_bytes[8];
    uint32_t ipv4_address;
    uint8_t ipv4_address_bytes[4];
} IP_ADDRESS_;

int main(int argc, char **argv) {
    IP_ADDRESS local_ip_address = {.ipv4_address_bytes = {192, 168, 1, 1}};
    printf("%d.%d.%d.%d\n", local_ip_address.ipv4_address_bytes[0], local_ip_address.ipv4_address_bytes[1], local_ip_address.ipv4_address_bytes[2], local_ip_address.ipv4_address_bytes[3]);

    IP_ADDRESS_ local_ip_address_ = {.ipv4_address_bytes = {192, 168, 1, 1}};
    IP_ADDRESS_ local_ip_address__ = {.ipv6_address_bytes = {0x2001, 0x0db8, 0x85a3, 0x0000, 0x0000, 0x8140, 0x3306, 0x6565}};
    // 大きさの違うものも代入できる
    return 0;
}