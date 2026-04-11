#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct SHIPPER{
	char code[5];
	char Name[30];
	long long CCCD;
	int x;
	int y;
	double weight;
	int status;
	int numberOrder;
	struct SHIPPER *next;
}shipper;

//Code Ôg Huy có cái này nha t đưa ra để ktr nó chạy thôi -----> order **headO,

int Shipper_Management(shipper **headS){  
	int countChoice = 0;
	int choiceTwo;
	printf("\n1. Register as a new Shipper"
			"\n2. Update Location/Status (Available/Busy/On Break)"
			"\n3. View the list of active shippers\n\n");
	do{
		if(countChoice==3){
			return -1;
		}		
		printf("Enter your choice(1-3): ");
		scanf("%d", &choiceTwo);
		++countChoice;
	}while(choiceTwo>3 || choiceTwo<1);
}

//Hàm Thông tin của shipper
void EnterInformation (shipper *A){
    printf ("Enter Your Name: ");
    fgets (A->Name,sizeof(A->Name),stdin);
    A->Name[strcspn(A->Name, "\n")]='\0';
    
    printf ("Enter Your CCCD: ");
    scanf("%lld", &A->CCCD);
    getchar();
}


//Hàm cung cấp trọng tải va sô lượng đơn hàng
//theo ý kiến cá nhân cái này có thể linh hoạt thay đổi nên tạo hàm riêng để cho dễ xử lý
void orderWeightAndNumber (shipper *A){
	//trọng lượng tối đa cảu shipper 
	//thông tin từ shipper cung cấp
    printf ("Enter Your Order Weight: ");
    scanf ("%lf",&A->weight);
    getchar();
    
    //Nhập lượng đơn hàng đã nhận;
    printf ("Enter Your Order Number: ");
    scanf ("%d",&A->numberOrder);
    getchar();
}

//Nhận Mã shipper
void receiveCode (shipper *A){
	static int NoS=1;
    sprintf (A->code,"SP%02d",NoS++);
}

//Mỗi 1 shipper là 1 Node chưa dc liên kết
shipper *CreateShipper (){
    shipper *newShipper=(shipper*)malloc(sizeof(shipper));
    if(newShipper == NULL)
        return NULL;
        
    EnterInformation(newShipper);
    orderWeightAndNumber(newShipper);
    receiveCode(newShipper);
    printf ("\n");
    
    newShipper->next=NULL;
    return newShipper;
}

shipper *registerShipper (){
    shipper *Tail=NULL,*headS=NULL;
    
    //nhập số lượng shipper chuẩn bị cung cấp thông  tin ( có thể nhiều hoặc ít)
	//để linh hoạt trong việc cung cấp thông tin
    //NoS = Number of Shhipper
    int NoS;
    scanf ("%d",&NoS);
    getchar();
    
    for (int i=0;i< NoS;i++){
    //Nhập thông tin của n shipper đã đăng ký
    shipper *newShipPtr=CreateShipper();
    	if (headS==NULL){
    		headS=newShipPtr;
    		Tail=newShipPtr;
		}
		else{
			Tail->next=newShipPtr;
			Tail = newShipPtr;
		}
	}
    return headS;
}
// dựa vào thằng làm phần 3
/*void updateLocation(shipper **headS){
    
}*/

//dựa vào cái location
/*void updateAndEnterOrder (shipper **headS){

}*/

//dựa vào cái location
/*void statusShipper (shipper **headS){

}*/

//lưu thông tin shipper vao file
void informationShipper (shipper **headS){
	shipper *p=*headS;
	FILE *S = fopen("information.txt","w");
	if (S==NULL){
		printf ("ERROL!");
		return;
	}
	fprintf (S,"====================================List shipper==================================\n");
	fprintf (S,"----------------------------------------------------------------------------------\n");
	fprintf (S,"||%-30s||%-20s||%-10s||\n","Name","CCCD","CODE");
	while (p!=NULL){
		fprintf (S,"||%-30s||%-20lld||%-10s||\n",p->Name,p->CCCD,p->code);
		p=p->next;
	}
	fclose(S);
}

int main(){
	shipper *S=registerShipper();
	//====================
	informationShipper(&S);
}