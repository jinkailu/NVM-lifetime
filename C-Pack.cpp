//
//  main.cpp
//  C-Pack
//
//  Created by 金凯伦 on 2021/7/1.
//

#include <iostream>
#include <fstream>
using namespace std;
#define dsize 16
ofstream outfile;
uint64_t zero_c = 0,one_c = 0,zero_to_one = 0;

void rat(uint64_t *values,uint64_t *words,uint64_t *wordPos){
    uint64_t zero_count = 0,one_count = 0;
    for(int i = 0;i < 16;i++){
        for(uint64_t j = 0;j < wordPos[i];j++){
            if(((words[i] >> j)&0x1) == 1) one_count++;
            else zero_count++;
        }
    }
    zero_c += zero_count;
    one_c += one_count;
    //cout<<"压缩之后的0的个数和1的个数"<<zero_count<<"  "<<one_count<<endl;
    zero_count = 0;one_count = 0;
    for(int i = 0;i < 16;i++){
        for(int j = 0;j < 32;j++){
            if(((values[i] >> j)&0x1) == 1) one_count++;
            else zero_count++;
        }
    }
    //cout<<"压缩之前的0的个数和1的个数"<<zero_count<<"  "<<one_count<<endl;
}

void change(uint64_t *values,uint64_t *words,uint64_t *wordPos){
    uint64_t num = 64*8-1,ztoocount = 0;
    for(int i = 0;i < 16;i++){
        for(int j = wordPos[i]-1;j >= 0;j-- ){
            uint64_t x=(values[(64*8-1-num)/32] >> (num%32)) & 0x1;
            uint64_t y=(words[i] >> j) & 0x1;
            if(x == 0 && y == 1)
                ztoocount++;
            num--;
        }
    }
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
    zero_to_one += ztoocount;
    //cout<<"由0变成1的个数为"<<ztoocount<<endl;
}

