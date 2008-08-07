#include <stdio.h>

int getNybble(char x) {
    switch (x) {
        case 'a':
        case 'A':
            return 10;
        case 'b':
        case 'B':
            return 11;
        case 'c':
        case 'C':
            return 12;
        case 'd':
        case 'D':
            return 13;
        case 'e':
        case 'E':
            return 14;
        case 'f':
        case 'F':
            return 15;
        default:
            return x-'0';
    }
}

int main(int a, char* args[]) {
    int i = getNybble(args[3][2])*16 + getNybble(args[3][3]);
    int j;
    int y=(i&2?4:0)|(i&16?2:0) | (i&32?1:0);
    int x=(i&4?8:0) | (i&64?4:0) | (i&128?2:0) | (i&8?1:0);
    //printf("X=%d\n",x);
    switch(y) {
        case 4: y=3; break;
        case 5: y=2; break;
        case 6: y=1; break;
        case 7: y=0; break;
        case 0: y=7; break;
        case 1: y=6; break;
        case 2: y=5; break;
        case 3: y=4; break;
    }
    switch(x) {
        case 13:x=8; break;
        case 12:x=9; break;
        case 11: x=10; break;
        case 10: x=11; break;
        case 9: x=12; break;
        case 8: x=13; break;
        case 5: x=4; break;
        case 4: x=5; break;
        case 3: x=3; break;
        case 2: x=2; break;
        case 1: x=1; break;
        default:
            printf("%s %s %2.2X gave an Error\n",args[1],args[2],i);
            exit(-1);
    }
    j=(x&2?128:0) | (x&4?64:0) | (y&1?32:0) | (y&2?16:0) | (x&1?8:0) | (x&8?4:0) | (y&4?2:0); 
    printf("%s %s 0x%2.2X\n",args[1],args[2],j);
}
