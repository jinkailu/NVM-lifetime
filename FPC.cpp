//
//  main.cpp
//  FPC
//
//  Created by 金凯伦 on 2021/6/21.
//

#include <iostream>
#include <fstream>
using namespace std;
ofstream outfile;
int rorw;
uint64_t zero_c = 0,one_c = 0,zero_to_one=0;
void rat(uint64_t *values,uint64_t *words,uint64_t *wordPos){
    uint64_t zero_count = 0,one_count = 0;
    for(int i = 0;i < 16;i++){
        for(uint64_t j = 0;j < wordPos[i];j++){
            if(((words[i] >> j)&0x1) == 1) one_count++;
            else zero_count++;
        }
    }
    zero_c +=zero_count;
    one_c += one_count;
    //cout<<"压缩之后的0的个数和1的个数"<<zero_count<<"  "<<one_count<<endl;
    zero_count = 0;one_count = 0;
    for(int i = 0;i < 64;i++){
        for(int j = 0;j < 8;j++){
            if(((values[i] >> j)&0x1) == 1) one_count++;
            else zero_count++;
        }
    }
    //cout<<"压缩之前的0的个数和1的个数"<<zero_count<<"  "<<one_count<<endl;
}

void change(uint64_t *values,uint64_t *words,uint64_t *wordPos){
    uint64_t num = 64*8-1,ztoocount = 0;
    /*for(int i = 0;i < 16;i++){
        encode += ((words[i] >> (wordPos[i] - 3)) & 0x7) << (3 * i);
    }
    for(int i = 0;i < 48;i++){
        uint64_t x=(values[i/8] >> ((47-i)%8)) & 0x1;
        uint64_t y= (encode >> (48 - i)) & 0x1;
        if(x == 0 && y == 1)
            ztoocount++;
    }*/
    for(int i = 0;i < 16;i++){
        //cout<<hex<<words[i]<<endl;
        for(int j = wordPos[i]-1;j >= 0;j-- ){
            uint64_t x=(values[(64*8-1-num)/8] >> (num%8)) & 0x1;
            uint64_t y=(words[i] >> j) & 0x1;
            if(x == 0 && y == 1)
                ztoocount++;
            num--;
           // cout<<x<<"  "<<y<<endl;
            
        }
    }
    zero_to_one += ztoocount;
    //cout<<"由0变成1的个数为"<<ztoocount<<endl;
    int num1 = 64*8-1;
    while(num1>=0){
            uint64_t x=(values[(64*8-1-num1)/8] >> (num1%8)) & 0x1;
            outfile << x ;
            //cout<<x;
            num1--;
    }
    //cout<<endl;
    outfile<<endl;
    num1 = 64*8-1;
    for(int i = 0;i < 16;i++){
        for(int j = wordPos[i]-1;j >= 0;j-- ){
            uint64_t y=(words[i] >> j) & 0x1;
            outfile << y;
            //cout<<y;
            num1--;
        }
    }
    while(num1>=0){
            uint64_t x=(values[(64*8-1-num1)/8] >> (num1%8)) & 0x1;
            outfile << x ;
            //cout<<x;
            num1--;
    }
    outfile<<endl;
}

