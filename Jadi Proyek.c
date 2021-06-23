#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DELAY 100000
#include <omp.h>

//Sistem Identifikasi Zona Daerah

//Agung Firmansyah (2006577454)
//Brian Christian Pangaribuan (2006577510)
//Muhammad Aditya Kurniawan (200577340)

//Berikut adalah program yang mendata kasus positif di suatu daerah berdasarkan gejala yang dimasukkan (1 - 10).
//Jika gejala yang diinput totalnya lebih besar daripada 25, penduduk dinyatakan positif. Total penduduk yang 
//positif pada suatu daerah dibagi dengan total penduduknya jika lebih besar dan sama dengan 0.51, daerah 
//dinyatakan zona merah. Lamanya waktu diukur menggunakan OpenMP di line ke-47, di mana waktu berbanding 
//lurus dengan jumlah penduduk dan/atau daerah.	

typedef struct warga{ //Penduduk yang akan didata
	int gejala;
	char positif[1]; //status korona, 1 adalah size positif, yakni + atau - saja
	struct warga *next; //penduduk berikutnya
	char nama[];
}warga;

typedef struct daerah{ //Daerah yang akan didata
	int Positif; //
	int nWarga;
	char zona[5]; //Zona merah/hijau
	struct warga *head;
	struct daerah *next; //Daerah berikutnya
	char kota[100]; //Nama daerah
}daerah;
//Prototype 
	void printGejala();//Opsi gejala yang akan dipilih
	warga *buatinPenduduk();//Input nama, gejala, dan status penduduk dalam linked list 
	daerah *buatinDaerah();//Input nama daerah dan jumlah penduduk dalam linked list
	void printPenduduk(warga *temp, FILE *fptr);//output nama dan status penduduk
	void printsemua(daerah *temp, FILE *fptr, int waktu);//output keterangan program, nama daerah, total penduduk yang positif, dan penduduk yang didata

int main(){
	daerah *head = NULL;//node awal masih kosong
	daerah *pred = NULL, *temp; //node di daerah
	warga *Wpred = NULL, *Wtemp;// node di penduduk
	int Daerah,i,j,del;
	int waktu, waktu_1, waktu_2;//agar hari bukan bilangan pecahan
	FILE *fptr;//deklarasi file

	fptr = fopen("adit.txt","w");//tipe file ditulis
	printf (" --------------------------------------------------------------------------------------------------\n");
	printf("| Masukkan jumlah daerah: ");//input jumlah daerah
	scanf("%d", &Daerah);
	
	waktu_1 = omp_get_wtime();//mulai
	#pragma omp for
	for(i=0;i<Daerah;i++){
		temp = NULL;//temp daerah masih kosong
		temp = buatinDaerah();//call buatinDaerah dan dimasukkan  sementara  ke temp daerah
		Wpred = temp->head;//Wpred penduduk menjadi node awal di daerah 
		for(j=0;j<temp->nWarga;j++){
			
			Wtemp = NULL;//Wtemp penduduk masih kosong
			Wtemp = buatinPenduduk();//call buatinPenduduk
			if(Wtemp->positif[0] == '+') temp->Positif += 1;//sebagai total positif penduduk di daerah
			
			if(Wpred == NULL){//penduduk belum/tidak ada
				temp->head = Wtemp;//buat penduduk di awal node
			}//end if 
			else{//jika penduduk sudah ada
				Wpred->next = Wtemp;//node next menunjuk ke penduduk berikutnya
			}//end else
			Wpred = Wtemp;
		}//end for
		for (del=0;del<DELAY;del++);//Delay agar waktu tidak 0
		if(1.0 * temp->Positif / temp->nWarga >= 0.51){//Jika jumlah positif per total penduduk >= 51%
			strcpy(temp->zona , "Merah");//Zona merah
		}//end if
		else strcpy(temp->zona , "Hijau");//Zona hijau
		
		if(pred == NULL) head = temp;//daerah belum/tidak ada
		else pred->next = temp;//node next menunjuk ke daerah berikutnya
		pred = temp;
	}//end for
	waktu_2 = omp_get_wtime();//selesai
	
	waktu = (waktu_2 - waktu_1)/6;//total waktu (hari)

	printsemua(head, fptr, waktu);//call printsemua
	
	fclose (fptr);//file ditutup
	
	return 0;
}//end main

