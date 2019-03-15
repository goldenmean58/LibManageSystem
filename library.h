#ifndef LIBRARY_H
#define LIBRARY_H

#include<list>
#include "BTree.h"
#include "BTree.cpp"
//#include "btree/btree_map.h"
#include <string.h>
#include<iostream>
using std::cin;
using std::cout;
using std::endl;
using std::list;
//using btree::btree_map;
#define MAX_BOOK_NUM 100
#define MAX_AUTH_NUM 100
#define MAX_BORROW_DAY 30
int getNowTime(){
    static int nowTime;
    cout<<"Time was "<<nowTime<<endl;
    cout<<"Now is ";
    cin>>nowTime;
    return nowTime;
}
class Author;
class LoanInfo;
class ReserveInfo;
class Book;
class Reader;
#define REMAINDER 97
#define KEYLEN 20

template<class T>
struct Node {
	char key[KEYLEN + 1];
	T value;
	Node *next;
};

template<class T>
class HashTable {
public:
	struct Node<T> nodes[REMAINDER];
	HashTable() {
		for (int i = 0; i < REMAINDER; i++) {
			memset(&nodes[i].key, 0, KEYLEN + 1);
			nodes[i].next = NULL;
	    }
    } 
    void put(const char *key, T value) {
		unsigned int index = Time33(key) % REMAINDER;
		Node<T> *p = &nodes[index];
		Node<T> *pre;
		while (p) {
			if (strcmp(p->key, key) == 0) {
				p->value = value;
				return;
			}
			pre = p;
			p = p->next;
		}
		pre->next = new Node<T>();
		p = pre->next;
		strcpy(p->key, key);
		p->value = value;
		p->next = NULL;
	}
	void remove(const char *key) {
		unsigned int index = Time33(key) % REMAINDER;
		Node<T> *p = &nodes[index];
		Node<T> *pre;
		while (p) {
			if (strcmp(p->key, key) == 0) {
				pre->next = p->next;
				delete p;
				return;
			}
			pre = p;
			p = p->next;
		}
	}
	T get(const char *key) {
		unsigned int index = Time33(key) % REMAINDER;
		Node<T> *p = &nodes[index];
		Node<T> *pre;
		while (p) {
			if (strcmp(p->key, key) == 0) {
				return p->value;
			}
			pre = p;
			p = p->next;
		}
		return NULL;
	}
private:
	static unsigned int Time33(const char *str) {	//hash function
		int len = strlen(str);
		unsigned int hash = 0;
		for (int i = 0; i < len; i++) {
			hash += (hash << 5) + (int)str[i];
		}
		return (hash & 0x7FFFFFFF);	//取int长度
	}
};

class Author{
public:
	char name[10];
	list<Book *> bl; //相关著作
	Author(const char *name){
		strcpy(this->name,name);
	}
};

class LoanInfo{
public:
	Reader *reader;
	int lendDay;
    LoanInfo(Reader* reader, int lendDay):reader(reader), lendDay(lendDay){}
};


class ReserveInfo{
public:
    Reader *reader;
    int lendDay;
    ReserveInfo(Reader* reader, int lendDay):reader(reader), lendDay(lendDay){}
};