uint64_t FPC(uint64_t *cline){
    uint64_t words[16],values[16],worsPos[16];
    uint64_t comSize=0;
    for(int i = 0;i < 16;i++){
        values[i] = 0x0;
    }
    for(int i=0;i<64;i+=4){
        for(int j=0;j<4;j++){
            values[i/4] +=(cline[i+j]<<(8*(3-j)));
        }
    }
    for(int i=0;i<16;i++){
        if(values[i]==0){
            cout<<"all 0"<<endl;
            words[i]=values[i]+0x0;
            worsPos[i]=3;
            comSize+=3;
            continue;
        }//000
        if(((values[i])<=0x7)||(values[i]>=0xFFFFFFF8)){
            cout<<"4-bit sign extended"<<endl;
            int x=(int)(values[i]);
            if((x>>31)==1)
                words[i]=(values[i]&0xF)+0x10;
            else
                words[i]=values[i]+0x10;
            worsPos[i]=7;
            comSize+=7;
            continue;
        }//001
        if((values[i]<=0x7F)||(values[i]>=0xFFFFFF80)){
            cout<<"1-byte sign extended"<<endl;
            if(((values[i]>>31)&0x1)==1)
                words[i]=(values[i]&0xFF)+0x200;
            else words[i]=values[i]+0x200;
            worsPos[i]=11;
            comSize+=11;
            continue;
        }//010
        if((values[i]<=0x7FFF)||(values[i]>=0xFFFF8000)){
            cout<<"Halfword sign extended"<<endl;
            int x=(int)(values[i]);
            if((x>>31)==1)
                words[i]=(values[i]&0xFFFF)+0x30000;
            else words[i]=values[i]+0x30000;
            worsPos[i]=19;
            comSize+=19;
            continue;
        }//011
        if((values[i]&0xFFFF)==0){
            cout<<"Halfword ,padded with a zero Halfword"<<endl;
            words[i]=(values[i]>>16)+0x40000;
            worsPos[i]=19;
            comSize+=19;
            continue;
        }//100
        uint64_t byte0=values[i]&0xFFFF;
        uint64_t byte1=(values[i]>>16)&0xFFFF;
        if((byte0 <= 0x7F||byte0 >= 0xFF80)&&
           (byte1 <= 0x7F||byte1 >= 0xFF80)){
            cout<<"Two Halfword,each a byte sign extended"<<endl;
            words[i]=((byte1 & 0xFF)<<8) + (byte0 & 0xFF) + 0x50000;
            worsPos[i]=19;
            comSize+=19;
            continue;
        }//101
        byte0=values[i]&0xFF;
        byte1=(values[i]>>8)&0xFF;
        uint64_t byte2=(values[i]>>16)&0xFF;
        uint64_t byte3=(values[i]>>24)&0xFF;
        if(byte0 == byte1 && byte0 == byte2 && byte0 == byte3){
            cout<<"repeated"<<endl;
            words[i] = byte0 + 0x600;
            worsPos[i] = 11;
            comSize += 11;
            continue;
        }//110
        //cout<<"umcompress"<<endl;
        words[i] = values[i] + 0x700000000;
        worsPos[i] = 35;
        comSize += 35;
    }
    
    if(comSize >= 512){
        for(int j = 0;j < 16; j++) {
            words[j] = values[j];
            worsPos[j] = 32 ;
            comSize = 512;
        }
      }
    //cout<<comSize<<endl;
    //if(rorw == 1){
    rat(cline,words,worsPos);
    change(cline,words,worsPos);
    //}
    return comSize;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    ifstream infile,ifile;
    uint64_t buffer[64],comSize = 0;
    float ratio=0;
    int i=0,count=0;
    long long int addr;
    string a;
    infile.open("/Users/jinkailun/Desktop/pin/rbtree.out");
    ifile.open("/Users/jinkailun/Desktop/rw.out");
    outfile.open("/Users/jinkailun/Desktop/压缩算法/FPC/FPC/outfile.txt");
    float comratio = 0;
    //cout<<1<<endl;
    while(!infile.eof()){
        if(i%65==0) {infile>>addr;
            ifile >> a;
            if(a=="R") rorw = 0;
            else rorw = 1;
            outfile << addr << endl;
        }
        else infile>>buffer[i-1];
    /*for(int i = 0;i < 64;i++){
        buffer[i] = 0;
    }
    buffer[3] = 0x7;
    for(int i = 4;i <= 6;i++){
        buffer[i] = 0xFF;
    }
    buffer[7] = 0xB6;
    buffer[10] = 0x54;
    buffer[11] = 0x32;
    buffer[12] = 0x54;
    buffer[13] = 0x32;
    buffer[16] = 0xFF;
    buffer[17] = 0xB6;
    buffer[19] = 0x36;
    for(int i = 20;i < 24;i++){
        buffer[i] = 0x20;
    }
    buffer[28] = 0xF4;
    buffer[29] = 0xF6;
    buffer[30] = 0xF7;
    buffer[31] = 0xF8;*/
        if(i == 64){
            comSize=FPC(buffer);
            if(rorw ==1 )ratio += (float)comSize/512;
        }
        i++;
        if(rorw == 1 && i%65!=0) count++;
        i=i%65;
    }
    if(i != 0){
        for(;i < 64;i++)
            buffer[i] = 0;
        comSize=FPC(buffer);
        ratio += (float)comSize/512;
        ratio = ratio / ((count/64)+1);
    }
    else
       ratio = ratio / (count/64);
    cout<<"压缩率为:"<<ratio<<endl;
    cout<<"压缩之后的0的个数和1的个数"<<zero_c<<"  "<<one_c<<endl;
    cout<<"由0变成1的个数为"<<zero_to_one<<endl;
//}
    std::cout << "Hello, World!\n";
    infile.close();
    return 0;
}
