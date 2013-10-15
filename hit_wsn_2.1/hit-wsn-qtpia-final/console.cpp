/****************************************************************************
** Form implementation generated from reading ui file 'console.ui'
**
** Created: Wed Feb 2 14:32:45 2011
**      by:  The User Interface Compiler (uic)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************
** xiaoyang yi@2011.5.12
** 		modify the mouse event handle
****************************************************************************
** xiaoyang yi@2011.5.27
**		add monitor
***************************************************************************/

#include "console.h"
#include "Monitor.h"

//-------------------------------------------------------------
// 	xaoyang yi@2011.5.12
//	global extern params
//--------------------------------------------------------------
extern int dot_count;//dot counts
extern int dot_pos[MAX_MACHINE][2];//all of the machine
//extern int last_dot_pos[MAX_MACHINE][2];//last position
//extern void copy_dot_pos();
extern int fresh_time;
extern int current_data_index;
extern WsnData GWsnData[MAX_HISTORY_DATA];
//address for end_device
extern char all_addr[MAX_MACHINE][17];
extern int current_node;
//-------------------------------------------------------------
// 	xaoyang yi@2011.5.12
//	global params
//--------------------------------------------------------------
//for zoom_out and zoom_in
const float scaling_size[ALL_SCALING] = {
	SMALL10,
	SMALL8,
	SMALL5 ,
	SMALL4,
	SMALL2 ,
	NORMAL,
	LARGER2,
	LARGER4,
	LARGER5,
	LARGER8,
	LARGER10
};

//-------------------------------------------------------------
// 	xaoyang yi@2011.5.12
//	images
//--------------------------------------------------------------

static const char* const image0_data[] = { 
"16 16 40 1",
"x c #769150",
"# c #779252",
"G c #789253",
"f c #789353",
"w c #799354",
"k c #799454",
"L c #7a9454",
"a c #7a9456",
". c #7b9557",
"I c #7c9557",
"i c #7c9559",
"l c #7c9657",
"r c #7d9659",
"q c #7d975b",
"b c #849c63",
"p c #8ba26d",
"K c #8ca26e",
"t c #8da370",
"n c #8fa572",
"v c #99ad80",
"c c #a0b389",
"z c #a5b790",
"B c #acbb98",
"d c #b5c4a6",
"C c #bdc9af",
"D c #c4cfb9",
"j c #cad4c1",
"E c #cbd4c2",
"A c #dae1d6",
"J c #ebeeee",
"g c #eeefef",
"F c #eef1f1",
"H c #eff1f3",
"m c #f1f3f3",
"h c #f1f3f5",
"o c #f3f3f6",
"s c #f3f5f6",
"y c #f3f5f8",
"u c #f3f5f9",
"e c #f6f6f9",
"................",
".#............a.",
"...........bca..",
"..........def...",
".........aghij.k",
".........lhmmn.k",
".........ompq..k",
".......rsmt....k",
"......aumv.....k",
"...awxymz......k",
"..aAhhmB.......k",
"..CDEmF........k",
"..a.GmH........k",
"...IJmK........k",
"....kL..........",
"..kaaaaaaaaaaa.."};