class Book{
public:
	int id;
	char name[20];
	Author *auth;
    static HashTable<Author*> ht;
	int currentNum;
	int totalNum;
	list<LoanInfo> loanInfo;
    list<ReserveInfo> reseverInfo;
    Book(){

    }
	Book(int id, const char *name, const char *authName, 
			int importNum):id(id),currentNum(importNum),totalNum(importNum){
		strcpy(this->name,name);
		auth=ht.get(authName);
		if(auth==NULL){
			auth=new Author(authName);
            ht.put(authName,auth);
		}
        auth->bl.push_back(this);
	}
    void addCurrentNum(int num){
        currentNum+=num;
        checkReserveInfo();
    }
    void addTotalNum(int num){
        totalNum+=num;
        //checkReserveInfo();
    }
    void checkReserveInfo(){ //预约相关
        std::list<ReserveInfo>::iterator ri=this->reseverInfo.begin();
        std::list<ReserveInfo>::iterator pre;
        while(ri!=this->reseverInfo.end()){
            if(this->currentNum>0){
                this->currentNum--;
                LoanInfo *newLoanInfo=new LoanInfo(ri->reader,ri->lendDay); //TODO: lendDay?
                this->loanInfo.push_back(*newLoanInfo);
                pre=ri;
                pre++;
                this->reseverInfo.erase(ri);
                ri=pre;
                continue;
            }else{
                break;
            }
            ri++;
        }
    }
    bool operator==(const Book & b)const{
        return this->id==b.id;
    }
    bool operator< (const Book &b)const{
        return this->id<b.id;
    }
    bool operator<= (const Book &b)const{
        return this->id<b.id;
    }
    bool operator>= (const Book &b)const{
        return this->id>=b.id;
    }
    bool operator> (const Book &b)const{
        return this->id>b.id;
    }
    bool operator==(int id)const{
        return this->id==id;
    }
    bool operator< (int id)const{
        return this->id<id;
    }
    bool operator<= (int id)const{
        return this->id<id;
    }
    bool operator>= (int id)const{
        return this->id>=id;
    }
    bool operator> (int id)const{
        return this->id>id;
    }
    friend ostream &operator<<(ostream &output, const Book &book){
        output << book.id;
        return output;
    }
};

typedef BTree<Book> MyBTree;

class Reader{
public:
	int id;
    bool borrow(Book* book){
        if(book->currentNum>0){
            book->addCurrentNum(-1);
            LoanInfo *newLoanInfo=new LoanInfo(this,getNowTime());
            book->loanInfo.push_back(*newLoanInfo);
            return true;
        }else{
            return false;
        }
    }
	int returnBook(Book* returnedBook){ //返回:-1并未借阅,其他逾期罚款金额
        if(returnedBook==NULL) return -1;
        int fine=0;
        std::list<LoanInfo>::iterator it=returnedBook->loanInfo.begin();
        while(it!=returnedBook->loanInfo.end()){
            if(this==it->reader){
                int now=getNowTime();
                fine=now-it->lendDay-MAX_BORROW_DAY; //最长借阅时间
                fine=(fine<0)?0:fine*2;
                returnedBook->addCurrentNum(1);
                returnedBook->loanInfo.erase(it);
                return fine;
            }
            it++;
        }
        return -1;
	}
	bool reserveBook(Book* book){
        ReserveInfo *newReserveInfo=new ReserveInfo(this,getNowTime());
        book->reseverInfo.push_back(*newReserveInfo);
        return true;
	}
	Reader(int id):id(id){

	}
};


class Library{
public:
	int readerNum; //读者数量
	int totalBookNum; //全馆图书数量
	int difBookNum; //全馆不相同图书数量
	static MyBTree *btree;
    list<Reader *> reader;
	Book *findBookById(int id){ //树中查找Book NULL表示不存在
		return btree->find(btree->getRoot(),id);
	}
    Reader *findReaderById(int id){
        for(auto r : reader){
            if(r->id==id) return r;
        }
        return NULL;
    }
    Reader* addReader(int id){
        if(!findReaderById(id)){
            Reader *r=new Reader(id);
            reader.push_back(r);
            return r;
        }
        return NULL;
    }
	Library():readerNum(0),totalBookNum(0),difBookNum(0){
		btree = new MyBTree(4);
	}
	~Library(){
		delete btree;
	}
	void importBook(Book* importedBook, int num){ //已有图书入库
		//importedBook应该是树中已经有的图书的指针
		//num为新购入几本
		if(importedBook==NULL) return;
        importedBook->addTotalNum(num);
        importedBook->addCurrentNum(num);
	}
	void importBook(int id, const char *name, const char *authName, int num){
		Book *book = findBookById(id);
		if(book){
			importBook(book,num);
            return;
        }
	    book = new Book(id,name,authName,num);
        btree->insert(*book);
	}
	bool removeBook(int id){ //清除库存 完全删除
		Book *book = findBookById(id);
        if(book){
            btree->erase(*book);
            return true;
        }else{
            return false;
        }
	}
	void showTree(){

	}
};

#endif