uint64_t c_peak(uint64_t *values,uint64_t dictionary[],uint64_t dicnum){
    uint64_t words[16];
    uint64_t wordPos[16];
    uint64_t comSize = 0;
    for(int i = 0;i < 16;i++){
        if(values[i] == 0){
            cout<<"zzzz"<<endl;
            words[i] = 0;
            wordPos[i] = 2;
            comSize +=wordPos[i];
            continue;
        } //zzzz
        if(values[i] < 0xFF){
            cout<<"zzzx"<<endl;
            words[i] = values[i] + 0xD00;
            wordPos[i] = 12;
            comSize +=wordPos[i];
            continue;
        }//zzzx
        int matchnum[16] ;
        for(int j = 0;j < 16;j++){
            matchnum[j] = 0;
        }
        for(int j = 0;j < dicnum;j++){
            uint64_t midvalue = 0,middic = 0;
            middic = (dictionary[j] >> 24) & 0xFF;
            midvalue = (values[i] >> 24) & 0xFF;
            if(midvalue != middic) {
                matchnum[j] = 0;
                continue;
            }
            middic = (dictionary[j] >> 16) & 0xFF;
            midvalue = (values[i] >> 16) & 0xFF;
            if(midvalue != middic) {
                matchnum[j] = 1;
                continue;
            }
            middic = (dictionary[j] >> 8) & 0xFF;
            midvalue = (values[i] >> 8) & 0xFF;
            if(midvalue != middic) {
                matchnum[j] = 2;
                continue;
            }
            middic = dictionary[j]  & 0xFF;
            midvalue = values[i]  & 0xFF;
            if(midvalue != middic) {
                matchnum[j] = 3;
                continue;
            }
            matchnum[j] = 4;
        }
        int index = 0,commat = matchnum[0];
        for(int j = 1;j < dicnum;j++){
            if(commat == 4) {break;}
            if(commat < matchnum[j])
            {
                commat = matchnum[j];
                index = j;
            }
        }
        if(commat == 4){
            cout<<"mmmm"<<endl;
            if(index == 0) words[i]=0x20 ;
            if(index == 1) words[i]=0x20 + 0x1 ;
            if(index == 2) words[i]=0x20 + 0x2 ;
            if(index == 3) words[i]=0x20 + 0x3 ;
            if(index == 4) words[i]=0x20 + 0x4 ;
            if(index == 5) words[i]=0x20 + 0x5 ;
            if(index == 6) words[i]=0x20 + 0x6 ;
            if(index == 7) words[i]=0x20 + 0x7 ;
            if(index == 8) words[i]=0x20 + 0x8 ;
            if(index == 9) words[i]=0x20 + 0x9 ;
            if(index == 10) words[i]=0x20 + 0xA ;
            if(index == 11) words[i]=0x20 + 0xB ;
            if(index == 12) words[i]=0x20 + 0xC ;
            if(index == 13) words[i]=0x20 + 0xD ;
            if(index == 14) words[i]=0x20 + 0xE ;
            if(index == 15) words[i]=0x20 + 0xF ;
            wordPos[i] = 6;
            comSize +=wordPos[i];
            dictionary[dicnum%dsize] = values[i];
            dicnum++;
            continue;
        }//mmmm
        if(commat == 3){
            cout<<"mmmx"<<endl;
            if(index == 0) words[i]=0xE000 + (values[i] & 0xFF);
            if(index == 1) words[i]=0xE000 + 0x100 + (values[i] & 0xFF);
            if(index == 2) words[i]=0xE000 + 0x200 + (values[i] & 0xFF);
            if(index == 3) words[i]=0xE000 + 0x300 + (values[i] & 0xFF);
            if(index == 4) words[i]=0xE000 + 0x400 + (values[i] & 0xFF);
            if(index == 5) words[i]=0xE000 + 0x500 + (values[i] & 0xFF);
            if(index == 6) words[i]=0xE000 + 0x600 + (values[i] & 0xFF);
            if(index == 7) words[i]=0xE000 + 0x700 + (values[i] & 0xFF);
            if(index == 8) words[i]=0xE000 + 0x800 + (values[i] & 0xFF);
            if(index == 9) words[i]=0xE000 + 0x900 + (values[i] & 0xFF);
            if(index == 10) words[i]=0xE000 + 0xA00 + (values[i] & 0xFF);
            if(index == 11) words[i]=0xE000 + 0xB00 + (values[i] & 0xFF);
            if(index == 12) words[i]=0xE000 + 0xC00 + (values[i] & 0xFF);
            if(index == 13) words[i]=0xE000 + 0xD00 + (values[i] & 0xFF);
            if(index == 14) words[i]=0xE000 + 0xE00 + (values[i] & 0xFF);
            if(index == 15) words[i]=0xE000 + 0xF00 + (values[i] & 0xFF);
            wordPos[i] = 16;
            comSize +=wordPos[i];
            dictionary[dicnum%dsize] = values[i];
            dicnum++;
            continue;
        }//mmmx
        if(commat == 2){
            cout<<"mmxx"<<endl;
            if(index == 0) words[i]=0xC00000 + (values[i] & 0xFFFF);
            if(index == 1) words[i]=0xC10000 + (values[i] & 0xFFFF);
            if(index == 2) words[i]=0xC20000 + (values[i] & 0xFFFF);
            if(index == 3) words[i]=0xC30000 + (values[i] & 0xFFFF);
            if(index == 4) words[i]=0xC40000 + (values[i] & 0xFFFF);
            if(index == 5) words[i]=0xC50000 + (values[i] & 0xFFFF);
            if(index == 6) words[i]=0xC60000 + (values[i] & 0xFFFF);
            if(index == 7) words[i]=0xC70000 + (values[i] & 0xFFFF);
            if(index == 8) words[i]=0xC80000 + (values[i] & 0xFFFF);
            if(index == 9) words[i]=0xC90000 + (values[i] & 0xFFFF);
            if(index == 10) words[i]=0xCA0000 + (values[i] & 0xFFFF);
            if(index == 11) words[i]=0xCB0000 + (values[i] & 0xFFFF);
            if(index == 12) words[i]=0xCC0000 + (values[i] & 0xFFFF);
            if(index == 13) words[i]=0xCD0000 + (values[i] & 0xFFFF);
            if(index == 14) words[i]=0xCE0000 + (values[i] & 0xFFFF);
            if(index == 15) words[i]=0xCF0000 + (values[i] & 0xFFFF);
            wordPos[i] = 24;
            comSize +=wordPos[i];
            dictionary[dicnum%dsize] = values[i];
            dicnum++;
            continue;
        }//mmxx
        if(commat < 2){
            cout<<"xxxx"<<endl;
            words[i] = values[i] + 0x100000000;
            wordPos[i] = 34 ;
            comSize +=wordPos[i];
            dictionary[dicnum%dsize] = values[i];
            dicnum++;
            continue;
        }//xxxx
    }
    rat(values,words,wordPos);
    change(values,words,wordPos);
    return comSize;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    uint64_t va[64];
    uint64_t dictionary[dsize],dicnum = 0;
    uint64_t *value = (uint64_t *)malloc(sizeof(uint64_t) * 16);
    ifstream infile;
    uint64_t comSize;
    float comratio = 0;
    long long int addr;
    infile.open("/Users/jinkailun/Desktop/压缩算法/BDI/BDI/file.txt");
    outfile.open("/Users/jinkailun/Desktop/压缩算法/BDI/BDI/outfile.txt");
    for(int i = 0;i < 16;i++){
        value[i] = 0;
        dictionary[i] = 0;
    }
    int i = 0,count = 0;
    while(!infile.eof()){
        if(i%65==0) {infile>>addr;
            outfile << addr << endl;
        }
        else infile >> va[i-1];
        if(i == 64){
            for(int j = 0;j < 64;j += 4){
                for(int k = 0;k < 4;k++){
                    value[j/4] += (uint64_t)(va[k+j] << (8*(3-k)));
                    }
                }
            comSize = c_peak(value,dictionary,dicnum);
            comratio += (float)comSize/512;
        }
        i++;count++;
        i = i%65;
    }
   
    if(i != 0){
        for(;i < 64;i++)
            va[i] = 0;
        for(int j = 0;j < 64;j += 4){
            for(int k = 0;k < 4;k++){
                value[j/4] += (uint64_t)(va[k+j] << (8*(3-k)));
                }
            }
        comSize = c_peak(value,dictionary,dicnum);
        comratio += (float)comSize/512;
        comratio = comratio / ((count/64)+1);
    }
    else
        comratio = comratio / (count / 64);
    /*for(int i = 0;i<64;i++){
        va[i] = 0;
    }
    va[0] = 0x12;
    va[1] = 0x34;
    va[2] = 0x56;
    va[3] = 0x78;
    va[4] = 0xAA;
    va[5] = 0xAA;
    va[6] = 0xAA;
    va[7] = 0xAA;
    va[8] = 0x12;
    va[9] = 0x34;
    va[12] = 0x35;
    va[13] = 0x27;
    va[14] = 0x89;
    va[15] = 0x4E;
    va[19] = 0xAB;
    va[20] = 0x12;
    va[21] = 0x34;
    va[22] = 0x56;
    va[23] = 0xAA;
    va[24] = 0xBB;
    va[25] = 0xBB;
    va[26] = 0x20;
    va[27] = 0x22;
    va[32] = 0x35;
    va[33] = 0x27;
    va[34] = 0x89;
    va[35] = 0x4E;
    va[36] = 0xBB;
    va[37] = 0xBB;
    va[38] = 0x00;
    va[39] = 0x00;
    for(int j = 0;j < 64;j += 4){
        for(int k = 0;k < 4;k++){
            value[j/4] += (uint64_t)(va[k+j] << (8*(3-k)));
            }
        }
    comSize = c_peak(value,dictionary,dicnum);*/
    std::cout << "压缩率为  "<<comratio<<endl;
    cout<<"压缩之后的0的个数和1的个数 "<<zero_c<<"  "<<one_c<<endl;
    cout<<"由0变成1的个数为 "<<zero_to_one<<endl;
    //std::cout << "Hello, World!\n";
    infile.close();
    return 0;
}
