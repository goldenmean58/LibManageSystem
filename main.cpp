#include "library.h"
#include<iostream>
using std::cin;
using std::cout;
using std::endl;

Library lib;
HashTable<Author*> Book::ht;
BTree *Library::btree;

int printMenu(){
    int choice=0;
    cout<<"1. Import Books."<<endl;
    cout<<"2. Remove Books."<<endl;
    cout<<"3. Borrow Book."<<endl;
    cout<<"4. Return Book."<<endl;
    cout<<"5. Show Book's Status."<<endl;
    cout<<"6. Show Author's Works."<<endl;
    cout<<"7. Reserve Books."<<endl;
    cout<<"8. Exit System."<<endl;
    cout<<"Choice : ";
    cin>>choice;
    return choice;
}

void showBookReserveInfo(Book *book){
    if(!book){
        cout<<"No such a book!"<<endl;
        return;
    }
    cout<<"Reserve Info:"<<endl;
    for(auto ri : book->reseverInfo){
        cout<<"Reader id:"<<ri.reader->id<<'\t'<<"The day on reservation:"<<ri.lendDay<<endl;
    }
}

void showBookReserveInfo(int id){
    Book *book=lib.findBookById(id);
    if(book) showBookReserveInfo(book);
    else cout<<"No such a book!"<<endl;
}

void showBookLoanInfo(Book *book){
    if(!book){
        cout<<"No such a book!"<<endl;
        return;
    }
    cout<<"Loan Info:"<<endl;
    for(auto li : book->loanInfo){
        cout<<"Reader id:"<<li.reader->id<<'\t'<<"The day on loan:"<<li.lendDay<<endl;
    }
}

void showBookLoanInfo(int id){
    Book *book=lib.findBookById(id);
    if(book) showBookLoanInfo(book);
    else cout<<"No such a book!"<<endl;
}

void showBookBasicInfo(Book* book){
    if(book){
        cout<<"Book id:"<<book->id<<endl;
        cout<<"Book name:"<<book->name<<endl;
        cout<<"Book author:"<<book->auth->name<<endl;
        cout<<"Book standing stock:"<<book->currentNum<<endl;
        cout<<"Book total stock:"<<book->totalNum<<endl;
    }else cout<<"No such a book!"<<endl;
}

void showBookBasicInfo(int id){
    Book* book=lib.findBookById(id);
    if(book) showBookBasicInfo(book);
    else cout<<"No such a book!"<<endl;
}

void showBookInfo(Book *book){
    if(book) {
        showBookBasicInfo(book);
        showBookLoanInfo(book);
        showBookReserveInfo(book);
    }
    else cout<<"No such a book!"<<endl;
}

void showBookInfo(int id){
    Book *book=lib.findBookById(id);
    if(book) showBookInfo(book);
    else cout<<"No such a book!"<<endl;

}
int main()
{
    cout<<"Welcome to Library Manage System"<<endl;
    char bookName[20];
    char authName[10];
    int id;
    int num;
    int fine=0;
    Book* book=NULL;
    Reader* reader=NULL;
    Author* auth=NULL;
    int choice=0;
    int yesOrNo=0;
    while(1){
        choice=printMenu();
        switch(choice){
            case 1: //Import Books;
                cout<<"The book's id : ";
                cin>>id;
                cout<<"The book's name : ";
                cin>>bookName;
                cout<<"The author's name : ";
                cin>>authName;
                cout<<"The number of the book : ";
                cin>>num;
                lib.importBook(id,bookName,authName,num);
                cout<<"Import Book(s) successfully!";
                break;
            case 2: //Remove Book
                cout<<"The to be removed book's id : ";
                cin>>id;
                if(lib.removeBook(id))
                    cout<<"Remove the book successfully!"<<endl;
                else
                    cout<<"Failed to remove the book"<<endl;
                break;
            case 3: //Borrow Book
                cout<<"The book's id you want to borrow : ";
                cin>>id;
                book=lib.findBookById(id);
                if(book){
                    cout<<"Please enter your id to continue : ";
                    cin>>id;
                    reader=lib.findReaderById(id);
                    if(!reader){
                        reader=lib.addReader(id);
                    }
                    if(reader->borrow(book)){
                        cout<<"Borrowed successfully!"<<endl;
                    }else{
                        cout<<"Failed to borrow, because there is no book to borrow. Do you want to reserve one?(1 for y/0 for n)"<<endl;
                        cin>>yesOrNo;
                        if(yesOrNo==1){
                            reader->reserveBook(book);
                        }
                    }
                }
                break;
            case 4: //Return Book
                cout<<"Please enter your id : ";
                cin>>id;
                reader=lib.findReaderById(id);
                if(!reader){
                    cout<<"No such a reader, please check your id again!"<<endl;
                }else{
                    cout<<"Please enter the book's id : ";
                    cin>>id;
                    book=lib.findBookById(id);
                    fine=reader->returnBook(book);
                    if(fine==-1) cout<<"You didn't borrow that book, please check your id and the book's id again!"<<endl;
                    else cout<<"Returned book successfully, and you have to pay "<<fine<<" in fines!"<<endl;
                }
                break;
            case 5: //Show Book's Status
                cout<<"Please enter the book's id you want to check : ";
                cin>>id;
                showBookInfo(id);
                break;
            case 6: //Show Author's works
                cout<<"Please enter the author's name : ";
                cin>>authName;
                auth=Book::ht.get(authName);
                if(auth==NULL){
                    cout<<"No any information for that author!"<<endl;
                }else{
                    for(auto bookInfo:auth->bl){
                        showBookBasicInfo(bookInfo);
                    }
                }
                break;
            case 7: //Reserve Book
                cout<<"Please enter the book's id you want to reserve : ";
                cin>>id;
                book=lib.findBookById(id);
                if(book){
                    cout<<"Please enter your id : ";
                    cin>>id;
                    reader=lib.findReaderById(id);
                    if(!reader){
                        reader=lib.addReader(id);
                    }
                    if(reader->reserveBook(book)){
                        cout<<"Reversed that book successfully!"<<endl;
                    }else{
                        cout<<"Failed to reserve that book!"<<endl;
                    }
                }else{
                    cout<<"No such a book!"<<endl;
                }
                break;
            case 8:
                return 0;
                break;
            default:
                return 0;
        }
        cout<<endl;
    }
    return 0;
}
