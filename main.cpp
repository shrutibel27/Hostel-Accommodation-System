#include <iostream>
#include <mysql.h>
#include <mysqld_error.h>
#include <windows.h>
#include <sstream>

using namespace std;


const char* host= "localhost";
const char* user="root";
const char* pw="Shruti@2705";
const char* db="mybd";


class seats{
	private:
		int seat[5][10];
	public:
	seats()
	{
		for(int i=0;i<5;i++){
			for(int j=0;j<10;j++){
				seat[i][j]=1;
			}
		}
	}
	
	int getseatstatus(int row,int seatnumber){
		if(row<1 || row>5 || seatnumber<1 || seatnumber>10){
			return -1;
		}
		return seat[row-1][seatnumber-1];
	}	
	
	void reserveseat(int row,int seatnumber){
		if(row<1 || row>5 || seatnumber<1 || seatnumber>10){
			return ;
		}
		seat[row-1][seatnumber-1]=0;
	}
	
	display(){
		cout<<" ";
		for(int i=0;i<10;i++){
			cout<<" "<<i+1;
		}
		cout<<endl;
		for(int row=0;row<5;row++){
			cout<<row+1<<" ";
			for(int col=0;col<10;col++){
				if(seat[row][col]==1){
					cout<<"-";
				}
				else{
					cout<<"x";
				}
			}
			cout<<"|"<<endl;
		}
		cout<<"____________________"<<endl;
	}
	
	getdb(MYSQL* conn){
		string query="SELECT rownumber,seatnumber,seat FROM ticket";
		if(mysql_query(conn,query.c_str())){
			cout<<"ERROR : "<<mysql_error(conn)<<endl;
		}
		MYSQL_RES* result;
		result=mysql_store_result(conn);
		if(!result){
			cout<<"ERROR: "<<mysql_error(conn)<<endl;
		}
		MYSQL_ROW row;
		while((row=mysql_fetch_row(result))){
			int rownumber=atoi(row[0]);
			int seatnumber=atoi(row[1]);
			int seatstatus=atoi(row[2]);
			seat[rownumber-1][seatnumber-1]=seatstatus;
		}
		mysql_free_result(result);
	}
};

int main() {
	seats s;
	MYSQL* conn;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,host,user,pw,db,3306,NULL,0)){
		cout<<"Error: "<<mysql_error(conn)<<endl;
	}
	else
	{
		cout<<"Logged In Datbase!"<<endl;
	}
	
//	sleep(3000);  // windows.h 3s in milisec
	
	if(mysql_query(conn,"CREAT TABLE IF NOT EXITS ticket(rownumber int , seatnumber int ,seat int)")){
		cout<<"Error : "<<mysql_error(conn)<<endl;
	}
	
	for(int row=1;row<=5;row++){
		for(int seatnumber=1;seatnumber<=10;seatnumber++){
			stringstream ss;
			ss<<"INSERT INTO ticket(rownumber,seatnumber,seat)"
			<<"SELECT '"<<row<<"','"<<seatnumber<<"','1'"
			<<"WHERE NOT EXIST (SELECT * FROM ticket WHERE rownumber='"<<row<<"' AND seatnumber='"<<seatnumber<<"')";
			string insertquery=ss.str();
			if(mysql_query(conn,insertquery.c_str())){
				cout<<"ERROT: "<<mysql_error(conn);
			}
		}
	}
//	sleep(3000);
	bool exit=false;
	while(!exit){
		system("cls");
		cout<<endl;
		cout<<"WELCOME TO MOVIE TICKET RESERVATION SYSTEM\n"<<endl;
		cout<<"1. RESERVE A TICKET"<<endl;
		cout<<"2. EXIT"<<endl;
		cout<<"ENTER YOUR CHOICE: ";
		int val;
		cin>>val;
		if(val==1){
			s.getdb(conn);
			s.display();
			
			int row,col;
			cout<<"Enter Row(1-5) :";
			cin>>row;
			cout<<"Enter seat number (1-10) :";
			cin>>col;
			
			if(row<1 || row>5 || col<1 || col>10){
			    cout<<"INVALID Row or Seat NUMBER!"<<endl;
//			    sleep(3000);
			    continue;
			}
			int seatstatus=s.getseatstatus(row,col);
			if(seatstatus==-1){
				cout<<"INVALID row or seat number!"<<endl;
//				sleep(3000);
				continue;
			}
			if(seatstatus==0){
				cout<<"Sorry: seat is already reseraved!"<<endl;
//				sleep(3000);
				continue;
			}
			s.reserveseat(row,col);
			stringstream ss;
			ss<<"UPDATE ticket SET seat=0 WHERE rownumber="<<row<<"AND seatnumber ="<<col;
			string update=ss.str();
			if(mysql_query(conn,update.c_str())){
			cout<<"ERROR : "<<mysql_error(conn)<<endl;
			}
			else{
				ss<<"seat is reserved successfully in row"<<row<<"AND seat number ="<<col<<endl;
			}
//			sleep(3000);
		}
		
		else if(val==2){
			exit=true;
			cout<<"GOOD LUCK"<<endl;
//			sleep(3000);
		}
		else{
			cout<<"INVALID INPUT"<<endl;
//			sleep(3000);
		}
	}
	mysql_close(conn);
	return 0;
}