void printGejala(){
	#pragma omp parallel num_threads(4)//Membagi menjadi 4 threads
	{
		#pragma omp master  //Agar fungsi yang terdapatg printf dijalankan hanya sekali, bukan 4
		{
			printf ("| Daftar Gejala dari teringan hingga terberat:\n");
			printf ("|	1. Hidung tersumbat\n|	2. Batuk ringan\n|	3. Sakit kepala\n|	4. Asma\n|	5. Diare\n|	6. Demam tinggi\n");
			printf ("|	7. Kulit, bibir, dan kuku membiru\n|	8. Kehilangan indera perasa\n|	9. Kehilangan indera penciuman\n|	10. Pneumonia kronis\n");
			printf ("| Masukkan -1 untuk berhenti menginput gejala!\n");
			printf ("|-------------------------------------------------------------------------------------------\n");
		}//end #pragma omp master
	}//end #pragma omp parallel
}//end PrintGejala

warga *buatinPenduduk(){
	warga *newNode;
	newNode = (warga*) malloc (sizeof(warga));//membuat node penduduk
	
	newNode->next = NULL;//node next belum diisi
	newNode->gejala = 0;//node gejala belum diisi
	printf ("|-------------------------------------------------------------------------------------------\n");
	printf("| Nama penduduk : ");//input nama penduduk
	fflush(stdin);
	scanf("%[^\n]s", &newNode->nama);
	
	printGejala();//call printGejala
	printf ("|-------------------------------------------------------------------------------------------\n");
	printf("| Masukkan gejala : \n");//input gejala penduduk
	int Gejala[10] = {0,0,0,0,0,0,0,0,0,0};//array gejala belum diisi
	int input;
	
	do{
		printf ("| ");
		scanf("%d", &input);
		if(input<11 && input>0)//input normal (kondisi tidak error)
			if(Gejala[input] == 0){//kondisi agar perulangan gejala yang sama per penduduk tidak dijumlahkan
				Gejala[input] = 1;
				newNode->gejala += input;//menjumlahkan gejala per penduduk berdasarkan urutan opsi
			}//end if
	}while(input != -1 && input>0 && input<11);//looping normal (kondisi tidak error)
	
	if(newNode->gejala >= 25){//Jika total gejala per penduduk >= 25
		newNode->positif[0] = '+';//penduduk positif korona 
	}//end if
	else newNode->positif[0] = '-';//penduduk negatif korona
	
	return newNode;
}//end *buatinPenduduk

daerah *buatinDaerah(){
	daerah *baru;
	baru = (daerah*) malloc (sizeof(daerah));//membuat node daerah
	
	baru->next = NULL;//node next daerah belum diisi 
	baru->head = NULL;//node awal daerah belum diisi
	baru->Positif = 0;//total jumlah penduduk yang positif belum diisi
	printf ("|-------------------------------------------------------------------------------------------\n|");
	printf("\n| Nama daerah : ");//input nama daerah
	scanf("%s", &baru->kota);
	printf ("|-------------------------------------------------------------------------------------------\n");
	printf("| Jumlah penduduk : ");//input jumlah penduduk
	scanf("%d", &baru->nWarga);
	
	return baru;
}//end *buatinDaerah


void printPenduduk(warga *temp, FILE *fptr){

	fprintf(fptr, "(");
	while(temp != NULL){
		if(temp->next != NULL) fprintf(fptr, "%s [%s], ",temp->nama,temp->positif);//untuk tanda koma terletak di akhir
		else fprintf(fptr, "%s [%s]",temp->nama,temp->positif);//untuk tanda koma terletak di akhir
		temp = temp->next;
	}//end while
	fprintf(fptr, ")\n");
}//end printPenduduk 

void printsemua(daerah *temp, FILE *fptr, int waktu){
	//keterangan program
		fprintf (fptr," ===============================================================================================================\n");
		fprintf (fptr,"| Berikut adalah program yang mendata kasus positif di suatu daerah berdasarkan gejala yang dimasukkan (1 - 10).\n");
		fprintf (fptr,"| Jika gejala yang diinput totalnya lebih besar daripada 25, penduduk dinyatakan positif.\n");
		fprintf (fptr,"| Total penduduk yang positif pada suatu daerah dibagi dengan total penduduknya jika lebih besar dan sama dengan 0.51,\n");	 
		fprintf (fptr,"| daerah dinyatakan zona merah.\n");	
		fprintf (fptr," ===============================================================================================================\n");
		fprintf(fptr,"| Kasus korona yang berlangsung selama %d hari mempunyai data sebagai berikut\n", waktu);
		//output nama daerah, total penduduk yang positif, dan penduduk yang didata
		while(temp != NULL){
			fprintf(fptr," ==============================================================================\n|\n");
			fprintf(fptr,"| Nama Daerah    : %s [%s]", temp->kota, temp->zona);
			fprintf(fptr,"\n| Jumlah Positif : %d", temp->Positif);
			fprintf(fptr,"\n| Daftar Warga   : ");
			printPenduduk(temp->head, fptr);//call printPenduduk
			fprintf (fptr,"|\n ==============================================================================\n\n\n");
			
			temp = temp->next;
		}//end while
}//end printsemua
