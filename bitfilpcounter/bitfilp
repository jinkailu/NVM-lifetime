//
//  main.cpp
//  bitfilp
//
//  Created by 金凯伦 on 2021/7/30.
//

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <string>
#include <cmath>
using namespace std;

struct cache{
    uint64_t addr;
    string compressbefor;
    string compressafter;
    int noslc[512];
    int nomlc[256];
    int notlc[171];
    int cslc[512];
    int cmlc[256];
    int ctlc[171];
    struct cache *next;
};

int main(){
    ifstream infile,ifile;
    infile.open("/Users/jinkailun/Desktop/压缩算法/FPC/FPC/outfile.txt");
    ifile.open("/Users/jinkailun/Desktop/rw.out");
    cache ca;
    cache *head=NULL;
    cache *p=NULL ,*pr = head;
    string a;
    int flipnumnoc=0,flipnumc=0,numberall = 0;
    /*while(!infile.eof()){
        infile >> ca.addr;
        infile >> ca.compressbefor;
        infile >> ca.compressafter;
        cout<<ca.addr<<endl;
        cout<<ca.compressbefor.length()<<endl;
        cout<<ca.compressafter.length()<<endl;
    }*/
    while(numberall < 1000){
        ifile >> a;
        infile >> ca.addr;
        infile >> ca.compressbefor;
        infile >> ca.compressafter;
        pr=head;
        while(pr!=NULL){
            //cout<<pr->addr<< " "<<ca.addr<<" ";
            //cout<< a<<endl;
            if(pr->addr == ca.addr && a == "W"){
                //cout<<1<<endl;
                for(int i = 0;i < ca.compressbefor.length();i++){
                    if(ca.compressbefor[i] != pr->compressbefor[i]){
                        flipnumnoc++;
                        pr->noslc[i]++;
                        pr->nomlc[i/2]++;
                        pr->notlc[i/3]++;
                    }
                }
                for(int i = 0;i < ca.compressafter.length();i++){
                    if(ca.compressafter[i] != pr->compressafter[i]){
                        flipnumc++;
                        pr->cslc[i]++;
                        pr->cmlc[i/2]++;
                        pr->ctlc[i/3]++;
                    }
                }
                pr->compressbefor = ca.compressbefor;
                pr->compressafter = ca.compressafter;
                break;
            }
            pr = pr->next;
        }
        if(head == NULL){
            head = new cache;
            head->addr = ca.addr;
            if(a == "W") head->compressafter=ca.compressafter;
            else head->compressafter=ca.compressbefor;
            head->compressbefor=ca.compressbefor;
            head->next=NULL;
            for(int i = 0;i < 512;i++){
                head->noslc[i]=0;
                head->cslc[i]=0;
            }
            for(int i = 0;i < 256;i++){
                head->nomlc[i]=0;
                head->cmlc[i]=0;
            }
            for(int i = 0;i < 171;i++){
                head->notlc[i]=0;
                head->ctlc[i]=0;
            }
        }
        else if(pr == NULL){
            p = new cache;
            p->addr = ca.addr;
            if(a == "W") p->compressafter=ca.compressafter;
            else p->compressafter=ca.compressbefor;
            p->compressbefor = ca.compressbefor;
            for(int i = 0;i < 512;i++){
                p->noslc[i]=0;
                p->cslc[i]=0;
            }
            for(int i = 0;i < 256;i++){
                p->nomlc[i]=0;
                p->cmlc[i]=0;
            }
            for(int i = 0;i < 171;i++){
                p->notlc[i]=0;
                p->ctlc[i]=0;
            }
            p->next = head->next;
            head->next=p;
        }
        numberall++;
    }
    cout<<"压缩前比特翻转"<<flipnumnoc<<endl;
    cout<<"压缩后比特翻转"<<flipnumc<<endl;
    pr=head;
    int totalwritenoslc=0,totalwritecslc=0;
    int totalwritenomlc=0,totalwritecmlc=0;
    int totalwritenotlc=0,totalwritectlc=0;
    while(pr!=NULL){
        cout<<"压缩前slc单元中每个单元的写入次数："<<endl;
        for(int i = 0;i < 512;i++){
            totalwritenoslc += pr->noslc[i];
           cout<< pr->noslc[i]<<" ";
        }
        cout<<endl;
        cout<<"压缩后slc单元中每个单元的写入次数："<<endl;
        for(int i = 0;i < 512;i++){
            totalwritecslc += pr->cslc[i];
            cout<<pr->cslc[i]<<" ";
        }
        cout<<endl;
        cout<<"压缩前mlc单元中每个单元的写入次数："<<endl;
        for(int i = 0;i < 256;i++){
            totalwritenomlc += pr->nomlc[i];
           cout<< pr->nomlc[i]<<" ";
        }
        cout<<endl;
        cout<<"压缩后mlc单元中每个单元的写入次数："<<endl;
        for(int i = 0;i < 256;i++){
            totalwritecmlc += pr->cmlc[i];
            cout<<pr->cmlc[i]<<" ";
        }
        cout<<endl;
        cout<<"压缩前tlc单元中每个单元的写入次数："<<endl;
        for(int i = 0;i < 171;i++){
            totalwritenotlc += pr->notlc[i];
           cout<< pr->notlc[i]<<" ";
        }
        cout<<endl;
        cout<<"压缩后tlc单元中每个单元的写入次数："<<endl;
        for(int i = 0;i < 171;i++){
            totalwritectlc += pr->ctlc[i];
            cout<<pr->ctlc[i]<<" ";
        }
        cout<<endl;
        pr=pr->next;
    }
    cout<<"slc单元写入次数"<<totalwritecslc<<endl;
    cout<<"mlc单元写入次数"<<totalwritecmlc<<endl;
    cout<<"tlc单元写入次数"<<totalwritectlc<<endl;
}


