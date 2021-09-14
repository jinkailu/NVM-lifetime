//
//  main.cpp
//  FVC
//
//  Created by 金凯伦 on 2021/7/8.
//

#include <iostream>
#include <fstream>
#define times 64
using namespace std;
ofstream outfile;
uint64_t zero_c = 0,one_c = 0,zero_to_one = 0;
struct freva{
    uint64_t value;
    uint64_t num;
};
void rat(uint64_t *words,uint64_t *values,uint64_t count){
    uint64_t zeroc = 0,onec = 0;
    for(int i = 0;i < 64;i++){
        if(((words[0] >> i) & 0x1) == 1) onec++;
        else zeroc++;
    }
    for(int i = 1;i <= (16 - count); i++){
        for(int j = 0;j < 32;j++){
            if(((words[i] >> j) & 0x1) == 1) onec++;
            else zeroc++;
        }
    }
    //cout<<"压缩之后的0的个数和1的个数"<<zeroc<<"  "<<onec<<endl;
    zero_c += zeroc;
    one_c += onec;
}

void change(uint64_t *words,uint64_t *values,uint64_t count){
    uint64_t ztoocount = 0;
    for(int i = 0;i <= 16-count;i++){
        for(int j = 0;j < 32;j++){
            if(i > 1){
                if(((values[i] >> j) & 0x1) == 0 &&((words[i-1] >> j) & 0x1) == 1) ztoocount++;
            }
            if(i == 1){
                if(((values[i] >> j) & 0x1) == 0 &&((words[i-1] >> (j+32)) & 0x1) == 1) ztoocount++;
            }
            else{
                if(((values[i] >> j) & 0x1) == 0 &&((words[i] >> j) & 0x1) == 1) ztoocount++;
            }
        }
    }
    zero_to_one += ztoocount;
    //cout<<"从0到1的个数"<<ztoocount<<endl;
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
    for(int i = 0;i < 16-count;i++){
        for(int j = 0;j < 32;j++ ){
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
void sort(freva *fremidv,int ncou){
    uint64_t max = 0;
    freva chan;
    int index = 0;
    for(int j = 99-ncou-1;j >= 0;j--){
        if(fremidv[j].num >= max){
            max = fremidv[j].num;
            index = j;
        }
    }
    chan.value = fremidv[index].value;
    chan.num = fremidv[index].num;
    fremidv[index].value = fremidv[99].value;
    fremidv[index].num = fremidv[99].num;
    fremidv[99].value = chan.value;
    fremidv[99].num = chan.num;
    for(int i = 1;i < 8;i++){
        max = 0;
        for(int j = 99-ncou-1-i;j >= 0;j--){
            if(fremidv[j].num >= max){
                max = fremidv[j].num;
                index = j;
            }
        }
        chan.value = fremidv[index].value;
        chan.num = fremidv[index].num;
        fremidv[index].value = fremidv[99-ncou-1-i].value;
        fremidv[index].num = fremidv[99-ncou-1-i].num;
        fremidv[99-ncou-1-i].value = chan.value;
        fremidv[99-ncou-1-i].num = chan.num;
    }
}
void dealflow(freva *fremidv){
    uint64_t account = fremidv[0].num;
    int index = 0;
    for(int i = 1;i < 100;i++){
        if(account >= fremidv[i].num){
            account = fremidv[i].num;
            index = i;
        }
    }
    for(int i = index;i > 0;i--){
        fremidv[i].num = fremidv[i-1].num;
        fremidv[i].value = fremidv[i-1].value;
    }
}

uint64_t profile(freva *fremidv,uint64_t *values,int ncou){
    if(ncou == 99){
        fremidv[99].value=values[0];
        fremidv[99].num = 0;
        ncou--;
        for(int i = 1;i < 16;i++){
            bool nofind = true;
            for(int j = 99; j > ncou;j--){
                if(values[i] == fremidv[j].value) {
                    fremidv[j].num++;
                    nofind = false;
                    break;
                }
            }
            if(nofind){
                if(ncou < 0){
                    dealflow(fremidv);
                    ncou++;
                }
                fremidv[ncou].value = values[i];
                fremidv[ncou].num = 0;
                ncou--;
            }
        }
    }
    else{
        for(int i = 0;i < 16;i++){
            bool nofind = true;
            for(int j = 99; j > ncou;j--){
                if(values[i] == fremidv[j].value) {
                    fremidv[j].num++;
                    nofind = false;
                    break;
                }
            }
            if(nofind){
                if(ncou < 0){
                    dealflow(fremidv);
                    ncou++;
                }
                fremidv[ncou].value = values[i];
                fremidv[ncou].num = 0;
                ncou--;
            }
        }
    }
    return ncou;
}

uint64_t PFV(uint64_t *frevalues,uint64_t *values){
    uint64_t words[16];
    uint64_t mask[16];
    uint64_t count = 0,comSize = 0;
    for(int i = 0;i < 16;i++){
        mask[i] = 0x8;
        words[i] = 0x0;
    }
    for(int i = 0;i < 16;i++){
        for(uint64_t j = 0;j < 8;j++){
            if(values[i] == frevalues[j]){
                count++;
                mask[i] = j;
                break;
            }
        }
    }
    if(count < 8){
        for(int i = 0;i < 16;i++){
            words[i] = values[i];
        }
        comSize = 16*32;
        count = 0;
    }
    else{
        uint64_t site = 0x8,j = 1;
        for(int i = 0;i < 16;i++){
            if(mask[i] == 8){
                mask[i] = site;
                site += 0x1;
                words[j] = values[i];
                j++;
            }
            words[0] = (words[0]<<4) + mask[i];
        }
        comSize = 4*16 + (16 - count)*32;
    }
    rat(words,values,count);
    change(words,values,count);
    return comSize;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    uint64_t values[16],va[64];
    uint64_t frevalues[8];
    uint64_t comSize = 0;
    ifstream infile;
    float comratio = 0;
    long long int addr;
    infile.open("/Users/jinkailun/Desktop/压缩算法/BDI/BDI/file.txt");
    outfile.open("/Users/jinkailun/Desktop/压缩算法/BDI/BDI/outfile.txt");
    int ncou = 99,i,j=0;
    freva fremidv[100];
    for(i = 0;i < 16;i++){
        values[i] = 0;
    }
    for(i = 0;i< 100;i++){
        fremidv[i].num=0x0;
        fremidv[i].value=0x0;
    }
    j=0;i=0;
    while(!infile.eof()){
        if(i%65==0) {infile>>addr;
            outfile << addr << endl;
        }
        else infile >> va[i-1];
        if(i == 64 && j < times){
            for(int j = 0;j < 64;j += 4){
                for(int k = 0;k < 4;k++){
                    values[j/4] += (uint64_t)(va[k+j] << (8*(3-k)));
                    }
                }
            ncou = profile(fremidv,values,ncou);
        }
        if(j == times){
            sort(fremidv,ncou);
            for(uint64_t i = 0;i < 8; i++){
                frevalues[i] = fremidv[99-i].value;
            }
        }
        if(i == 64 && j >= times){
            for(int j = 0;j < 64;j += 4){
                for(int k = 0;k < 4;k++){
                    values[j/4] += (uint64_t)(va[k+j] << (8*(3-k)));
                    }
                }
            comSize = PFV(frevalues,values);
            comratio += (float)comSize/512;
        }
        i++;j++;
        i = i%65;
    }
    if(i != 0){
        for(;i < 64;i++)
            va[i] = 0;
        for(int j = 0;j < 64;j += 4){
            for(int k = 0;k < 4;k++){
                values[j/4] += (uint64_t)(va[k+j] << (8*(3-k)));
                }
            }
        comSize = PFV(frevalues,values);
        comratio += (float)comSize/512;
        comratio = comratio / (((j-times)/64)+1);
    }
    else
        comratio = comratio/((j-times) / 64);
    /*while(i<100){
        values[0] = 0;
        values[1] = 1;
        values[2] = -1;
        values[3] = 2;
        values[4] = i;
        values[5] = i*i - i;
        ncou = profile(fremidv, values, ncou);
        i++;
    }
    sort(fremidv,ncou);
    for(uint64_t i = 0;i < 8; i++){
        frevalues[i] = fremidv[99-i].value;
    }
    values[0] = 0;
    values[1] = 1000;
    values[2] = 0;
    values[3] = 99999;
    values[4] = -1;
    values[5] = 0xf17c8;
    values[6] = 5963;
    values[7] = 0;
    PFV(frevalues, values);*/
    cout<<"压缩率为  "<<comratio<<endl;
    cout<<"压缩之后的0的个数和1的个数 "<<zero_c<<"  "<<one_c<<endl;
    cout<<"由0变成1的个数为  "<<zero_to_one<<endl;
    infile.close();
    //std::cout << "Hello, World!\n";
    return 0;
}
