#include "config.h"
#include "first.h"

//global params
int dot_count = 0;
int dot_pos[MAX_MACHINE][2];//[0]:x,[1]:y
int fresh_time;

//data storage for WSN
int current_data_index = 0;
WsnData GWsnData[MAX_HISTORY_DATA];

//address for end_device
char all_addr[MAX_MACHINE][8];

//current_node record
int current_node = 0;

//configuration
void config()
{
	dot_count = 0;
	fresh_time = DEF_FRESH;
/*
	dot_count = 3;
	dot_pos[0][0]=100;
	dot_pos[0][1]=60;
	dot_pos[1][0]=80;
	dot_pos[1][1]=120;
	dot_pos[2][0]=200;
	dot_pos[2][1]=200;
	
	fresh_time = DEF_FRESH;
	//x_max:450,y_max:200
	for(int i = 0; i < MAX_HISTORY_DATA; i++){
		float xxx = i%10;
		GWsnData[i].temp = xxx;
		GWsnData[i].humidity = xxx;
		GWsnData[i].smoke = xxx;
		GWsnData[i].co = xxx;
		GWsnData[i].ch4 = xxx;
	}
*/
}

int main(int argc,char **argv)
{
	config();
	QApplication app(argc,argv); 
	first *drawdemo = new first(0); 
	drawdemo->setGeometry(10,20,480,320); 
	app.setMainWidget(drawdemo); 
	drawdemo->show();
	int result = app.exec();
	return result;
} 
