/*
DSProject
T076 - Nathanael Juan Gauthama
PS: UNZIP file before compiling!
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#define SIZE_TP 10
#define SIZE_TU 10

struct Cred{
	char email[51];
	char user[11];
	char pass[11];
};

struct user{
	//BST
	struct Cred log;
	struct user *l, *r;
};

struct prod{
	char code[10];
	char prod[151];
	double price;
	int qty;
};

char TempProd[101][151];

struct prodHash{
	struct prod Product;
	struct prodHash *nx;
};

//head hash product
struct prodHash *ListProd[SIZE_TP];
//total product
int allProd=0;

struct myCart{
	//DLL
	struct prod cart;
	struct myCart *nx;
	struct myCart *prev;
};

struct cart{
	//DLL
	struct myCart *head;
	struct myCart *tail;
};

struct TransHead{
	char ID[7];
	char user[11];
	char Date[31];
	struct TransHead *nx;
	struct TransHead *prev;
};

struct TransList{
	//DLL
	struct TransHead *head;
	struct TransHead *tail;
};

struct TransItem{
	char TransID[7];
	char ProdID[7];
	int qty;
	struct TransItem *nx;
	struct TransItem *prev;
};

struct TransDet{
	struct TransItem *head;
	struct TransItem *tail;
}currTrans;

struct UserData{
	struct Cred Crd;
	struct cart Cart;
	struct TransList THead;
} currUser;

struct UserList{
	struct UserData user;
	struct UserList *nx;
};

//head hash user logged in
struct UserList *UserLogged[SIZE_TU];

//root list credential all user
struct user *Root_User=NULL;

struct TrID{
	char ID[7];
	struct TrID *nx;
	struct TrID *prev;
};

//check all transaction ID
struct TrID *HeadAllID=NULL;
struct TrID *TailAllID=NULL;

struct TrID *createID(char ID[]){
	struct TrID *NewID = (struct TrID *) malloc(sizeof(struct TrID));
	strcpy(NewID->ID, ID);
	NewID->nx=NULL;
	NewID->prev=NULL;
	return NewID;
}

void insertID(struct TrID *NewID){
	if(!HeadAllID){
		HeadAllID=TailAllID=NewID;
	} else{
		TailAllID->nx=NewID;
		NewID->prev=TailAllID;
		TailAllID=NewID;
		NewID->nx=NULL;
	}
}

void CleanTemp(){
	for(int i=0; i<101; i++){
		strcpy(TempProd[i], "");
	}
}

unsigned long DJB2_Hash(char *Key){
	unsigned long HKey = 5381;
	int c=0;
	while(c=*Key++){
		HKey = ((HKey<<5)+HKey)+c;
	}
	return HKey%SIZE_TP;
}

struct prod SearchNum(int num){
	printf("%s\n", TempProd[num]);
	int key = DJB2_Hash(TempProd[num]);
	if(ListProd[key]){
		struct prodHash *now = ListProd[key];
		while(now){
			if(strcmp(TempProd[num], now->Product.prod)==0){
				return now->Product;
			}
			now=now->nx;
		}
	}
}

struct myCart *CreateCart(int num, int qty){
	struct myCart *NewCart =(struct myCart *) malloc(sizeof(struct myCart));
	struct prod tmp = SearchNum(num);
	strcpy(NewCart->cart.prod,tmp.prod);
	strcpy(NewCart->cart.code, tmp.code);
	NewCart->cart.price = tmp.price;
	NewCart->cart.qty=qty;
	NewCart->nx=NULL;
	NewCart->prev=NULL;
	return NewCart;
}

void InsertCart(struct myCart *NewCart){
	if(!currUser.Cart.head){
		currUser.Cart.head=currUser.Cart.tail=NewCart;
		
	} else{
		currUser.Cart.tail->nx=NewCart;
		NewCart->prev=currUser.Cart.tail;
		currUser.Cart.tail=NewCart;
		currUser.Cart.tail->nx=NULL;
	}
	printf("%s added to cart!\n", NewCart->cart.prod);
	getchar();
}

int InputNum(int init, int end){
	int opt;
	do{
		printf("Enter product number[ %d - %d ]: ", init, end);
		scanf("%d", &opt); getchar();
	}while(opt<init||opt>end);
	return opt;
}

int InputQty(){
	int opt;
	do{
		printf("Enter product quantity[ > 0 ]: ");
		scanf("%d", &opt); getchar();
	}while(opt<=0);
	return opt;
}

struct myCart *LookDupItem(char que[]){
	if(currUser.Cart.head){
		struct myCart *now = currUser.Cart.head;
		while(now){
			if(strcmp(now->cart.prod, que)==0){
				return now;
			}
			now=now->nx;
		}	
	}
	return NULL;
}

void BuyProd(int init, int end, int method){
	int num = InputNum(init, end);
	//method==1 from view, 2 from search
	num-=1;
	if(method==1) num%=10;
	int qty = InputQty();
	struct myCart *check=LookDupItem(TempProd[num]);
	if(check!=NULL){
		check->cart.qty+=qty;
		printf("%d %s is added!\n", qty, check->cart.prod);
		getchar();
	} else InsertCart(CreateCart(num, qty));
}

void PrintHeader(){
	puts("=====================================================================================================");
	puts("|                                                 All Products                                      |");
	puts("=====================================================================================================");
	puts("|[No.]|       Product Id  |                        Product Name|                     Product Price  |");
	puts("=====================================================================================================");
}

struct prodHash *createProd(char code[], char prod[], double price){
	struct prodHash *NewProd = (struct prodHash *) malloc(sizeof(struct prodHash));
	strcpy(NewProd->Product.code, code);
	strcpy(NewProd->Product.prod, prod);
	NewProd->Product.price=price;
	NewProd->Product.qty=0;
	NewProd->nx=NULL;
	return NewProd;
}

void InsertProduct(char code[], char prod[], double price){
	int key = DJB2_Hash(prod);
	if(ListProd[key]==NULL){
		ListProd[key]=createProd(code, prod, price);
	} else{
		struct prodHash *now = ListProd[key];
		while(now->nx){
			now=now->nx;
		}
		now->nx=createProd(code, prod, price);
	}
}

void ReadProduct(){
	allProd=0;
	FILE *ReadProd = fopen("./Products/ProductList.csv", "r");
	char cd[11], prd[151];
	double pri;
	if(ReadProd){
		while(!feof(ReadProd)){
			fscanf(ReadProd, "%[^,],%[^,],$%lf\n", cd, prd, &pri);
			InsertProduct(cd,prd,pri);
			allProd++;
		}
	}
	fclose(ReadProd);
}

void ProductList(int totalProd){
	int a=0;
	for(int i=0; i<SIZE_TP; i++){
		if(ListProd[i]){
			struct prodHash *now = ListProd[i];
			while(now){
				if(a>=totalProd&&a<totalProd+10){
					printf("|[%2d.]|       %6s      | %34s |                           $%.2lf  |\n", a+1, now->Product.code, now->Product.prod, now->Product.price);
					puts("=====================================================================================================");
					strcpy(TempProd[a-totalProd], now->Product.prod);
				}
				a++;
				now=now->nx;
			}
		}
		if(a>=totalProd+10){
			break;
		}
	}
}

void InputView(char choice[], int tot){
	int check=0;
	do{
		printf("Do you want to add any of the product to your cart [Yes | No ");
		if(tot<allProd-10) printf("| Next ");
		if(tot>0) printf("| Back ");
		printf("] (case insensitive) ?: ");
		scanf("%[^\n]", choice); getchar();
		if(tot>0&&tot<allProd-10&&(strcmpi(choice, "yes")==0||strcmpi(choice, "no")==0||strcmpi(choice, "next")==0||strcmpi(choice, "back")==0)) check=1;
		else if (tot<=0&&(strcmpi(choice, "yes")==0||strcmpi(choice, "no")==0||strcmpi(choice, "next")==0)) check=1;
		else if (tot>=allProd-10&&(strcmpi(choice, "yes")==0||strcmpi(choice, "no")==0||strcmpi(choice, "back")==0)) check=1;
		else check=0;
	}while(check==0);
}

void ViewProduct(){
	int TotalProd=0;
	char choice[51];
	do{
		CleanTemp();
		system("cls");
		PrintHeader();
		ProductList(TotalProd);	
		InputView(choice, TotalProd);
//		for(int i=0; i<11; i++) printf("%s\n", TempProd[i]);
		if(strcmpi(choice, "yes")==0) BuyProd(TotalProd+1, TotalProd+10, 1);
		else if (strcmpi(choice, "next")==0) TotalProd+=10;
		else if (strcmpi(choice, "back")==0) TotalProd-=10;
	}while(strcmpi(choice, "no")!=0);
}

void InputChoiceSearch(char choice[]){
	do{
		printf("Do you want to add any of the product to your cart [Yes | No] case insensitive? : ");
		scanf("%[^\n]", choice); getchar();
	}while(!(strcmpi(choice,"yes")==0||strcmpi(choice,"no")==0));
}

int InputSearch(char que[]){
	do{
		printf("Enter the products that you're looking for [1-50 char]: ");
	scanf("%[^\n]", que); getchar();
	}while(strlen(que)<1||strlen(que)>50);
}

int SearchSub(char ch1[], char ch2[]){
	for(int i=0; i<strlen(ch1); i++){
		int checkStr=0;
		if(tolower(ch1[i])==tolower(ch2[0])){
			for(int j=0; j<strlen(ch2); j++){
				if(tolower(ch1[i+j])==tolower(ch2[j])) checkStr++;
			}
			if(checkStr==strlen(ch2)) return 1;
		}
	}
	return 0;
}

int PrintSearch(char que[]){
	int a=0, SubCheck=0;
	for(int i=0; i<SIZE_TP; i++){
		if(ListProd[i]){
			struct prodHash *now = ListProd[i];
			while(now){
				SubCheck=SearchSub(now->Product.prod, que);
				if(SubCheck==1){
					printf("|[%2d.]|       %6s      | %34s |                           $%.2lf  |\n", a+1, now->Product.code, now->Product.prod, now->Product.price);
					puts("=====================================================================================================");
					strcpy(TempProd[a], now->Product.prod);
					a++;
				}
				now=now->nx;
			}
		}
	}
	return a;
}

void SearchProduct(){
	char query[101];
	char choice[51];
	CleanTemp();
	InputSearch(query);
	system("cls");
	printf("Enter products that you're looking for: %s\n", query);
	PrintHeader();
	int totalSearch=PrintSearch(query);
	if(totalSearch>0){
	do{
		InputChoiceSearch(choice);
		if(strcmpi(choice, "yes")==0) BuyProd(1, totalSearch, 2);
	}while(strcmpi(choice, "no")!=0);
	} else{
		system("cls");
		printf("Enter products that you're looking for: %s\n", query);
		printf("We're sorry but the product that you're looking for doesn't seem ot exist.. YET...\n");
		printf("======================================================================================\n");
		printf("Press ENTER to continue...");
		getchar();
	}
}

int UpdateQty(){
	int qty;
	do{
		printf("Update to new quantity [>0]: ");
		scanf("%d", &qty); getchar();
	}while(qty<=0);
}

int ChooseProd(int num){
	int opt;
	do{
		printf("Select product [1 - %d]: ", num);
		scanf("%d", &opt); getchar();
	}while(opt<1||opt>num);
	return opt-1;
}

void UpdateCart(int num){
	int ProdIdx = ChooseProd(num);
	struct myCart *now= currUser.Cart.head;
	for(int i=0; i<ProdIdx; i++){
		now=now->nx;
	}
	now->cart.qty=UpdateQty();
}

void DeleteCart(int num){
	if(!currUser.Cart.head){
		return;
	}else{
		int ProdIdx = ChooseProd(num);
		if(currUser.Cart.head==currUser.Cart.tail){
			free(currUser.Cart.head);
			currUser.Cart.head=currUser.Cart.tail=NULL;
		}else{
			struct myCart *now= currUser.Cart.head;
			for(int i=0; i<ProdIdx; i++){
				now=now->nx;
			}
			if(now==currUser.Cart.head){
				struct myCart *tmp = currUser.Cart.head->nx;
				tmp->prev=NULL;
				currUser.Cart.head->nx=NULL;
				free(currUser.Cart.head);
				currUser.Cart.head=tmp;
			}else if(now==currUser.Cart.tail){
				struct myCart *tmp = currUser.Cart.tail->prev;
				tmp->nx=NULL;
				currUser.Cart.tail->prev=NULL;
				free(currUser.Cart.tail);
				currUser.Cart.tail=tmp;
			}else{
				now->nx->prev=now->prev;
				now->prev->nx=now->nx;
				free(now);
				now=NULL;	
			}	
	}
	}
}

void DeleteCartHead(){
	if(!currUser.Cart.head){
		return;
	}else if(currUser.Cart.head==currUser.Cart.tail){
		free(currUser.Cart.head);
		currUser.Cart.head=currUser.Cart.tail=NULL;
	}else{
		struct myCart *tmp = currUser.Cart.head->nx;
		tmp->prev=NULL;
		currUser.Cart.head->nx=NULL;
		free(currUser.Cart.head);
		currUser.Cart.head=tmp;
		currUser.Cart.head->prev=NULL;
	}	
}

struct TransItem *createItemHis(char TransID[], char ProdID[], int qty){
	struct TransItem *NewHis = (struct TransItem *) malloc(sizeof(struct TransItem));
	strcpy(NewHis->ProdID, ProdID);
	strcpy(NewHis->TransID, TransID);
	NewHis->qty=qty;
	NewHis->nx=NULL;
	NewHis->prev=NULL;
	return NewHis;
}

void InsertTrans(struct TransItem *NewHis){
	if(!currTrans.head){
		currTrans.head=currTrans.tail=NewHis;
	} else{
		currTrans.tail->nx=NewHis;
		NewHis->prev=currTrans.tail;
		currTrans.tail=NewHis;
		currTrans.tail->nx=NULL;
	}
}

void DeleteTrans(){
	if(!currTrans.head){
		return;
	} else if(currTrans.head==currTrans.tail){
		free(currTrans.head);
		currTrans.head=currTrans.tail=NULL;
	} else{
		struct TransItem *tmp = currTrans.head->nx;
		tmp->prev=NULL;
		currTrans.head->nx=NULL;
		free(currTrans.head);
		currTrans.head=tmp;
		currTrans.head->prev=NULL;
	}
}

int CheckID(char ID[]){
	if(HeadAllID){
		struct TrID *now = HeadAllID;
		while(now){
			if(strcmp(now->ID, ID)==0){
				return 1;
			}
			now=now->nx;
		}
	}
	return 0;
}

void DeletenCopyItem(char ID[]){
	while(currUser.Cart.head){
		InsertTrans(createItemHis(ID, currUser.Cart.head->cart.code, currUser.Cart.head->cart.qty));
		DeleteCartHead();
	}
	char dir[51];
	strcpy(dir, "./TransactionDetails/");
	strcat(dir, ID);
	strcat(dir, ".txt");
	dir[strlen(dir)]='\0';
	FILE *WriteDet = fopen(dir, "w");
	while(currTrans.head){
		fprintf(WriteDet, "%s#%s#%d\n", currTrans.head->TransID, currTrans.head->ProdID, currTrans.head->qty);
//		printf("%s#%s#%d\n", currTrans.head->TransID, currTrans.head->ProdID, currTrans.head->qty);
		DeleteTrans();
	}
	fclose(WriteDet);
}

struct TransHead *createTrHead(char ID[]){
	struct TransHead *NewHead = (struct TransHead*) malloc(sizeof(struct TransHead));
	strcpy(NewHead->ID, ID);
	char date[51];
	time_t TimenDate=time(NULL);
	struct tm lctm = *localtime(&TimenDate);
	sprintf(date, "%d-%d-%d %d:%d:%d", lctm.tm_year+1900, lctm.tm_mon+1, lctm.tm_mday, lctm.tm_hour, lctm.tm_min, lctm.tm_sec);
	strcpy(NewHead->Date, date);
	strcpy(NewHead->user, currUser.Crd.user);
	NewHead->nx=NULL;
	NewHead->prev=NULL;
	return NewHead;
}

void saveID(struct TransHead *NewHead){
	if(!currUser.THead.head){
		currUser.THead.head=currUser.THead.tail=NewHead;
	}else{
		currUser.THead.tail->nx=NewHead;
		NewHead->prev=currUser.THead.tail;
		currUser.THead.tail=NewHead;
		currUser.THead.tail->nx=NULL;
	}
}

void CheckOut(){
	printf("Initializing checkout sequence...\n");
	system("cls");
	char IDTrans[7];
	do{
		sprintf(IDTrans, "TR-%d%d%d", (int)rand()%10,(int)rand()%10,(int)rand()%10);
	}while(CheckID(IDTrans)==1);
	printf("Transaction ID: %s\n", IDTrans);
	insertID(createID(IDTrans));
	DeletenCopyItem(IDTrans);
	saveID(createTrHead(IDTrans));
	printf("Checkout Successful!\n");
	printf("Press ENTER to continue...");
	getchar();
}

void HeaderCart(){
	printf("==============================================================================================================\n");
 	printf("| [No.] |     Product Id|                       Product Name |                 Product Price |    Quantity   |\n");
 	printf("==============================================================================================================\n");
}

int PrintCart(){
	if(!currUser.Cart.head){
		return 0;
	} else{
		int a=0;
		struct myCart *now = currUser.Cart.head;
		while(now){
			printf("| [%2d.] |       %6s | %35s |                      $%.2lf  |       %4d    |\n", a+1, now->cart.code, now->cart.prod, now->cart.price, now->cart.qty);
			puts("==============================================================================================================");
			a++;		
			now=now->nx;
		}
		return a;
	}
	
}

InputChoiceCart(char choice[]){
	do{
		printf("What do you want to do? [ Update | Delete | Checkout | Exit ] (case insensitive): ");
		scanf("%[^\n]", choice); getchar();
	}while(!(strcmpi(choice, "update")==0||strcmpi(choice, "delete")==0||strcmpi(choice, "checkout")==0||strcmpi(choice, "exit")==0));
}

void ManageCart(){
	if(currUser.Cart.head){
		char choice[51];
		do{
			HeaderCart();
			int totalCart = PrintCart();
			InputChoiceCart(choice);
			if(strcmpi(choice, "update")==0){
				UpdateCart(totalCart);
			} else if (strcmpi(choice, "delete")==0){
				DeleteCart(totalCart);
			} else if (strcmpi(choice, "checkout")==0){
				CheckOut();
			}
		} while(strcmpi(choice, "exit")!=0&&currUser.Cart.head!=NULL);
		
	} else{
		printf("Cart is empty, please add some item to the cart!\n");
		printf("====================================================\n");
		printf("Press ENTER to continue...");
		getchar();
	}
}

void PrintDetailHead(){
	printf("==================================================================================\n");
	printf("| [No.] | Transaction ID |                     Product ID |             Quantity |\n");
	printf("==================================================================================\n");
}

void CheckDetail(int idx){
	struct TransHead *now = currUser.THead.head;
	for(int i=0; i<idx; i++){
		now=now->nx;
	}
	char dir[51];
	strcpy(dir, "./TransactionDetails/");
	strcat(dir, now->ID);
	strcat(dir, ".txt");
	dir[strlen(dir)]='\0';
	FILE *OpenDet = fopen(dir, "r");
	if(OpenDet){
		PrintDetailHead();
		int a=0, qty=0;
		char TransID[11], ProdID[11];
		while(!feof(OpenDet)){
			fscanf(OpenDet, "%[^#]#%[^#]#%d\n", TransID, ProdID, &qty);
			printf("| [%2d.] |         %6s | %30s | %19d |\n", a+1, TransID, ProdID, qty);
			printf("==================================================================================\n");
			a++;
		}
	}
	printf("Press ENTER to continue...\n");
	getchar();
}

int InputIdxCheck(int tot){
	int num;
	do{
		printf("Which of the header that you would want to check the detail? [ 1 - %d ]: ", tot);
		scanf("%d", &num); getchar();
	}while(num<1||num>tot);
	return num-1;
}

void PrintTransHeader(){
	system("cls");
	printf("=======================================================================================================\n");
	printf("| [No.] |     Product Id|                           Username |                                   Date |\n");
	printf("=======================================================================================================\n");
}

int ViewTransHis(){
	PrintTransHeader();
	int a=0;
	struct TransHead *now = currUser.THead.head;
	while(now){
		printf("| [%2d.] |       %6s | %34s | %39s |\n", a+1, now->ID, now->user, now->Date);
		printf("=======================================================================================================\n");
		a++;
		now=now->nx;
	}
	return a;
}

void InputChoice(char choice[]){
	do{
		printf("What would you like to do? [ Back | Check ] (case insesnsitive): ");
		scanf("%[^\n]", choice); getchar();
	}while(!(strcmpi(choice, "back")==0||strcmpi(choice, "check")==0));
}

void ViewHis(){
	if(currUser.THead.head){
		char choice[51];
		do{
			int totalHis = ViewTransHis();
			InputChoice(choice);
			if(strcmpi(choice, "check")==0){
				CheckDetail(InputIdxCheck(totalHis));
			}
		}while(strcmpi(choice, "back")!=0);
	}else{
		printf("No transaction has been made...\n");
		printf("=======================================\n");
		printf("Press ENTER to continue...");
		getchar();
	}
}

void CleanCurrUser(){
	currUser.Cart.head=NULL;
	currUser.Cart.tail=NULL;
	strcpy(currUser.Crd.email, "");
	strcpy(currUser.Crd.pass, "");
	strcpy(currUser.Crd.user, "");
	currUser.THead.head=NULL;
	currUser.THead.tail=NULL;
}

struct UserList *CreateHash(){
	struct UserList *NewUser = (struct UserList *) malloc(sizeof(UserList));
	NewUser->user=currUser;
	NewUser->nx=NULL;
	return NewUser;
}

void CopyToHashT(struct UserList *NewUser){
	int key = DJB2_Hash(NewUser->user.Crd.email);
	if(!UserLogged[key]){
		UserLogged[key] = NewUser;
	}else{
		struct UserList *now = UserLogged[key];
		while(now->nx){
			now=now->nx;
		}
		now->nx=NewUser;
	}
}

struct UserList *SearchHash(){
	int key = DJB2_Hash(currUser.Crd.email);
	if(UserLogged[key]){
		struct UserList *now = UserLogged[key];
		while(now){
			if(strcmp(currUser.Crd.email, now->user.Crd.email)==0){
				return now;
			}
			now=now->nx;
		}
	}
	return NULL;
}

struct myCart *CartFromFile(char prodID[], char prod[], double price, int qty){
	struct myCart *NewCart = (struct myCart*) malloc(sizeof(struct myCart));
	strcpy(NewCart->cart.code,prodID);
	strcpy(NewCart->cart.prod,prod);
	NewCart->cart.price=price;
	NewCart->cart.qty=qty;
	NewCart->nx=NULL;
	NewCart->prev=NULL;
	return NewCart;
}

void InserttoCartFromFile(struct myCart *NewCart){
	if(!currUser.Cart.head){
		currUser.Cart.head=currUser.Cart.tail=NewCart;
	} else{
		currUser.Cart.tail->nx=NewCart;
		NewCart->prev=currUser.Cart.tail;
		currUser.Cart.tail=NewCart;
		currUser.Cart.tail->nx=NULL;
	}
}

struct TransHead *HeadFromFile(char TransID[], char user[], char time[]){
	struct TransHead *NewHead = (struct TransHead*) malloc(sizeof(struct TransHead));
	strcpy(NewHead->ID,TransID);
	strcpy(NewHead->user,user);
	strcpy(NewHead->Date, time);
	NewHead->nx=NULL;
	NewHead->prev=NULL;
	return NewHead;
}

void InserttoHeadFromFile(struct TransHead *NewHead){
	if(!currUser.THead.head){
		currUser.THead.head=currUser.THead.tail=NewHead;
	} else{
		currUser.THead.tail->nx=NewHead;
		NewHead->prev=currUser.THead.tail;
		currUser.THead.tail=NewHead;
		currUser.THead.tail->nx=NULL;
	}
}

void popHash(char email[]){
	int key = DJB2_Hash(email);
	if(UserLogged[key]){
		if(!UserLogged[key]->nx){
			free(UserLogged[key]);
			UserLogged[key]=NULL;
		}else{
			struct UserList *now = UserLogged[key];
			while(now->nx&&strcmp(now->nx->user.Crd.email,email)!=0){
				now=now->nx;
			}
			struct UserList *Del= now->nx;
			now->nx=now->nx->nx;
			free(Del);
		}
	}
}

void CopyCartTrans(){
	struct UserList *piv=SearchHash();
	if(piv){
		currUser=piv->user;
		popHash(piv->user.Crd.email);
	} else{
		char dir[51];
		strcpy(dir, "./UserCarts/");
		strcat(dir, currUser.Crd.email);
		strcat(dir, ".txt");
		dir[strlen(dir)]='\0';
		FILE *ReadCart= fopen(dir,"r");
		if(ReadCart){
			char ProdID[11], Prod[101];
			int qty;
			double price;
			while(!feof(ReadCart)){
				fscanf(ReadCart, "%[^#]#%[^#]#$%lf#%d\n", ProdID, Prod, &price, &qty);
				if(strcmp(Prod,"")!=0){
					InserttoCartFromFile(CartFromFile(ProdID, Prod, price,qty));
				}
			}
		}
		fclose(ReadCart);
		
		strcpy(dir, "./TransactionHeader/");
		strcat(dir, currUser.Crd.email);
		strcat(dir, ".txt");
		dir[strlen(dir)]='\0';
		FILE *ReadHead= fopen(dir,"r");
		if(ReadHead){
			char TransID[11], time[101], user[101];
			while(!feof(ReadHead)){
				fscanf(ReadHead, "%[^#]#%[^#]#%[^\n]\n", TransID, user, time);
				InserttoHeadFromFile(HeadFromFile(TransID, user, time));
			}
		}
		fclose(ReadHead);
	}
}

void HomeDis(){
	system("cls");
	printf("Welcome to ShoPAI\n");
	printf("======================\n");
	printf("1. View product\n");
	printf("2. Search product\n");
	printf("3. Manage cart\n");
	printf("4. View transaction history\n");
	printf("5. Exit\n");
	printf(">> ");
}

int HomeOpt(){
	int opt;
	do{
		HomeDis();
		scanf("%d", &opt); getchar();
	}while(opt<1||opt>5);
	return opt;	
}

void Home(){
	int HOpt;
	CopyCartTrans();
	do{
		CleanTemp();
		HOpt=HomeOpt();
		switch(HOpt){
			case 1:
				ViewProduct();
				break;
			case 2:
				SearchProduct();
				break;
			case 3:
				ManageCart();
				break;
			case 4:
				ViewHis();
				break;
		}
	}while(HOpt!=5);
	CopyToHashT(CreateHash());
	CleanCurrUser();
}

int CheckCred(struct user *rt, char email[], char pass[]){
	if(!rt){
		return 0;
	} else if(strcmp(email, rt->log.email)<0){
		CheckCred(rt->l, email, pass);
	} else if(strcmp(email, rt->log.email)>0){
		CheckCred(rt->r, email, pass);
	} else if(strcmp(email, rt->log.email)==0){
		if(strcmp(pass, rt->log.pass)==0){
			strcpy(currUser.Crd.email, email);
			strcpy(currUser.Crd.user, rt->log.user);
			strcpy(currUser.Crd.pass, pass);
			return 1;
		} else{
			return 0;
		}
	}
}

void LoginEmail(char email[]){
	printf("Enter email: ");
	scanf("%[^\n]", email); getchar();
}

void LoginPass(char pass[]){
	printf("Enter password: ");
	scanf("%[^\n]", pass); getchar();
}

void Login(){
	system("cls");
	printf("Login\n");
	printf("============\n");
	char email[101], pass[51];
	LoginEmail(email);
	LoginPass(pass);
	if(CheckCred(Root_User, email, pass)==1){
		printf("Login successful!\n");
		printf("Press ENTER to continue....");
		getchar();
		Home();
	}else{
		printf("Invalid credential!\n");
		printf("Press ENTER to continue...");
		getchar();
	}
}

//void ViewInorder(struct user *now){
//	if(now){
//		ViewInorder(now->l);
//		printf("%s %s %s\n", now->log.email, now->log.user, now->log.pass);
//		ViewInorder(now->r);
//	}
//}

struct user *CreateUser(char email[], char user[], char pass[]){
	struct user *NewUser=(struct user *) malloc(sizeof(struct user));
	strcpy(NewUser->log.email, email);
	strcpy(NewUser->log.user, user);
	strcpy(NewUser->log.pass, pass);
	NewUser->l = NewUser->r = NULL;
	return NewUser;
}

struct user *InsertUser(struct user *rt, struct user *NewUser){
	if(!rt){
		return NewUser;
	} else if(strcmp(NewUser->log.email, rt->log.email)<0){
		rt->l=InsertUser(rt->l, NewUser);
	} else if(strcmp(NewUser->log.email, rt->log.email)>0){
		rt->r=InsertUser(rt->r, NewUser);
	}
}

int IsUnique(struct user *rt, char email[]){
	if(!rt){
		return 0;
	} else if(strcmp(email, rt->log.email)<0){
		IsUnique(rt->l, email);
	} else if(strcmp(email, rt->log.email)>0){
		IsUnique(rt->r, email);
	} else if(strcmp(email, rt->log.email)==0){
		return 1;
	}
}

int EmailSuffix(char email[]){
	char check[11]="";
	int j=0;
	for(int i=strlen(email)-10; i<=strlen(email)-1; i++, j++){
		check[j]=email[i];
	}
	check[j+1]='\0';
//	printf("%s\n", check);
	if(strcmp(check, "@gmail.com")==0){
		return 1;
	} else{
		return 0;
	}
}

void InputEmail(char email[]){
	do{
		printf("Email [ends with @gmail.com and unique]: ");
		scanf("%[^\n]", email); getchar();
	}while(EmailSuffix(email)==0||IsUnique(Root_User, email)==1);
}

void InputUser(char user[]){
	do{
		printf("Username [3-10 characters]: ");
		scanf("%[^\n]", user); getchar();
	}while(strlen(user)<3||strlen(user)>10);
}

int AlphaOnly(char pass[]){
	for(int i=0; i<strlen(pass); i++){
		if(!(pass[i]>='A'&&pass[i]<='Z')&&!(pass[i]>='a'&&pass[i]<='z')){
			return 0;
		}
	}
	return 1;
}

void InputPass(char pass[]){
	do{
		printf("Password [5-10 characters & alphabet]: ");
		scanf("%[^\n]", pass); getchar();
	}while(strlen(pass)<5||strlen(pass)>10||AlphaOnly(pass)==0);
}

void Register(){
	system("cls");
	printf("Register\n");
	printf("==========\n");
	char email[101];
	InputEmail(email);
	char user[51];
	InputUser(user);
	char pass[51];
	InputPass(pass);
	Root_User=InsertUser(Root_User, CreateUser(email, user, pass));
	printf("Registered successfully\n");
	printf("Press ENTER to continue...");
	getchar();
}

void PrintToFile(struct user *rt, FILE *Write){
	if(rt){
		PrintToFile(rt->l, Write);
		fprintf(Write, "%s#%s#%s\n", rt->log.email, rt->log.user, rt->log.pass);
		PrintToFile(rt->r, Write);
	}
}

void saveCred(){
	FILE *SaveCred=fopen("./UserData/LoginData.txt", "w");
	PrintToFile(Root_User,SaveCred);
	fclose(SaveCred);
}

void MainDis(){
	system("cls");
//	ViewInorder(Root_User);
	printf("Welcome to ShoPAI\n");
	printf("====================\n");
	printf("1. Login\n");
	printf("2. Register\n");
	printf("3. Exit\n");
	printf(">> ");
}

int MainOpt(){
	int opt;
	do{
		MainDis();
		scanf("%d", &opt); getchar();
	}while(opt<1||opt>3);
	return opt;
}

void ReadUser(){
	FILE *ReadUser=fopen("./UserData/LoginData.txt", "r");
	char email[101], user[51], pass[51];
	if(ReadUser){
		while(!feof(ReadUser)){
			fscanf(ReadUser, "%[^#]#%[^#]#%[^\n]\n", email, user, pass);
			Root_User=InsertUser(Root_User, CreateUser(email, user, pass));
		}
	}
	fclose(ReadUser);
}

void ReadTrID(struct user *rt){
	char dir[51]="";
	strcpy(dir, "./TransactionHeader/");
	strcat(dir, rt->log.email);
	strcat(dir, ".txt");
	dir[strlen(dir)]='\0';
	char ID[7], g1[101], g2[101];
	FILE *ReadID = fopen(dir, "r");
	if(ReadID){
		while(!feof(ReadID)){
			fscanf(ReadID, "%[^#]#%[^#]#%[^\n]\n", ID, g1, g2);
			insertID(createID(ID));
//			printf("%s\n", ID);
		}
	}
}

void ReadTrans(struct user *rt){
	if(rt){
		ReadTrans(rt->l);
//		printf("%s\n", rt->log.email);
		ReadTrID(rt);
		ReadTrans(rt->r);
	}
}

void SaveCartTrans(){
	for(int i=0; i<SIZE_TU; i++){
		if(UserLogged[i]){
			struct UserList *now = UserLogged[i];
			while(now){
				char dir[51];
				strcpy(dir, "./UserCarts/");
				strcat(dir, now->user.Crd.email);
				strcat(dir, ".txt");
				dir[strlen(dir)]='\0';
				FILE *WriteCart= fopen(dir,"w");
				struct myCart *ThisCart = now->user.Cart.head;
				while(ThisCart){
					fprintf(WriteCart, "%s#%s#$%.2lf#%d\n", ThisCart->cart.code, ThisCart->cart.prod, ThisCart->cart.price, ThisCart->cart.qty);
					ThisCart=ThisCart->nx;
				}
				fclose(WriteCart);
				
				strcpy(dir, "./TransactionHeader/");
				strcat(dir, now->user.Crd.email);
				strcat(dir, ".txt");
				dir[strlen(dir)]='\0';
				FILE *WriteHead= fopen(dir,"w");
				struct TransHead *ThisTrans= now->user.THead.head;
				while(ThisTrans){
					fprintf(WriteHead, "%s#%s#%s\n", ThisTrans->ID, ThisTrans->user, ThisTrans->Date);
					ThisTrans=ThisTrans->nx;
				}
				fclose(WriteHead);
				now=now->nx;
			}
		}
	}
		
}

int main(){
	int MOpt=0;
	ReadTrans(Root_User);
	srand(time(NULL));
	ReadUser();
	ReadProduct();
	do{
		MOpt=MainOpt();
		switch(MOpt){
			case 1:
				Login();
				break;
			case 2:
				Register();
				break;
		}
	}while(MOpt!=3);
	saveCred();
	SaveCartTrans();
	printf("Thanks for using ShoPAI!\n");
	return 0;
}
