#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <string>
#include <cctype>
#include <ctime>
#include <conio.h>
#include <cstdlib>
using namespace std;

const int MAX=200;
const char DATA[]="medicine.dat",BILL[]="bill_history.txt";
const string USER="admin",PASS="admin123";

const char *TYPE[]={
"Syrup","Tablet","Capsule","Syringe","Injection","Drops","Cream",
"Ointment","Inhaler","Powder","Gel","Suppository","Lozenge","Spray"
};

struct Medicine{
    int code,qty;
    char name[40],type[30],company[30],batch[15],expiry[10];
    float price;
};

bool nameOK(string s){
    int i;
    if(s.empty()||!isalpha((unsigned char)s[0])) return false;
    for(i=0;i<(int)s.length();i++)
        if(!isalnum((unsigned char)s[i])&&s[i]!=' '&&s[i]!='-'&&s[i]!='.')
            return false;
    return true;
}

bool companyOK(string s){
    int i;
    if(s.empty()||!isalpha((unsigned char)s[0])) return false;
    for(i=0;i<(int)s.length();i++)
        if(!isalnum((unsigned char)s[i])&&s[i]!=' '&&s[i]!='.'&&
           s[i]!='-'&&s[i]!='&'&&s[i]!='\'')
            return false;
    return true;
}

bool textOK(string s){
    return !s.empty()&&s.find('\t')==string::npos;
}

bool expiryOK(string s){
    int i,m,y;
    time_t t;
    tm *n;

    if(s.length()!=7||s[2]!='/') return false;

    for(i=0;i<7;i++)
        if(i!=2&&!isdigit((unsigned char)s[i])) return false;

    m=(s[0]-'0')*10+(s[1]-'0');
    y=(s[3]-'0')*1000+(s[4]-'0')*100+
      (s[5]-'0')*10+(s[6]-'0');

    t=time(0);
    n=localtime(&t);

    return m>=1&&m<=12&&y>=2000&&y<=2100&&
    !(y<n->tm_year+1900||
      (y==n->tm_year+1900&&m<n->tm_mon+1));
}

bool priceOK(string s,float &p){
    int i,d=-1;

    for(i=0;i<(int)s.length();i++)
        if(s[i]=='.'){
            if(d!=-1) return false;
            d=i;
        }

    if(d<=0||d==(int)s.length()-1) return false;

    for(i=0;i<(int)s.length();i++)
        if(i!=d&&!isdigit((unsigned char)s[i])) return false;

    p=(float)atof(s.c_str());
    return p>0;
}

int inputInt(string msg){
    int n;
    while(true){
        cout<<msg;
        if(cin>>n&&n>=0){
            cin.ignore(10000,'\n');
            return n;
        }
        cin.clear();
        cin.ignore(10000,'\n');
        cout<<"Error: Enter a valid whole number.\n";
    }
}

string getPassword(){
    string p="";
    char ch;

    while((ch=getch())!=13){
        if(ch==8){
            if(!p.empty()){
                p.erase(p.length()-1);
                cout<<"\b \b";
            }
        }
        else{
            p+=ch;
            cout<<"*";
        }
    }

    cout<<endl;
    return p;
}

bool login(){
    int i;
    string u,p;

    for(i=0;i<3;i++){
        cout<<"\nUsername: ";
        cin>>u;
        cin.ignore(10000,'\n');

        cout<<"Password: ";
        p=getPassword();

        if(u==USER&&p==PASS){
            cout<<"Login successful!\n";
            return true;
        }

        cout<<"Error: Invalid username/password. "
            <<"Attempts left: "<<2-i<<"\n";
    }

    cout<<"Too many failed attempts.\n";
    return false;
}

class MedicalShop{
    Medicine s[MAX];
    int total;

    int byCode(int c){
        int i;
        for(i=0;i<total;i++)
            if(s[i].code==c) return i;
        return -1;
    }

    int byName(string n){
        int i,j;
        char a,b;

        for(i=0;i<(int)n.length();i++)
            n[i]=(char)tolower((unsigned char)n[i]);

        for(i=0;i<total;i++){
            string x=s[i].name;
            for(j=0;j<(int)x.length();j++)
                x[j]=(char)tolower((unsigned char)x[j]);

            if(x==n) return i;
        }
        return -1;
    }

