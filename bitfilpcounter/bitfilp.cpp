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
    uint8_t noslc[512];
    uint8_t nomlc[256];
    uint8_t notlc[171];
    uint8_t cslc[512];
    uint8_t cmlc[256];
    uint8_t ctlc[171];
    struct cache *next;
};
struct cccc{
    uint64_t addr;
    string compressbefor;
    string compressafter;
};
int main(){
    ifstream infile,ifile;
    ofstream outfile;
    infile.open("/Users/jinkailun/Desktop/BDI/BDI/outfile_all.txt");
    ifile.open("/Users/jinkailun/Desktop/rw.out");
    outfile.open("/Users/jinkailun/Desktop/bitflipcount/bdi_all.txt");
    cccc ca;
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
    while(!infile.eof()){
    //while(numberall < 100){
        ifile >> a;
        infile >> ca.addr;
        infile >> ca.compressbefor;
        infile >> ca.compressafter;
        pr=head;
        while(pr!=NULL){
            //cout<<pr->addr<< " "<<ca.addr<<" ";
            //cout<< a<<endl;
            if(pr->addr == ca.addr){
                //cout<<1<<endl;
                if( a == "W"){
                for(int i = 0;i < ca.compressbefor.length();i++){
                    if(ca.compressbefor[i] != pr->compressbefor[i]){
                        flipnumnoc++;
                        pr->noslc[i]++;
                    }
                    if(ca.compressafter[i] != pr->compressafter[i]){
                        flipnumc++;
                        pr->cslc[i]++;
                    }
                }
                for(int i = 0;i < ca.compressbefor.length();i=i+2){
                    if(ca.compressbefor[i] != pr->compressbefor[i] || ca.compressbefor[i+1] != pr->compressbefor[i+1]){
                        pr->nomlc[i/2]++;
                    }
                    if(ca.compressafter[i] != pr->compressafter[i]|| ca.compressbefor[i+1] != pr->compressbefor[i+1]){
                        pr->cmlc[i/2]++;
                    }
                }
                for(int i = 0;i < ca.compressbefor.length();i=i+3){
                    if(ca.compressbefor[i] != pr->compressbefor[i] || ca.compressbefor[i+1] != pr->compressbefor[i+1]|| ca.compressbefor[i+2] != pr->compressbefor[i+2]){
                        pr->notlc[i/3]++;
                    }
                    if(ca.compressafter[i] != pr->compressafter[i]||ca.compressbefor[i+1] != pr->compressbefor[i+1]|| ca.compressbefor[i+2] != pr->compressbefor[i+2]){
                        pr->cmlc[i/3]++;
                    }
                }
                pr->compressbefor = ca.compressbefor;
                pr->compressafter = ca.compressafter;
                }
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
        cout<<numberall<<endl;
        numberall++;
    }
    outfile<<"压缩前比特翻转"<<flipnumnoc<<endl;
    outfile<<"压缩后比特翻转"<<flipnumc<<endl;
    pr=head;
    int totalwritenoslc=0,totalwritecslc=0;
    int totalwritenomlc=0,totalwritecmlc=0;
    int totalwritenotlc=0,totalwritectlc=0;
    while(pr!=NULL){
        outfile<<pr->addr<<endl;
        outfile<<"压缩前slc单元中每个单元的写入次数："<<endl;
        for(int i = 0;i < 512;i++){
            totalwritenoslc += (int)pr->noslc[i];
            outfile<< (int)pr->noslc[i]<<" ";
        }
        outfile<<endl;
        outfile<<"压缩后slc单元中每个单元的写入次数："<<endl;
        for(int i = 0;i < 512;i++){
            totalwritecslc += (int)pr->cslc[i];
            outfile<<(int)pr->cslc[i]<<" ";
        }
        outfile<<endl;
        outfile<<"压缩前mlc单元中每个单元的写入次数："<<endl;
        for(int i = 0;i < 256;i++){
            totalwritenomlc += (int)pr->nomlc[i];
            outfile<< (int)pr->nomlc[i]<<" ";
        }
        outfile<<endl;
        outfile<<"压缩后mlc单元中每个单元的写入次数："<<endl;
        for(int i = 0;i < 256;i++){
            totalwritecmlc += (int)pr->cmlc[i];
            outfile<<(int)pr->cmlc[i]<<" ";
        }
        outfile<<endl;
        outfile<<"压缩前tlc单元中每个单元的写入次数："<<endl;
        for(int i = 0;i < 171;i++){
            totalwritenotlc += (int)pr->notlc[i];
            outfile<< (int)pr->notlc[i]<<" ";
        }
        outfile<<endl;
        outfile<<"压缩后tlc单元中每个单元的写入次数："<<endl;
        for(int i = 0;i < 171;i++){
            totalwritectlc += (int)pr->ctlc[i];
            outfile<<(int)pr->ctlc[i]<<" ";
        }
        outfile<<endl;
        p=pr;
        pr=pr->next;
        free(p);
    }
    outfile<<"slc单元写入次数"<<totalwritecslc<<endl;
    outfile<<"mlc单元写入次数"<<totalwritecmlc<<endl;
    outfile<<"tlc单元写入次数"<<totalwritectlc<<endl;
}





