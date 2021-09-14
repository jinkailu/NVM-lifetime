//
//  main.cpp
//  SC2
//
//  Created by 金凯伦 on 2021/7/10.
//

#include <iostream>
#include <math.h>
#include <string>
#include <fstream>
#define times 2*64
#define listsize 1024
#define monitorp 1000
using namespace std;
uint64_t cou = 0;
uint64_t zero_c = 0,one_c = 0,zero_to_one = 0;
ofstream outfile;
struct VFT{
    uint64_t value;
    uint64_t counter;
};
struct LUT{
    uint64_t value;
    uint64_t cw;
    uint32_t cL;
};
struct list{
    uint64_t v;
    bool flage;
};
struct node{
    //uint64_t parent;
    uint16_t leftchild;
    uint16_t rightchild;
    uint16_t site;
    uint64_t code;//最长19bit
    uint32_t codesize;//最大19
};
void rat(uint64_t *values,uint64_t *words,uint64_t *wordPos){
    uint64_t zero_count = 0,one_count = 0;
    for(int i = 0;i < 16;i++){
        for(int j = wordPos[i]-1;j >= 0;j--){
            if(((words[i] >> j)&0x1) == 1) one_count++;
            else zero_count++;
        }
    }
    zero_c += zero_count;
    one_c +=one_count;
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
    uint64_t num = 16*32-1,ztoocount = 0;
    for(int i = 0;i < 16;i++){
        for(int j = wordPos[i]-1;j >= 0;j-- ){
            uint64_t x=(values[(16*32-1-num)/32] >> (num%32)) & 0x1;
            uint64_t y=(words[i] >> j) & 0x1;
            if(x == 0 && y == 1)
                ztoocount++;
            num--;
        }
    }
    zero_to_one += ztoocount;
   // cout<<"由0变成1的个数为"<<ztoocount<<endl;
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
uint64_t findsite(VFT *fretable,uint64_t addr){
    ifstream fine;
    uint64_t value=0,add=0,va = 0;
    int i = 0;
    bool findvalue = false;
    uint64_t oldvalue = 0;
    fine.open("/Users/jinkailun/Desktop/bitfilp/file.txt");
    while(!fine.eof()){
        fine>>value;
        fine>>add;
        if(add == addr) {
            va = value;
            break;
        }
    }
    while(i < cou){
        if(va == fretable[i].value){
            findvalue = true;
            break;
        }
        i++;
    }
    if(findvalue){
        oldvalue = i;
    }
    return oldvalue;
}
int dealoverflow(VFT *fretable){
    cout<<"deal overflow"<<endl;
    uint64_t minicou = fretable[0].counter;
    int index = 0;
    for(int i = 0;i < cou;i++){
        if(minicou >= fretable[i].counter){
            index = i;
        }
    }
    cou--;
    cout<<cou<<endl;
    return index;
}
void sampling(VFT *fretable,uint64_t operat,uint64_t *values,uint64_t addr[]){
    bool find = false;
    if(operat == 0){//block fetch
        cout<<"block fetch"<<endl;
       // cout<<cou<<endl;
        for(int i = 0;i < 16;i++){
            for(uint64_t j = 0;j < cou;j++){
                if(values[i] == fretable[j].value){
                    find = true;
                    fretable[j].counter++;
                    break;
                }
            }
            if(!find){
                int index = 0;
                if(cou >= listsize){
                    index = dealoverflow(fretable);
                    fretable[index].value = values[i];
                    fretable[index].counter = 0;
                    //cout<<fretable[index].value<<" "<<fretable[index].counter<<endl;
                    //cout<<index<<endl;
                }
                else{
                    fretable[cou].value = values[i];
                    fretable[cou].counter = 0;
                    //cout<<fretable[cou].value<<" "<<fretable[cou].counter<<endl;
                    cou++;
                }
            }
        }
    }
    if(operat == 1){//block update
        cout<<"block update"<<endl;
        uint64_t oldvalue;
        for(int i = 0;i < 16;i++){
            oldvalue=findsite(fretable,addr[i]);
            int index = 0;
            if(values[i] == fretable[oldvalue].value) fretable[oldvalue].counter++;
            else {
                fretable[oldvalue].counter--;
                if(cou >= listsize){
                    index = dealoverflow(fretable);
                    fretable[index].value = values[i];
                    fretable[index].counter = 0;
                }
                else{
                    fretable[cou].value = values[i];
                    fretable[cou].counter = 0;
                    cou++;
                    
                }
            }
        }
    }
    if(operat == 2){//block evict
        cout<<"block evict"<<endl;
        uint64_t oldvalue;
        for(int i = 0;i < 16;i++){
            oldvalue=findsite(fretable,addr[i]);
            fretable[oldvalue].counter--;
        }
    }
    //cout<<1<<" "<<cou<<endl;
}
/*void sampling(VFT *fretable,uint64_t operat,uint64_t *values,uint64_t cou){
    
}*/

int huffman(VFT *fretable,LUT *lookup){
    list Cnt[listsize],Heap[listsize],R;
    for(int i = 0;i < cou;i++){
        Cnt[i].v = fretable[i].counter;
        Cnt[i].flage = false;
    }
    uint64_t mini;
    for(int i = 0;i < cou;i++){
        mini = pow(2,24);
        uint64_t index = 100;
        for(uint64_t j = 0;j < cou;j++){
            if(mini >= Cnt[j].v && !Cnt[j].flage){
                mini = Cnt[j].v;
                index = j;
            }
        }
        Cnt[index].flage = true;
        Heap[i].v = index;
        Heap[i].flage = false;
    }
    /*for(int i = 0;i < cou;i++){
        cout<<Heap[i].v<<" ";
    }*/
    int ptr = cou - 1;
    for(int i = 0;i < cou-1;i++){
        list mid ;
        mid.v = 0;
        mid.flage = false;
        for(int j = 0;j < ptr;j++){
            if(!Heap[j].flage && !Heap[j+1].flage){
                if(Cnt[Heap[j].v].v > Cnt[Heap[j+1].v].v){
                    mid = Heap[j];
                    Heap[j] = Heap[j+1];
                    Heap[j+1] = mid;
                }
            }
            if(Heap[j].flage && !Heap[j+1].flage){
                if(Heap[j].v > Cnt[Heap[j+1].v].v){
                    mid = Heap[j];
                    Heap[j] = Heap[j+1];
                    Heap[j+1] = mid;
                }
            }
            if(!Heap[j].flage && Heap[j+1].flage){
                if(Cnt[Heap[j].v].v> Heap[j+1].v){
                    mid = Heap[j];
                    Heap[j] = Heap[j+1];
                    Heap[j+1] = mid;
                }
            }
            if(Heap[j].flage && Heap[j+1].flage){
                if(Heap[j].v > Heap[j+1].v){
                    mid = Heap[j];
                    Heap[j] = Heap[j+1];
                    Heap[j+1] = mid;
                }
            }
        }
        R.v = Heap[0].v;
        R.flage = Heap[0].flage;
        for(int j = 0;j < ptr;j++){
            Heap[j].v = Heap[j+1].v;
            Heap[j].flage = Heap[j+1].flage;
        }
        if(R.flage && Heap[0].flage){
            Heap[ptr].v = Heap[0].v + R.v;
            Heap[ptr].flage = true;
            Heap[ptr+1].v = ptr;
            Heap[ptr+2].v = ptr;
        }
        if(!R.flage && Heap[0].flage){
            Heap[ptr].v =Heap[0].v + Cnt[R.v].v;
            Heap[ptr].flage = true;
            Heap[ptr+1].v = ptr;
            Cnt[R.v].v = ptr;
        }
        if(R.flage && !Heap[0].flage){
            Heap[ptr].v =Cnt[Heap[0].v].v + R.v;
            Heap[ptr].flage = true;
            Heap[ptr+1].v = ptr;
            Cnt[Heap[0].v].v = ptr;
        }
        if(!R.flage && !Heap[0].flage){
            Heap[ptr].v =Cnt[Heap[0].v].v + Cnt[R.v].v;
            Heap[ptr].flage = true;
            Cnt[R.v].v = ptr;
            Cnt[Heap[0].v].v = ptr;
        }
        Heap[0] = Heap[ptr];
        Heap[ptr].v = ptr;
        ptr--;
    }//构造
    /*for(int i = 0;i < cou;i++){
        cout<<Heap[i].v<<" ";
    }
    cout<<endl;
    for(int i = 0;i < cou;i++){
        cout<<Cnt[i].v<<" ";
    }
    cout<<endl;*/
    node n[2*listsize];
    uint64_t codes[listsize];
    uint32_t csizes[listsize];
    uint64_t m[2] = {0,0},couu = 0;
    int num = 1;
    uint64_t child[2] = {0,0},sit[2] = {0,0};
    for(int i = cou-1;i > 0;i--){
        //cout<<num<<endl;
        int x = 0;
        for(int j = 0;j < cou;j++){
            if(i == Cnt[j].v){
                child[x] = fretable[j].counter;
                sit[x] = j+1;
                x++;
            }
        }
        //cout<<x<<endl;
        if(x == 2){
            n[num].leftchild = 0;
            n[num].code = 0x0;
            n[num].rightchild = 0;
            n[num].site = sit[0];
            n[num].codesize = 0;
            n[num+1].leftchild = 0;
            n[num+1].rightchild = 0;
            n[num+1].code = 0x0;
            n[num+1].site = sit[1];
            n[num+1].codesize = 0;
            n[num+2].site = child[0] + child[1];
            n[num+2].code = 0x0;
            n[num+2].codesize = 0;
            if(child[0] > child[1]){
                n[num+2].leftchild = num+1;
                n[num+2].rightchild = num;
            }
            else{
                n[num+2].leftchild = num;
                n[num+2].rightchild = num+1;
            }
            num = num+3;
            m[couu] = num -1;
            couu++;
        }
        if(x == 1){
            n[num].leftchild = 0;
            n[num].rightchild = 0;
            n[num].code = 0x0;
            n[num].site = sit[0];
            n[num].codesize = 0;
            n[num+1].code = 0x0;
            n[num+1].codesize = 0;
            if(couu == 2){
                if(n[m[0]].site > n[m[1]].site && child[0] >= n[m[1]].site){
                    n[num+1].leftchild = m[1];
                    n[num+1].rightchild = num;
                    n[num+1].site = n[m[1]].site + child[0];
                    n[m[1]].site = 0;
                    m[1] = num+1;
                    num +=2;
                }
                if(n[m[0]].site > n[m[1]].site && child[0] < n[m[1]].site){
                    n[num+1].rightchild = m[1];
                    n[num+1].leftchild = num;
                    n[num+1].site = n[m[1]].site + child[0];
                    n[m[1]].site = 0;
                    m[1] = num+1;
                    num +=2;
                }
                if(n[m[0]].site <= n[m[1]].site && child[0] < n[m[0]].site){
                    n[num+1].rightchild = m[0];
                    n[num+1].leftchild = num;
                    n[num+1].site = n[m[0]].site + child[0];
                    n[m[0]].site = 0;
                    m[0] = num+1;
                    num +=2;
                }
                if(n[m[0]].site <= n[m[1]].site && child[0] >= n[m[0]].site){
                    n[num+1].leftchild = m[0];
                    n[num+1].rightchild = num;
                    n[num+1].site = n[m[1]].site + child[0];
                    n[m[0]].site = 0;
                    m[0] = num+1;
                    num +=2;
                }
            }
            if(couu == 1){
                if(child[0] < n[m[0]].site){
                    n[num+1].rightchild = m[0];
                    n[num+1].leftchild = num;
                    n[num+1].site = n[m[0]].site + child[0];
                    n[m[0]].site = 0;
                    m[0] = num+1;
                    num +=2;
                }
                else{
                    n[num+1].leftchild = m[0];
                    n[num+1].rightchild = num;
                    n[num+1].site = n[m[0]].site + child[0];
                    n[m[0]].site = 0;
                    m[0] = num+1;
                    num +=2;
                }
            }
            if(couu == 0)
                cout<<"wrong with creat code"<<endl;
        }
        if(x == 0){
            n[num].code = 0x0;
            n[num].codesize = 0;
            if(n[m[0]].site <= n[m[1]].site){
                n[num].leftchild = m[0];
                n[num].rightchild = m[1];
                n[num].site = n[m[1]].site + n[m[0]].site;
                n[m[0]].site = 0;
                n[m[1]].site = 0;
                couu--;
                m[0] = num;
                num++;
            }
            else{
                n[num].leftchild = m[1];
                n[num].rightchild = m[0];
                n[num].site = n[m[1]].site + n[m[0]].site;
                n[m[0]].site = 0;
                n[m[1]].site = 0;
                couu--;
                m[0] = num;
                num++;
            }
        }
    }
    int number = num;
    n[num-1].code = 0x0;
    n[num-1].codesize = 0;
    //n[num-1].parent = 0;
    n[num-1].site = 0;
    while(num > 0){
        num--;
        if(n[num].codesize <= 19){
            n[n[num].leftchild].code = n[num].code << 1;
            n[n[num].leftchild].codesize = n[num].codesize + 1;
            n[n[num].rightchild].code = (n[num].code << 1) + 0x1;
            n[n[num].rightchild].codesize = n[num].codesize + 1;
        }
        else{
            n[n[num].leftchild].code = n[num].code;
            n[n[num].leftchild].codesize = 20;
            n[n[num].rightchild].code = n[num].code ;
            n[n[num].rightchild].codesize = 20;
        }
    }
    while(number > 0){
        number--;
        if(n[number].site != 0){
            //cout<<n[number].code<<" "<<n[number].codesize<<endl;
            codes[n[number].site - 1] = n[number].code;
            csizes[n[number].site - 1] = n[number].codesize;
        }
    }
    number = 0;
    for(int i = 0;i < cou;i++){
        if(csizes[i] < 20){
            lookup[number].value = fretable[i].value;
            lookup[number].cw = codes[i];
            lookup[number].cL = csizes[i];
            /*cout<<lookup[number].value<<" "<<lookup[number].cw<<" "<<lookup[number].cL<<endl;*/
            number++;
        }
    }
    return number;
}

uint64_t SC2(LUT *lookup,uint64_t *values){
    uint64_t words[16];
    uint64_t wordPos[16];
    uint64_t comSize = 0;
    int j=0;
    for(int i = 0;i < 16;i++){
        for(; j < cou;j++){
            if(values[i] == lookup[j].value){
                words[i] = lookup[j].cw;
                wordPos[i] = lookup[j].cL;
                break;
            }
        }
        if(j == cou){
            cout<<"can't compress"<<endl;
            words[i] = values[i];
            wordPos[i] = 32;
        }
        else{
            if(wordPos[i] > 19){
                words[i] = values[i];
                wordPos[i] = 32;
            }
        }
        comSize += wordPos[i];
    }
    rat(values,words,wordPos);
    change(values,words,wordPos);
    return comSize;
}//没实现：动态改变code


int main(int argc, const char * argv[]) {
    VFT fretable[listsize];
    LUT lookup[listsize];
    uint64_t address[64],adr[16],addr;
    ifstream infile,opfile;
    infile.open("/Users/jinkailun/Desktop/压缩算法/BDI/BDI/file.txt");
    opfile.open("/Users/jinkailun/Desktop/bitfilp/operation.txt");
    outfile.open("/Users/jinkailun/Desktop/压缩算法/BDI/BDI/outfile.txt");
    float comratiototal = 0,comratiomoni = 0;
    uint64_t comSize;
    uint64_t operat = 0,values[16],va[64];
    string oper;
    uint64_t THl = 1,THh=1;
    int i = 0,count = 0,samcount = 0,monit = 0;
    bool tranencode = false,creathuffe = false,activated = false,compreac = true;
    while(!infile.eof()){
        if(i%65==0) {infile>>addr;
            outfile << addr << endl;
        }
        else{
            infile >> va[i-1];
            address[i] = addr + i-1;
        }
        if(i > times && compreac){
            activated = true;
        }
        if((i == 64 && count < times)||(tranencode && i == 64)){
            opfile >> oper;
            if(oper == "fetch")  operat = 0;
            if(oper == "update")  operat = 1;
            if(oper == "eviction")  operat = 2;
            for(int j = 0;j < 64;j += 4){
                adr[j/4] = address[j];
                for(int k = 0;k < 4;k++){
                    values[j/4] += (uint64_t)(va[k+j] << (8*(3-k)));
                    }
                }
            sampling(fretable, operat, values,adr);
           // cout<<2<<" "<<cou<<endl;
           /* for(int l = 0;l < cou;l++){
                cout<<fretable[l].value<<" "<<fretable[l].counter<<endl;
            }*/
            samcount++;
            if(samcount == times/65) {
                creathuffe = true;
            }
        }
        if(creathuffe){
            /*cout<<cou<<endl;
            for(int l = 0;l < cou;l++){
                cout<<fretable[l].value<<" "<<fretable[l].counter<<endl;
            }*/
            cout<<"creat huffman tree"<<endl;
            tranencode = false;
            creathuffe = false;
            cou = huffman(fretable,lookup);
            /*for(int l = 0;l < listsize ;l++){
                cout<<lookup[l].value<<" "<<lookup[l].cw<<" "<<lookup[l].cL;
                cout<<endl;
            }*/
            activated = true;
        }
        if(i == 64 && activated && count > times){
            cout<<"begin compress"<<endl;
            for(int j = 0;j < 64;j += 4){
                for(int k = 0;k < 4;k++){
                    values[j/4] += (uint64_t)(va[k+j] << (8*(3-k)));
                    }
                //cout<<values[j/4]<<endl;
                }
            comSize = SC2(lookup, values);
            comratiototal += (float)comSize /512;
            comratiomoni += (float)comSize /512;
        }
        if(i == 64 && !activated && count > times){
            cout<<"stop compress"<<endl;
            for(int j = 0;j < 64;j += 4){
                for(int k = 0;k < 4;k++){
                    values[j/4] += (uint64_t)(va[k+j] << (8*(3-k)));
                    }
                }
            uint64_t words[16];
            uint64_t wordPos[16];
            for(int j=0;j < 16;j++){
                words[i] = values[i];
                wordPos[i] = 32;
            }
            rat(values,words,wordPos);
            change(values,words,wordPos);
            comratiototal += 1;
            comratiomoni += 1;
        }
        if((monit%monitorp) == (monitorp -1)){
            cout<<"deal monitor"<<endl;
            if((1 / (comratiomoni / (count/64))) < THl){//停用压缩
                cout<<"lower than thl"<<endl;
                compreac=false;
            }
            else if((1/(comratiomoni / (count/64))) < THh){
                cout<<"lower than thh"<<endl;
                tranencode = true;
            }
            comratiomoni = 0;
        }
        i++;count++;
        monit++;
        i=i%65;
    }
    /*if(i != 0){
        for(;i < 64;i++)
            va[i] = 0;
        for(int j = 0;j < 64;j += 4){
            for(int k = 0;k < 4;k++){
                values[j/4] += (uint64_t)(va[k+j] << (8*(3-k)));
                }
            }
        if(compreac){
            comSize=SC2(lookup, values, cou);
            comratiototal += (float)comSize/512;
        }
        else{
            uint64_t words[16];
            uint64_t wordPos[16];
            for(int j=0;j < 16;j++){
                words[i] = values[i];
                wordPos[i] = 32;
            }
            rat(values,words,wordPos);
            change(values,words,wordPos);
            comratiototal += 1;
            comratiomoni += 1;
        }
        comratiototal = comratiototal / ((count/64)+1);
    }
    else*/
        comratiototal = comratiototal / (count/64);
    // insert code here...
    std::cout << "压缩率为  "<<comratiototal<<endl;
    cout<<"压缩之后的0的个数和1的个数 "<<zero_c<<"  "<<one_c<<endl;
    cout<<"由0变成1的个数为 "<<zero_to_one<<endl;
    std::cout << "Hello, World!\n";
    infile.close();
    opfile.close();
    return 0;
}