    void copy(char *d,string x,int n){
        strncpy(d,x.c_str(),n-1);
        d[n-1]='\0';
    }

    void selectType(Medicine &m){
        int n;
        string x;

        cout<<"\n========== MEDICINE TYPES ==========\n";
        for(n=0;n<14;n++)
            cout<<n+1<<". "<<TYPE[n]<<endl;
        cout<<"15. Other\n";
        cout<<"====================================\n";

        do{
            n=inputInt("Enter type (1-15): ");
            if(n<1||n>15) cout<<"Error: Choose 1-15.\n";
        }while(n<1||n>15);

        if(n<15) x=TYPE[n-1];
        else{
            do{
                cout<<"Enter medicine type: ";
                getline(cin,x);
                if(!textOK(x))
                    cout<<"Error: Type cannot be empty.\n";
            }while(!textOK(x));
        }

        copy(m.type,x,30);
    }

public:
    MedicalShop(){
        total=0;
        load();
    }

    void load(){
        ifstream f(DATA,ios::binary);
        while(total<MAX&&f.read((char*)&s[total],sizeof(Medicine)))
            total++;
        f.close();
    }

    void save(){
        ofstream f(DATA,ios::binary|ios::trunc);
        int i;
        for(i=0;i<total;i++)
            f.write((char*)&s[i],sizeof(Medicine));
        f.close();
    }

    void add(){
        Medicine m;
        string x;
        float p;

        if(total>=MAX){
            cout<<"Error: Stock full!\n";
            return;
        }

        cout<<"\n========== ADD MEDICINE ==========\n";
        selectType(m);

        do{
            m.code=inputInt("Medicine Code : ");
            if(byCode(m.code)>=0)
                cout<<"Error: Code already exists.\n";
        }while(byCode(m.code)>=0);

        do{
            cout<<"Medicine Name : ";
            getline(cin,x);
            if(!nameOK(x))
                cout<<"Error: Invalid medicine name.\n";
        }while(!nameOK(x));
        copy(m.name,x,40);

        do{
            cout<<"Company       : ";
            getline(cin,x);
            if(!companyOK(x))
                cout<<"Error: Invalid company name.\n";
        }while(!companyOK(x));
        copy(m.company,x,30);

        do{
            cout<<"Batch         : ";
            getline(cin,x);
            if(!textOK(x))
                cout<<"Error: Batch cannot be empty.\n";
        }while(!textOK(x));
        copy(m.batch,x,15);

        do{
            cout<<"Expiry(MM/YYYY): ";
            getline(cin,x);
            if(!expiryOK(x))
                cout<<"Error: Invalid or expired date.\n";
        }while(!expiryOK(x));
        copy(m.expiry,x,10);

        do{
            cout<<"Price         : ";
            getline(cin,x);
            if(!priceOK(x,p))
                cout<<"Error: Enter price like 25.50.\n";
        }while(!priceOK(x,p));

        m.price=p;
        m.qty=inputInt("Quantity      : ");

        s[total++]=m;
        save();
        cout<<"Medicine added successfully!\n";
    }

    void display(){
        int i;

        if(!total){
            cout<<"No records found!\n";
            return;
        }

        cout<<"\n"<<left
            <<setw(6)<<"Code"<<setw(20)<<"Name"
            <<setw(15)<<"Type"<<setw(15)<<"Company"
            <<setw(10)<<"Batch"<<setw(10)<<"Expiry"
            <<right<<setw(10)<<"Price"<<setw(8)<<"Qty"<<endl;

        cout<<string(104,'-')<<endl;

        for(i=0;i<total;i++)
            cout<<left
                <<setw(6)<<s[i].code
                <<setw(20)<<s[i].name
                <<setw(15)<<s[i].type
                <<setw(15)<<s[i].company
                <<setw(10)<<s[i].batch
                <<setw(10)<<s[i].expiry
                <<right<<setw(10)<<fixed<<setprecision(2)<<s[i].price
                <<setw(8)<<s[i].qty<<endl;
    }