static const char* const image1_data[] = { 
"16 16 100 2",
"Qt c None",
"#E c #000000",
".J c #000b58",
".N c #001c6b",
".M c #001e6c",
".L c #001f6c",
".K c #00206c",
"#n c #010206",
"#C c #020303",
"#A c #030406",
"#y c #040404",
"#u c #050505",
"#r c #070707",
"#w c #0a0d0d",
"#H c #101010",
"#t c #111415",
"#B c #121212",
"#F c #131313",
"#x c #141414",
"#m c #515150",
"#s c #676666",
".O c #6a97cd",
".x c #70a4f5",
"#v c #777676",
"#l c #797a7d",
".H c #7bb3ff",
".R c #82aff8",
".1 c #8593b4",
"#z c #8a8989",
".n c #8b888a",
"#f c #93a4c1",
".g c #949295",
".G c #959090",
".S c #95bfff",
"#p c #999594",
"#G c #999999",
".o c #99add1",
"#o c #9a9691",
".9 c #9abff4",
"#q c #9d9a9a",
"#e c #9f9991",
"#D c #9f9f9f",
".2 c #9fc6ff",
".Q c #a1a2a5",
".T c #a1c9ff",
".0 c #a29e9e",
".I c #a3ccff",
".8 c #a5a198",
".y c #a5c9ff",
".w c #aac5f1",
".U c #abceff",
"#. c #adcdff",
".3 c #adceff",
".# c #aeaeb1",
".a c #afafaf",
".V c #b3d3ff",
".z c #b3d6ff",
"#k c #b5b2b4",
".4 c #b5d2ff",
".h c #b7caec",
".A c #b9d8ff",
".5 c #bbd6ff",
".W c #bbd7ff",
"## c #bdd7ff",
".B c #bedbff",
".q c #bfd9ff",
".c c #c1c0c8",
".r c #c1daff",
".X c #c2dcff",
"#g c #c2e0ff",
".s c #c3dbff",
".C c #c3deff",
".E c #c4ddff",
".Y c #c5dcff",
".t c #c5ddff",
"#d c #c6c4c5",
"#a c #c7deff",
".F c #c7e6ff",
".u c #c8deff",
".D c #c8e1ff",
".P c #c8e2ff",
".v c #cbdfff",
".f c #cdced5",
".p c #cde2ff",
"#h c #ceeaff",
".Z c #d4ebff",
".b c #d8d6d2",
".7 c #d9e5fa",
"#i c #d9eeff",
".6 c #dae8ff",
"#j c #dde3ee",
".l c #ddeaff",
"#b c #deebff",
".k c #dfebff",
".j c #dfecff",
".i c #e5f0ff",
".m c #f0ffff",
"#c c #f9ffff",
".d c #fcfbfc",
".e c #ffffff",
"QtQtQtQtQt.#.aQtQtQtQtQtQtQtQtQt",
"QtQt.b.c.d.e.e.f.gQtQtQtQtQtQtQt",
"QtQt.h.i.j.j.k.l.m.nQtQtQtQtQtQt",
"Qt.o.p.q.r.s.t.u.v.wQtQtQtQtQtQt",
"Qt.x.y.z.A.B.C.D.E.F.GQtQtQtQtQt",
"Qt.H.I.J.K.L.M.N.O.P.QQtQtQtQtQt",
"Qt.R.S.T.U.V.W.X.Y.Z.0QtQtQtQtQt",
"Qt.1.2.y.3.4.5.v.6.7QtQtQtQtQtQt",
"Qt.8.9#..4###a#b#c#dQtQtQtQtQtQt",
"QtQt#e#f#g#h#i#j#k#l#m#nQtQtQtQt",
"QtQtQtQt#o#p#qQtQtQt#r#s#tQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQt#u#v#wQtQt",
"QtQtQtQtQtQtQtQtQtQtQt#x#y#z#AQt",
"QtQtQtQtQtQtQtQtQtQtQtQt#B#C#D#E",
"QtQtQtQtQtQtQtQtQtQtQtQtQt#F#E#G",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQt#HQt"};

static const char* const image2_data[] = { 
"16 16 102 2",
"Qt c None",
"#G c #000000",
".K c #000b58",
".t c #001668",
".L c #001b6a",
".5 c #001c6a",
".O c #001c6b",
".N c #002370",
".C c #002470",
".W c #002671",
".M c #004a90",
"#p c #010206",
"#E c #020303",
"#C c #030406",
"#A c #040404",
"#w c #050505",
"#t c #070707",
"#y c #0a0d0d",
"#J c #101010",
"#v c #111415",
"#D c #121212",
"#H c #131313",
"#z c #141414",
"#o c #515150",
"#u c #676666",
".P c #6a97cd",
".y c #70a4f5",
"#x c #777676",
"#n c #797a7d",
".I c #7bb3ff",
".S c #82aff8",
".2 c #8593b4",
"#B c #8a8989",
".o c #8b888a",
"#h c #93a4c1",
".g c #949295",
".H c #959090",
".T c #95bfff",
"#r c #999594",
"#I c #999999",
".p c #99add1",
"#q c #9a9691",
"#. c #9abff4",
"#s c #9d9a9a",
"#g c #9f9991",
"#F c #9f9f9f",
".3 c #9fc6ff",
".R c #a1a2a5",
"#b c #a1c0ed",
".6 c #a1c2ef",
".U c #a1c9ff",
".1 c #a29e9e",
".X c #a2c4f4",
".J c #a3ccff",
".9 c #a5a198",
".z c #a5c9ff",
".D c #a9caf9",
".x c #aac5f1",
".u c #aac7ef",
"## c #adcdff",
".# c #aeaeb1",
".a c #afafaf",
".A c #b3d6ff",
"#m c #b5b2b4",
".h c #b7caec",
"#a c #b9d5ff",
".r c #bfd9ff",
".c c #c1c0c8",
".Y c #c2dcff",
"#i c #c2e0ff",
"#c c #c4dbfd",
".F c #c4ddff",
".Z c #c5dcff",
".V c #c5e3ff",
"#f c #c6c4c5",
".G c #c7e6ff",
".v c #c8deff",
".E c #c8e1ff",
".Q c #c8e2ff",
".4 c #c9e4ff",
".w c #cbdfff",
".f c #cdced5",
".q c #cde2ff",
"#j c #ceeaff",
".k c #d1dff7",
".0 c #d4ebff",
".B c #d5eeff",
".b c #d8d6d2",
".8 c #d9e5fa",
"#k c #d9eeff",
".7 c #dae8ff",
"#l c #dde3ee",
".l c #ddeafe",
".m c #ddeaff",
"#d c #deebff",
".s c #e0f2ff",
".j c #e1edff",
".i c #e5f0ff",
".n c #f0ffff",
"#e c #f9ffff",
".d c #fcfbfc",
".e c #ffffff",
"QtQtQtQtQt.#.aQtQtQtQtQtQtQtQtQt",
"QtQt.b.c.d.e.e.f.gQtQtQtQtQtQtQt",
"QtQt.h.i.j.k.l.m.n.oQtQtQtQtQtQt",
"Qt.p.q.r.s.t.u.v.w.xQtQtQtQtQtQt",
"Qt.y.z.A.B.C.D.E.F.G.HQtQtQtQtQt",
"Qt.I.J.K.L.M.N.O.P.Q.RQtQtQtQtQt",
"Qt.S.T.U.V.W.X.Y.Z.0.1QtQtQtQtQt",
"Qt.2.3.z.4.5.6.w.7.8QtQtQtQtQtQt",
"Qt.9#.###a#b#c#d#e#fQtQtQtQtQtQt",
"QtQt#g#h#i#j#k#l#m#n#o#pQtQtQtQt",
"QtQtQtQt#q#r#sQtQtQt#t#u#vQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQt#w#x#yQtQt",
"QtQtQtQtQtQtQtQtQtQtQt#z#A#B#CQt",
"QtQtQtQtQtQtQtQtQtQtQtQt#D#E#F#G",
"QtQtQtQtQtQtQtQtQtQtQtQtQt#H#G#I",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQt#JQt"};

static const char* const image3_data[] = { 
"16 16 119 2",
"Qt c None",
".x c #004d00",
".z c #005000",
".2 c #005100",
"#d c #005900",
"#b c #006900",
"#h c #006b00",
".8 c #006e00",
".f c #007000",
".L c #007900",
"#m c #007b00",
"#e c #008100",
"#i c #008200",
".U c #00a000",
".y c #034e03",
"#l c #035201",
"#0 c #035202",
".# c #036f03",
"#n c #038e01",
"#j c #058f02",
".K c #066003",
".g c #067003",
".m c #089104",
".A c #095e09",
"#y c #099105",
"#c c #0a4a04",
"#S c #0a6605",
"#W c #0bec06",
".n c #0d6606",
"#o c #0d9307",
"#. c #0e570d",
"#X c #0ee807",
".T c #0f5608",
"#a c #0f650f",
".7 c #0f900f",
".I c #104809",
"#f c #11610a",
"## c #125d12",
".F c #13520a",
".4 c #13e213",
"#g c #14470a",
".t c #14920c",
".J c #15570c",
"#w c #15990b",
"#z c #16990b",
".S c #17520c",
"#x c #185c0c",
"#p c #189a0d",
"#V c #19d70d",
".3 c #1a5b0f",
"#O c #1bd30e",
"#P c #1cd10f",
".k c #1d7d1c",
"#K c #1d9d0f",
"#k c #1e880e",
"#H c #1f9f10",
"#Y c #1fd211",
"#t c #207111",
".9 c #207b11",
".E c #209412",
"#v c #209e11",
"#A c #23a112",
"#N c #23c712",
"#Q c #26c314",
".5 c #26d226",
"#u c #27a315",
"#Z c #287e14",
".P c #289718",
".6 c #28b828",
"#s c #296816",
"#I c #299a15",
"#U c #29be15",
".Z c #2a6e29",
"#J c #2a7a16",
"#L c #2ba617",
"#G c #2ca617",
".V c #2cab2c",
"#E c #2cb817",
"#D c #2cb917",
".Q c #2d6a29",
".0 c #2e782e",
".Y c #2e911b",
".1 c #2f792f",
"#B c #2fa818",
".l c #30962f",
"#q c #30a219",
"#M c #30b319",
"#C c #31b11a",
"#F c #33ae1b",
"#r c #37851b",
"#R c #3ca91f",
".R c #3f883f",
".X c #3f9e3f",
".W c #3faa3f",
"#T c #48ad25",
".e c #4f984f",
".G c #509850",
".O c #50a450",
".N c #50aa50",
".M c #50af50",
".H c #519551",
".u c #5c9c5b",
".v c #61a761",
".D c #61ab61",
".C c #61ad61",
".B c #61ae61",
".w c #63a663",
".p c #72b772",
".r c #72b872",
".s c #75b474",
".q c #75b675",
".o c #79ba79",
".a c #7eb87e",
".d c #80c280",
".i c #83c783",
".h c #84c084",
".j c #84c784",
".b c #a2d6a2",
".c c #a7dca7",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQt.#.a.b.c.d.e.fQtQtQtQt",
"QtQtQt.g.h.i.i.i.i.i.i.j.kQt.l.m",
"QtQt.n.o.p.p.p.p.q.p.p.r.r.s.p.t",
"QtQt.u.v.v.w.x.x.y.z.A.B.B.C.D.E",
"Qt.F.G.G.H.x.IQtQt.J.K.L.M.N.O.P",
"Qt.Q.R.R.x.SQtQtQtQt.T.U.V.W.X.Y",
"Qt.Z.0.1.2QtQtQtQt.3.4.5.6.7.8.9",
"Qt#.###a#bQtQtQtQtQtQtQtQtQtQtQt",
"Qt#c#d.8#e#fQtQtQtQtQtQtQtQtQtQt",
"Qt#g#h#i#j#kQtQtQtQtQtQtQtQtQtQt",
"Qt#l#m#n#o#p#q#r#s#t#u#v#wQtQtQt",
"QtQt#x#y#z#A#B#C#D#E#F#G#H#IQtQt",
"QtQtQt#J#K#L#M#N#O#P#Q#F#RQtQtQt",
"QtQtQtQt#S#T#U#V#W#X#Y#ZQtQtQtQt",
"QtQtQtQtQtQt.2.2#0.zQtQtQtQtQtQt"};


/* 
 *  Constructs a Console which is a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f' 
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
Console::Console( QWidget* parent,  const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
{
    QPixmap image0( ( const char** ) image0_data );
    QPixmap image1( ( const char** ) image1_data );
    QPixmap image2( ( const char** ) image2_data );
    QPixmap image3( ( const char** ) image3_data );
	
	if ( !name )
	setName( "Console" );
    resize( 311, 167 ); 
    setCaption( tr( "Console" ) );
    ConsoleLayout = new QGridLayout( this ); 
    ConsoleLayout->setSpacing( 6 );
    ConsoleLayout->setMargin( 1 );
	
    Frame8 = new QFrame( this, "Frame8" );
	Frame8->setGeometry( QRect( 0, 0, 30, 102 ) ); 
    Frame8->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)0, (QSizePolicy::SizeType)0, Frame8->sizePolicy().hasHeightForWidth() ) );
    Frame8->setBackgroundOrigin( QFrame::ParentOrigin );
    Frame8->setFrameShape( QFrame::Box );
    Frame8->setFrameShadow( QFrame::Raised );
    Frame8->setLineWidth( 1 );
    Frame8->setMargin( 0 );

    PushButton6 = new QPushButton( Frame8, "PushButton6" );
    PushButton6->setGeometry( QRect( 1, 73, 28, 28 ) ); 
    PushButton6->setText( tr( "" ) );
    PushButton6->setPixmap( image0 );

    PushButton10 = new QPushButton( Frame8, "PushButton10" );
    PushButton10->setGeometry( QRect( 1, 25, 28, 28 ) ); 
    PushButton10->setText( tr( "" ) );
    PushButton10->setPixmap( image1 );

    PushButton9 = new QPushButton( Frame8, "PushButton9" );
    PushButton9->setGeometry( QRect( 1, 1, 28, 28 ) ); 
    PushButton9->setText( tr( "" ) );
    PushButton9->setPixmap( image2 );

    PushButton3 = new QPushButton( Frame8, "PushButton3" );
    PushButton3->setGeometry( QRect( 1, 49, 28, 28 ) ); 
    PushButton3->setText( tr( "" ) );
    PushButton3->setPixmap( image3 );

    //ConsoleLayout->addWidget( Frame6, 0, 0 );

    TextLabel2 = new QLabel( this, "TextLabel2" );
    TextLabel2->setSizePolicy( QSizePolicy( (QSizePolicy::SizeType)3, (QSizePolicy::SizeType)1, TextLabel2->sizePolicy().hasHeightForWidth() ) );
    TextLabel2->setMinimumSize( QSize( 0, 16 ) );
    TextLabel2->setMaximumSize( QSize( 32767, 16 ) );
    TextLabel2->setText( tr( "status:" ) );

    ConsoleLayout->addWidget( TextLabel2, 1, 0 );
	
    // signals and slots connections
    connect( PushButton3, SIGNAL( clicked() ), this, SLOT( refresh() ) );
    connect( PushButton10, SIGNAL( clicked() ), this, SLOT( zoom_out() ) );
    connect( PushButton9, SIGNAL( clicked() ), this, SLOT( zoom_in() ) );
    connect( PushButton6, SIGNAL( clicked() ), this, SLOT( setting() ) );
	
	//space between grids
	gridding_space = 10;
	scaling = NORMAL;
	scaling_index = (int)(ALL_SCALING/2);
	if_paint_gridding = true;
	data_trans_status = TRANS_IDEL;
	y_offset = 0;
	x_offset = 0;
	drag_status = DRAG_L_IDEL;
	//setBackgroundMode(NoBackground);
	
	//new a timer
	timer = new QTimer( this, "timer" ); 
	connect( timer, SIGNAL( timeout() ), this, SLOT( flush_buffer() ) ); 
	timer->start(fresh_time);//about 20ms 
		
	data_timer = new QTimer( this, "data_timer" );
	connect( data_timer, SIGNAL( timeout() ), this, SLOT( flush_data() ) ); 
	data_timer->start(60000);//about 60s  
	
	//set tooltips enable,you must enable mouse trace
	setMouseTracking(true);
	flush_data();//get all adress
}

/*  
 *  Destroys the object and frees any allocated resources
 */
Console::~Console()
{
    // no need to delete child widgets, Qt does it all for us
}

void Console::down()
{
    DEBUG_QWARNING( "Console::down(): Not implemented yet!" );
}

void Console::left()
{
    DEBUG_QWARNING( "Console::left(): Not implemented yet!" );
}

void Console::setting()
{
	QMessageBox::about( this, "QtEvent",
                        "Console::setting(): Not implemented yet!"
                        );
    DEBUG_QWARNING( "Console::setting(): Not implemented yet!" );
}

void Console::refresh()
{
	gridding_space = 10;
	scaling = NORMAL;
	scaling_index = (int)(ALL_SCALING/2);
	if_paint_gridding = true;
	y_offset = 0;
	x_offset = 0;
	drag_status = DRAG_L_IDEL;
    //DEBUG_QWARNING( "Console::refresh(): Not implemented yet!" );
}


void Console::flush_data()
{
	DataTrans data_trans;
	
	printf("flush_data dot_count=%d\r\n",dot_count);
	dot_count = 0;
	if(data_trans.get_all_addr() < 0){
		DEBUG_QWARNING("error:get all adress in flush_data");
		QMessageBox::about( this, "Connect Error",
                        "get all adress error!"
        );
		return ;
	}
	
/*
	DataTrans data_trans;
	
	printf("flush_data dot_count=%d\r\n",dot_count);
	if(data_trans.get_all_addr() < 0){
		DEBUG_QWARNING("error:get all adress in flush_data");
		return ;
	}
	
	for(int i = 0 ; i < dot_count; i++){
	//for(int i = 0 ; i < 1; i++){
		if(data_trans.get_data(i,current_data_index) > 0){
			DEBUG_PRINT("node:%d,ieee:%s,get data error!\r\n",i,all_addr[i]);
		}
	}
*/
}

void Console::right()
{
    DEBUG_QWARNING( "Console::right(): Not implemented yet!" );
}

void Console::up()
{
    DEBUG_QWARNING( "Console::up(): Not implemented yet!" );
}

void Console::zoom_in()
{
	if(scaling_index == ALL_SCALING-1) 
	{
		//LAGEST scaling,donothing
		TextLabel2->setText("warning:largest scaling!");
	}else{
		scaling_index++;
	}
    
}

void Console::zoom_out()
{
	if(scaling_index == 0) 
	{
		//LAGEST scaling,donothing
		TextLabel2->setText("warning:least scaling!");
	}else{
		scaling_index--;
	}
}

/*  
 *  Main event handler. Reimplemented to handle application
 *  font changes
 */
bool Console::event( QEvent* ev )
{
    bool ret = QDialog::event( ev ); 
	if(ev->type() == QEvent::MouseButtonPress)
	{
		/*
		QMessageBox::about( this, "Qt Mouse Click Event Example",
                        "You haved clicked the prearranged position \nand the widget will be closed."
                        );
		*/
		//DEBUG_QWARNING("event:MouseButtonPress");
		
	}
	
    return ret;
}

//-------------------------------------------------------------
// 	xaoyang yi@2011.5.12
//	Input:	void
//	Output:	void
//	Note:	when timer timeout,the slot is called
//--------------------------------------------------------------
void Console::flush_buffer()
{
	//trigger the repaint event 
	repaint(TRUE);
	
#if DEBUG
	//dot_pos[0][0]++;
	//dot_pos[0][1]++;
#endif
}

//-------------------------------------------------------------
// 	xaoyang yi@2011.5.12
//	Input:	QPainter of the Dialog object
//	Output:	void
//	Note:	paint the Rects(nodes)
//--------------------------------------------------------------
void Console::drawPrimitives(QPainter *p)
{
	QBrush b1( Qt::blue );
	QBrush b2( Qt::green, Qt::Dense6Pattern );
	QBrush b3( Qt::red );
	QBrush b4( Qt::CrossPattern );
	int rect_width = 12;
	
	p->setPen( Qt::blue );
	p->setBrush( b1 );
	//p->drawRect( 80, 80, 180, 180 );
	
	for(int i = 0 ; i < dot_count; i++)
	{
		//center=[x,y], [w,l]=[rect_width,rect_width]
		p->drawEllipse( 
				(int)(x_offset + (int)((dot_pos[i][0]-rect_width/2)*scaling) ),
				(int)(y_offset + (int)((dot_pos[i][1]-rect_width/2)*scaling) ),
				(int)( (rect_width*scaling)),
				(int)( (rect_width*scaling ))
		 );
	}
	
	//add a center dot
	p->drawEllipse( 
			(int)(x_offset + (int)((200-rect_width/2)*scaling) ),
			(int)(y_offset + (int)((40 -rect_width/2)*scaling) ),
			(int)( rect_width*scaling),
			(int)( rect_width*scaling)
	);
	
	p->setPen( Qt::red );
	p->setBrush( b3 );
	rect_width = rect_width/2;
	p->drawEllipse( 
			(int)(x_offset + (int)((200-rect_width/2)*scaling)),
			(int)(y_offset + (int)((40 -rect_width/2)*scaling)),
			(int)( rect_width*scaling),
			(int)( rect_width*scaling)
	);
	
	//DEBUG_QWARNING("test:%d,x_offset=%d,scaling=%f,%f,%d",(int)(x_offset + (int)(194*scaling)),x_offset,scaling,194*scaling,(int)(194*scaling)); 
}

//-------------------------------------------------------------
// 	xaoyang yi@2011.5.12
//	Input:	QPainter of the Dialog object
//	Output:	void
//	Note:	paint the grids
//--------------------------------------------------------------
void Console::paint_gridding(QPainter *p)
{
	int i;
	QBrush b1( Qt::gray );
	int m_x_offset = x_offset % gridding_space;
	int m_y_offset = y_offset % gridding_space;
	
	p->setPen( Qt::white );
	p->setBrush(b1);
	
	int x_count = X/gridding_space;
	int y_count = Y/gridding_space;
	
	//draw "--" line
	for(i = 0; i < y_count; i++)
	{
		p->drawLine( 0, m_y_offset + gridding_space*i, X, m_y_offset + gridding_space*i );
	}
	
	//draw "1"line
	for(i = 0; i < x_count; i++)
	{
		p->drawLine( m_x_offset + gridding_space*i, 0, m_x_offset + gridding_space*i, Y );
	}
}

//-------------------------------------------------------------
// 	xaoyang yi@2011.5.12
//	Input:	repaint
//	Output:	void
//	Note:	repaint event handle
//--------------------------------------------------------------
void Console::paintEvent( QPaintEvent *e)
{
	QPainter paint(this);
	
	scaling = scaling_size[scaling_index];
	if(if_paint_gridding)
	{
		paint_gridding(&paint);
		//if_paint_gridding = false;
	}

	drawPrimitives(&paint);
	paint.end();
}

//-------------------------------------------------------------
// 	xaoyang yi@2011.5.12
//	Input:	QPainter of the Dialog object
//	Output:	void
//	Note:	paint the grids
//--------------------------------------------------------------
void Console::mousePressEvent(QMouseEvent* e)
{
	switch(e->button())
	{
		//get the message of the point,show it on the status bar
		//	case RightButton:
		case LeftButton:
		{
			QPoint qp = e->pos();
			QString str = QString("X:");
			QString ps = "";
			
			ps = ps.setNum(e->x());
			str += ps;
	
			str += "  Y:";
			ps = "";
	
			ps = ps.setNum(e->y());
			str += ps;
			
			str += ",X2:";
			ps = ps.setNum(qp.x());
			str += ps;
			
			str += "  Y2:";
			ps = "";
	
			ps = ps.setNum(qp.y());
			str += ps;
			
			if(drag_status == DRAG_L_IDEL){
				DEBUG_QWARNING("left button pressed!");
				
				QCursor m_qc(PointingHandCursor);
				this->setCursor(m_qc);
				
				last_qp = e->pos();
				drag_status = DRAG_L_PRESSED;
				//break;
			}
		
			int rect_width = 12;
			QString detail_msg = "";
			//detail_msg = str;
			//QRect cur_rect = QRect( 0,0,0,0);
			for(int i = 0 ; i < dot_count; i++)
			{
				QRect cur_rect = QRect( 
						(int)(x_offset + (int) ( (dot_pos[i][0]-rect_width/2)*scaling)),
						(int)(y_offset + (int) ( (dot_pos[i][1]-rect_width/2)*scaling) ),
						(int)( rect_width*scaling),
						(int)( rect_width*scaling) 
				);
				TextLabel2->setText(str);
				
				if(cur_rect.contains(qp)){
					//which node
					current_node = i;
					DEBUG_PRINT("current_node=%d,pressed\r\n",current_node);
					MonitorDlg *dlg = new MonitorDlg(this,"NULL",TRUE);
					dlg->show();
					dlg->exec();
					delete dlg;
				}else{
					
				}
			}
			
			TextLabel2->setText(str);
			break;
		}
		case MidButton:
		{
			DEBUG_QWARNING("middle button pressed!");
			break;
		}
		default:
		{
			DEBUG_QWARNING("mouse just pressed!");
			break;
		}
	}
}

void Console::mouseReleaseEvent(QMouseEvent* e)
{
	e = e;
	if(drag_status == DRAG_L_PRESSED){
		QPoint qp = e->pos();
		
		QCursor m_qc(ArrowCursor);
		this->setCursor(m_qc);
		
		x_offset = x_offset + qp.x() - last_qp.x();
		y_offset = y_offset + qp.y() - last_qp.y();
		drag_status = DRAG_L_IDEL;
	}
}

//-------------------------------------------------------------
// 	xaoyang yi@2011.5.12
//	Input:	mouse Move Event handle
//	Output:	void
//	Note:	ignore
//--------------------------------------------------------------
void Console::mouseMoveEvent(QMouseEvent* e)
{
	e = e;
	/*
	QString str = QString("X:");
	QString ps = "";
	
	QPoint qp = e->pos();
			
	ps = ps.setNum(e->x());
	str += ps;
	
	str += "  Y:";
	ps = "";
	
	ps = ps.setNum(e->y());
	str += ps;
	
	str += ",X2:";
	ps = ps.setNum(qp.x());
	str += ps;
	
	TextLabel2->setText(str);
	*/
}

//-------------------------------------------------------------
// 	xaoyang yi@2011.5.13
//	Input:	
//	Output:	void
//	Note:	ignore
//--------------------------------------------------------------
void Console::resizeEvent(QResizeEvent *  e)
{
	e = e ;
}

void Console::dragEnterEvent(QDragEnterEvent* e )
{
	e = e;
	DEBUG_QWARNING("dragEnterEvent");
}

void Console::dropEvent(QDropEvent* e)
{
	e = e;
	DEBUG_QWARNING("QDropEvent");
}
//-------------------------------------------------------------
// 	xaoyang yi@2011.5.13
//	Input:	data trans funtion
//	Output:	void
//	Note:	handle the data transfer of uart1  
//--------------------------------------------------------------
int open_data_trans()
{
	
}

//-------------------------------------------------------------
// 	xaoyang yi@2011.5.13
//	Input:	
//	Output:	void
//	Note:	check tth status of data transfer  
//--------------------------------------------------------------
int check_trans_status()
{
	
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void Console::languageChange()
{
	setCaption( tr( "Console" ) );
	PushButton9->setText( QString::null );
	PushButton10->setText( QString::null );
	PushButton3->setText( QString::null );
	PushButton6->setText( QString::null );
	TextLabel2->setText( tr( "status:" ) );
}
