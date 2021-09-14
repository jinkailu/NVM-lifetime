//
//  main.cpp
//  BDI
//
//  Created by 金凯伦 on 2021/7/13.
//

#include <iostream>
#include <fstream>
using namespace std;
ofstream outfile;
int rorw=0;
uint64_t zero_c = 0,one_c = 0,zero_to_one=0;
void rat(uint64_t *values,uint64_t *words,uint64_t *wordPos,uint64_t currCSize){
    uint64_t zero_count = 0,one_count = 0;
    for(int i = 0;i < currCSize;i++){
        for(uint64_t j = 0;j < wordPos[i];j++){
            if(((words[i] >> j)&0x1) == 1) one_count++;
            else zero_count++;
        }
    }
    //cout<<"压缩之后的0的个数和1的个数"<<zero_count<<"  "<<one_count<<endl;
    zero_c +=zero_count;
    one_c += one_count;
    zero_count = 0;one_count = 0;
    for(int i = 0;i < 64;i++){
        for(int j = 0;j < 8;j++){
            if(((values[i] >> j)&0x1) == 1) one_count++;
            else zero_count++;
        }
    }
    //cout<<"压缩之前的0的个数和1的个数"<<zero_count<<"  "<<one_count<<endl;
}

/*void change(uint64_t *values,uint64_t *words,uint64_t *wordPos){
    uint64_t num = 64*8-1,ztoocount = 0;
    for(int i = 0;i < 16;i++){
        for(uint64_t j = wordPos[i]-1;j >= 0;j-- ){
            uint64_t x=(values[num/32] >> (num%32)) & 0x1;
            uint64_t y=(words[i] >> j) & 0x1;
            if(x == 0 && y == 1)
                ztoocount++;
        }
    }
    cout<<"由0变成1的个数为"<<ztoocount<<endl;
}*/
void change(uint64_t *values,uint64_t *words,uint64_t *wordPos,uint64_t currCSize){
    uint64_t num = 64*8-1,ztoocount = 0;
   /* for(int i = 0;i < 4;i++){
        uint64_t x=(values[i/8] >> ((4-i)%8)) & 0x1;
        uint64_t y= (words[0] >> (4 - i)) & 0x1;
        if(x == 0 && y == 1)
            ztoocount++;
    }*/
    for(int i = 0;i < currCSize;i++){
        for(int j = wordPos[i]-1;j >= 0;j-- ){
            uint64_t x=(values[(64*8-1-num)/8] >> (num%8)) & 0x1;
            uint64_t y=(words[i] >> j) & 0x1;
           // cout<<x<<"  "<<y<<endl;
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
    for(int i = 0;i < currCSize;i++){
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
    //cout<<endl;
    zero_to_one += ztoocount;
    //cout<<"由0变成1的个数为"<<ztoocount<<endl;
}
uint64_t BDI(uint64_t *values){
    uint64_t words[35],curren[32];
    uint64_t wordPos[35];
    uint64_t comSize  = 512;
    uint64_t currCSize = 512;
    int i = 0,j = 0;
    for(i = 0;i < 8;i++){
        curren[i] = 0x0;
        for(j = 0;j < 8;j++){
            curren[i] += (values[i * 8 + j] << (56 - 8*j));
        }
    }
    for(i = 0;i < 8;i++){
        if(curren[0] != curren[i]){
            break;
        }
    }
    if(i >= 8 && curren[0] == 0){
        cout<<"all zero"<<endl;
        words[0] = 0x0;
        words[1] = 0x0;
        wordPos[0] = 4;
        wordPos[1] = 8;
        comSize = 12;
        currCSize = 2;
    }
    else if(i >= 8 && curren[0] != 0){
        cout<<"all the same"<<endl;
        words[0] = 0x1;
        wordPos[0] = 4;
        words[1] = curren[0];
        wordPos[1] = 64;
        comSize = 68;
        currCSize = 2;
    }
    bool cancom[6] ;
    for(j = 0;j < 6;j++){
        cancom[j] = true;
    }
    if(i < 8){
        for(j = 0;j < 8;j++){
            for(int k = 0;k < 7;k++){
                if(values[k] != values[j * 8 + k]){
                    cancom[0] = false;
                }
            }
            uint64_t mid = values[8*j+7] - values[7];
            if(mid > 0x7F && mid < 0xFFFFFFFFFFFFFF80)
                cancom[0] = false;
        }//8-1
        for(j = 0;j < 8;j++){
            for(int k = 0;k < 6;k++){
                if(values[k] != values[j * 8 + k]){
                    cancom[1] = false;
                }
            }
            uint64_t mid[3] ;
            mid[0]=(values[8*j+6] << 8)+values[8*j+7];
            mid[1]=(values[6] << 8)+values[7];
            mid[2]=mid[0]-mid[1];
            if(mid[2] > 0x7FFF && mid[2] < 0xFFFFFFFFFFFF8000)
                cancom[1] = false;
        }//8-2
        for(j = 0;j < 8;j++){
            for(int k = 0;k < 4;k++){
                if(values[k] != values[j * 8 + k]){
                    cancom[2] = false;
                }
            }
            uint64_t mid[3] ;
            mid[0]=(values[8*j+4] << 24)+(values[8*j+5] << 16)+(values[8*j+6] << 8)+values[8*j+7];
            mid[1]=(values[4] << 24)+(values[5] << 16)+(values[6] << 8)+values[7];
            mid[2]=mid[0]-mid[1];
            if(mid[2] > 0x7FFFFFFF && mid[2] < 0xFFFFFFFF80000000)
                cancom[2] = false;
        }//8-4
        for(j = 0;j < 16;j++){
            for(int k = 0;k < 3;k++){
                if(values[k] != values[j * 4 + k]){
                    cancom[3] = false;
                }
            }
            uint64_t mid[3];
            mid[2]=values[4*j+3] - values[3];
            if(mid[2] > 0x7F && mid[2] < 0xFFFFFFFFFFFFFF80)
                cancom[3] = false;
        }//4-1
        for(j = 0;j < 16;j++){
            for(int k = 0;k < 2;k++){
                if(values[k] != values[j * 4 + k]){
                    cancom[4] = false;
                }
            }
            uint64_t mid[3];
            mid[0]=(values[4*j+2] << 8)+values[4*j+3];
            mid[1]=(values[2] << 8)+values[3];
            mid[2]=mid[0]-mid[1];
            if(mid[2] > 0x7FFF && mid[2] < 0xFFFFFFFFFFFF8000)
                cancom[4] = false;
        }//4-2
        for(j = 0;j < 32;j++){
            if(values[0] != values[2 * j])
                cancom[5] = false;
            uint64_t mid[3];
            mid[2]=values[2*j+1] - values[1];
            if(mid[2] > 0x7F && mid[2] < 0xFFFFFFFFFFFFFF80)
                cancom[5] = false;
        }//2-1
        if(cancom[0]){
            cout<<"8-1"<<endl;
            words[0] = 0x2;
            wordPos[0] = 4;
            words[1] = curren[0];
            wordPos[1] = 64;
            for(j = 0;j < 8;j++){
                words[2 + j] =values[8*j+7] - values[7];
                wordPos[j+2] = 8;
            }
            comSize = 4 + 64 + 8*8;
            currCSize = 1+1+8;
        }
        else if(cancom[3]){
            cout<<"4-1"<<endl;
            words[0] = 0x5;
            wordPos[0] = 4;
            words[1] = (values[3]<<24)+(values[2]<<16)+(values[1]<<8)+values[0];
            wordPos[1] = 32;
            for(j = 0;j < 16;j++){
                words[2 + j] =values[4*j+3] - values[3];
                wordPos[j+2] = 8;
            }
            comSize = 4 + 32 + 8*16;
            currCSize = 1 + 1 +16;
        }
        else if(cancom[1]){
            cout<<"8-2"<<endl;
            words[0] = 0x3;
            wordPos[0] = 4;
            words[1] = curren[0];
            wordPos[1] = 64;
            for(j = 0;j < 8;j++){
                words[2 + j] =((values[8*j+6] << 8)+values[8*j+7]) - ((values[6] << 8)+values[7]);
                wordPos[j+2] = 16;
            }
            comSize = 4 + 64 + 8*16;
            currCSize = 1 + 1 + 8;
        }
        else if(cancom[5]){
            cout<<"2-1"<<endl;
            words[0] = 0x7;
            wordPos[0] = 4;
            words[1] = (values[1]<<8)+values[0];
            wordPos[1] = 16;
            for(j = 0;j < 32;j++){
                words[2 + j] =values[2*j+1] - values[1];
                wordPos[j+2] = 8;
            }
            comSize = 4 + 16 + 8*32;
            currCSize = 1 + 1+ 32;
        }
        else if(cancom[4]){
            cout<<"4-2"<<endl;
            words[0] = 0x6;
            wordPos[0] = 4;
            words[1] = curren[0] & 0xFFFFFFFF;
            wordPos[1] = 32;
            for(j = 0;j < 16;j++){
                words[2 + j] =((values[4*j+2] << 8)+values[4*j+3]) - ((values[2] << 8)+values[3]);
                wordPos[j+2] = 16;
            }
            comSize = 4 + 32 + 16*16;
            currCSize = 2 + 16;
        }
        else if(cancom[2]){
            cout<<"8-4"<<endl;
            words[0] = 0x4;
            wordPos[0] = 4;
            words[1] = curren[0];
            wordPos[1] = 64;
            for(j = 0;j < 8;j++){
                words[2 + j] =(curren[j]&0xFFFFFFFF)-(curren[0]&0xFFFFFFFF);
                wordPos[j+2] = 32;
            }
            comSize = 4 + 64 + 8*32;
            currCSize = 2 + 8;
        }
        else{
            //cout<<"umcompress"<<endl;
            for(int i = 0;i < 8;i++){
                words[i] = curren[i];
                wordPos[i] = 64;
            }
            comSize = 8 * 64;
            currCSize = 8;
        }
    }
    //if(rorw == 1){
        rat(values,words,wordPos,currCSize);
        change(values,words,wordPos,currCSize);
    //}
    return comSize;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    uint64_t values[64];
    ifstream infile,ifile;
    uint64_t comSize=0;
    infile.open("/Users/jinkailun/Desktop/pin/rbtree.out");
    ifile.open("/Users/jinkailun/Desktop/rw.out");
    if(!infile.is_open()){
        cout<<"打开出错";
    }
   
    outfile.open("/Users/jinkailun/Desktop/压缩算法/BDI/BDI/outfile.txt");
    float comratio = 0;
    /*for(int i = 0;i < 64;i++)
        values[i]=0;
    for(int i = 7;i < 64;i += 8){
        values[i] = j;
        j++;
    }*/
    int i=0;
    int count = 0;
    long long int addr;
    string a;
    while(!infile.eof()){
        if(i%65==0) {infile>>addr;
            ifile>>a;
            if(a=="R") rorw = 0;
            else rorw = 1;
            //cout<<rorw;
            outfile << addr << endl;
        }
        else {infile >> values[i - 1];}
        if(i == 64){
            comSize = BDI(values);
            if(rorw ==1) comratio += (float)comSize/512;
        }
        if(rorw == 1 && i%65!=0) count++;
        i++;
        i=i%65;
    }
    if(i != 0){
        for(;i < 64;i++)
            values[i] = 0;
        comSize=BDI(values);
        comratio += (float)comSize/512;
        comratio = comratio / ((count/64)+1);
    }
    else
        comratio = comratio / (count/64);
    std::cout << "压缩率为  "<<comratio<<endl;
    cout<<"压缩之后的0的个数和1的个数 "<<zero_c<<"  "<<one_c<<endl;
    cout<<"由0变成1的个数为 "<<zero_to_one<<endl;
    infile.close();
    return 0;
}