    void search(){
        int c,i=-1;
        string n;

        c=inputInt("\n1.Code  2.Name : ");

        if(c==1)
            i=byCode(inputInt("Code: "));
        else if(c==2){
            cout<<"Name: ";
            getline(cin,n);
            i=byName(n);
        }
        else{
            cout<<"Error: Choose 1 or 2.\n";
            return;
        }

        if(i<0){
            cout<<"Error: Medicine not found.\n";
            return;
        }

        cout<<"\n========== MEDICINE DETAILS ==========\n"
            <<"Code: "<<s[i].code
            <<"\nName: "<<s[i].name
            <<"\nType: "<<s[i].type
            <<"\nCompany: "<<s[i].company
            <<"\nBatch: "<<s[i].batch
            <<"\nExpiry: "<<s[i].expiry
            <<"\nPrice: Rs."<<fixed<<setprecision(2)<<s[i].price
            <<"\nQuantity: "<<s[i].qty<<endl;
    }

    void update(){
        int i,c;
        string x;
        float p;

        i=byCode(inputInt("\nMedicine Code: "));

        if(i<0){
            cout<<"Error: Medicine not found.\n";
            return;
        }

        c=inputInt("1.Price 2.Quantity 3.Expiry 4.Type : ");

        if(c==1){
            do{
                cout<<"New Price: ";
                getline(cin,x);
                if(!priceOK(x,p))
                    cout<<"Error: Invalid price.\n";
            }while(!priceOK(x,p));
            s[i].price=p;
        }
        else if(c==2)
            s[i].qty=inputInt("New Quantity: ");
        else if(c==3){
            do{
                cout<<"New Expiry: ";
                getline(cin,x);
                if(!expiryOK(x))
                    cout<<"Error: Invalid expiry.\n";
            }while(!expiryOK(x));
            copy(s[i].expiry,x,10);
        }
        else if(c==4)
            selectType(s[i]);
        else{
            cout<<"Error: Choose 1-4.\n";
            return;
        }

        save();
        cout<<"Updated successfully!\n";
    }

    void remove(){
        int i=byCode(inputInt("\nMedicine Code: "));

        if(i<0){
            cout<<"Error: Medicine not found.\n";
            return;
        }

        for(;i<total-1;i++)
            s[i]=s[i+1];

        total--;
        save();
        cout<<"Deleted successfully!\n";
    }

    void sell(){
        int i,q;
        float amount;
        ofstream f;

        i=byCode(inputInt("\nMedicine Code: "));

        if(i<0){
            cout<<"Error: Medicine not found.\n";
            return;
        }

        cout<<"Medicine: "<<s[i].name
            <<" | Type: "<<s[i].type
            <<" | Available: "<<s[i].qty<<endl;

        q=inputInt("Quantity: ");

        if(q==0||q>s[i].qty){
            cout<<"Error: Invalid quantity/insufficient stock.\n";
            return;
        }

        amount=q*s[i].price;
        s[i].qty-=q;
        save();

        cout<<"\n========== BILL ==========\n"
            <<"Medicine: "<<s[i].name
            <<"\nType: "<<s[i].type
            <<"\nQuantity: "<<q
            <<"\nAmount: Rs."<<fixed<<setprecision(2)<<amount
            <<"\n==========================\n";

        f.open(BILL,ios::app);
        f<<"Medicine: "<<s[i].name
         <<" | Type: "<<s[i].type
         <<" | Qty: "<<q
         <<" | Amount: Rs."<<amount<<endl;
        f.close();
    }
};

int main(){
    MedicalShop shop;
    int c;

    cout<<"====================================\n"
        <<"    MEDICAL SHOP MANAGEMENT SYSTEM\n"
        <<"====================================\n";

    if(!login()) return 0;

    do{
        cout<<"\n1. Add Medicine\n"
            <<"2. Display All Medicines\n"
            <<"3. Search Medicine\n"
            <<"4. Update Medicine\n"
            <<"5. Delete Medicine\n"
            <<"6. Sell Medicine / Billing\n"
            <<"7. Exit\n";

        c=inputInt("Enter choice: ");

        switch(c){
            case 1: shop.add(); break;
            case 2: shop.display(); break;
            case 3: shop.search(); break;
            case 4: shop.update(); break;
            case 5: shop.remove(); break;
            case 6: shop.sell(); break;
            case 7: cout<<"Thank you for using Medical Shop Management System!\n"; break;
            default: cout<<"Error: Choose 1-7.\n";
        }
    }while(c!=7);

    return 0;
}
