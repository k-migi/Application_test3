#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include <string>
#include <vector>
////自定义库

#include "sections/timer.h"

bool test();

//宏定义
#define right 0    //鱼头的朝向
#define left 1

//屏幕
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 690;
const int SCREEN_BPP = 32;

//刷新率
int FRAMES_PER_SECOND = 20;

//我的鱼的大小
const int DOT_WIDTH = 20;
const int DOT_HEIGHT = 20;

//当前状态:主菜单,游戏设置菜单,游戏执行,游戏失败,确认退出
enum{menu,set_player,set_difficulty,run_game,game_over,comfirm_quit}state;

     //图片:鱼
SDL_Surface *my_fish_right[8] ;//玩家鱼向右
SDL_Surface *my_fish_left[8] ;//鱼向左
SDL_Surface *fish[8];
      //图片:背景
SDL_Surface *main_menu = NULL;
SDL_Surface *difficulty = NULL;
SDL_Surface *player = NULL;
SDL_Surface *end = NULL;//结束语:game over
SDL_Surface *quitting = NULL;//確認退出的界面
SDL_Surface *background  =  NULL;//大海背景
     //图片:鼠标按钮
SDL_Surface *buttonSheet = NULL;
     //屏幕指针
SDL_Surface *screen = NULL;


    //quit是结束整个游戏;over是结束本轮游戏,可选择进入下一轮
    bool quit = false;
    bool over = true;

//The event structure
SDL_Event event;

/////////////////////////////////////////////////////////////////////////
////--------------------------图片---------------------------////////////
void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination, SDL_Rect* clip = NULL )
{
    //Holds offsets
    SDL_Rect offset;

    //Get offsets
    offset.x = x;
    offset.y = y;

    //Blit
    SDL_BlitSurface( source, clip, destination, &offset );
}

SDL_Surface *load_image( std::string  filename )
{
    //The image that's loaded
    SDL_Surface* loadedImage = NULL;

    //The optimized surface that will be used
    SDL_Surface* optimizedImage = NULL;

    //Load the image
    loadedImage = SDL_LoadBMP( filename.c_str() );

    //If the image loaded
    if( loadedImage != NULL )
    {
        //Create an optimized surface
        optimizedImage = SDL_DisplayFormat( loadedImage );

        //Free the old surface
        SDL_FreeSurface( loadedImage );

        //If the surface was optimized
        if( optimizedImage != NULL )
        {
            //Color key surface

            SDL_SetColorKey( optimizedImage, SDL_SRCCOLORKEY, SDL_MapRGB( optimizedImage->format, 0xFF, 0xFF, 0xFF ) );

        }
    }

    //Return the optimized surface
    return optimizedImage;
}

bool init()
{
    //Initialize all SDL subsystems
    if( SDL_Init( SDL_INIT_EVERYTHING ) == -1 )
    {
        return false;
    }

    //Set up the screen
    screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

    //If there was an error in setting up the screen
    if( screen == NULL )
    {
        return false;
    }

    //Set the window caption
    SDL_WM_SetCaption( "fish fight", NULL );

    //If everything initialized fine
    return true;
}

bool load_files()
{
    //Load the my_fish_right image

    my_fish_right[1] = load_image( "pictures/my_fish_right1.bmp" );
    my_fish_left[1]  =  load_image(  "pictures/my_fish_left1.bmp");
    my_fish_right[2] = load_image( "pictures/my_fish_right2.bmp" );
    my_fish_left[2]  =  load_image(  "pictures/my_fish_left2.bmp");
    my_fish_right[3] = load_image( "pictures/my_fish_right3.bmp" );
    my_fish_left[3]  =  load_image(  "pictures/my_fish_left3.bmp");
    my_fish_right[4] = load_image( "pictures/my_fish_right4.bmp" );
    my_fish_left[4]  =  load_image(  "pictures/my_fish_left4.bmp");
    my_fish_right[5] = load_image( "pictures/my_fish_right5.bmp" );
    my_fish_left[5]  =  load_image(  "pictures/my_fish_left5.bmp");
    my_fish_right[6] = load_image( "pictures/my_fish_right6.bmp" );
    my_fish_left[6]  =  load_image(  "pictures/my_fish_left6.bmp");
    my_fish_right[7] = load_image( "pictures/my_fish_right7.bmp" );
    my_fish_left[7]  =  load_image(  "pictures/my_fish_left7.bmp");

    fish[1]= load_image( "pictures/fish1.bmp");
    fish[2] = load_image( "pictures/fish2.bmp");
    fish[3] = load_image( "pictures/fish3.bmp");
    fish[4] = load_image( "pictures/fish4.bmp");
    fish[5]= load_image( "pictures/fish5.bmp");
    fish[6]= load_image( "pictures/fish6.bmp");
    fish[7]= load_image( "pictures/fish7.bmp");

    buttonSheet = load_image( "pictures/button.bmp" );
    background  =  load_image("pictures/background.bmp");
    main_menu = load_image("pictures/main_menu.bmp");
    difficulty = load_image("pictures/difficulty.bmp");
    player = load_image("pictures/player.bmp");
    end = load_image("pictures/game_over.bmp");
    quitting = load_image("pictures/quit.bmp");


    //If there was a problem in loading the dotbm
    if(   (my_fish_right[1] != NULL)&&(my_fish_left[1]  !=  NULL) &&(main_menu!=NULL)&&(end!=NULL)&&(buttonSheet!=NULL)   )
    //If S loaded fine
      return true;
}

void clean_up()
{
    //Free the surface


		SDL_FreeSurface( screen );
		SDL_FreeSurface( background );

		SDL_FreeSurface( main_menu );
		SDL_FreeSurface( end );
		for(int i =1;i<8;i++)
		 {
		 	 SDL_FreeSurface( fish[i] );
		 	 SDL_FreeSurface( my_fish_right[i] );
             SDL_FreeSurface( my_fish_left[i] );
		 }
    //Quit SDL
    SDL_Quit();
}
///////////////////////////////////////////
//------碰撞检测------------------------////////////////////
bool check_collision( std::vector<SDL_Rect> &A, std::vector<SDL_Rect> &B )
{
    //The sides of the rectangles
    int leftA, leftB;
    int rightA, rightB;
    int topA, topB;
    int bottomA, bottomB;

    //Go through the A boxes
    for( int Abox = 0; Abox < A.size(); Abox++ )
    {
        //Calculate the sides of rect A
        leftA = A[ Abox ].x+5;
        rightA = A[ Abox ].x + A[ Abox ].w;
        topA = A[ Abox ].y;
        bottomA = A[ Abox ].y + A[ Abox ].h;

        //Go through the B boxes
        for( int Bbox = 0; Bbox < B.size(); Bbox++ )
        {
            //Calculate the sides of rect B
            leftB = B[ Bbox ].x+5;
            rightB = B[ Bbox ].x + B[ Bbox ].w;
            topB = B[ Bbox ].y;
            bottomB = B[ Bbox ].y + B[ Bbox ].h;

            //If no sides from A are outside of B
            if( ( ( bottomA <= topB ) || ( topA >= bottomB ) || ( rightA <= leftB ) || ( leftA >= rightB ) ) == false )
            {
                //A collision is detected
                return true;
            }
        }
    }
    //If neither set of collision boxes touched
    return false;
}

////////////////////////////////////////////////////////////////////////
// //////---------鱼的类定义-------------------------------//////////////

class Fish
{
    private:
          //鱼的大小
         int size;
         //blood增加到5次就造成size的增加一次
         int blood;
         //The X and Y offsets of the fish
        int x, y;

         //鱼的方向
         int direction;

        //The velocity of the dot
        int xVel, yVel;

            //碰撞检测盒的坐标及大小
        std::vector<SDL_Rect> box;
        //移动检测盒
			    void shift_boxes();

    public:
        //Initializes the variables
        Fish();

        //Takes key presses and adjusts the my_fish's velocity
        void handle_input();

        //移动及检测
        void move();

        //显现鱼
        void show();

        //设置碰撞盒
        void set_box();

        //鱼长大
        void grow();

        //死亡
        void die();

        //获取鱼的大小值
        int get_size();

    //获取鱼的坐标及
       std::vector<SDL_Rect> &get_rects();
};

//构造函数
Fish::Fish()
{
	  //鱼的大小,一开始为1
	    size = 1;
		blood = 0;

	  //碰撞盒乘的倍数
	  //int wit=3,hei=2;

    //Initialize the offsets
    x = SCREEN_WIDTH/2;
    y = SCREEN_HEIGHT/2;

    //initialize the direction
    direction  =  right;
    //Initialize the velocity
    xVel = 0;
    yVel = 0;

	  set_box();
}

void Fish::set_box()
{
	        //Create the necessary SDL_Rects
    box.resize( 1 );

    //大小乘一个倍数(方便而已)

    //Initialize the collision boxes' width and height

    if(size==1)
    {
       box[0].w=125;
       box[0].h=63;
    }
    else if(size==2)
    {
       box[0].w=150;
       box[0].h=76;
    }
    else if(size==3)
    {
       box[0].w=175;
       box[0].h=89;
    }
    else if(size==4)
    {
       box[0].w=200;
       box[0].h=101;
    }
    else if(size==5)
    {
       box[0].w=225;
       box[0].h=114;
    }
    else if(size==6)
    {
       box[0].w=250;
       box[0].h=126;
    }
    else if(size==7)
    {
       box[0].w=275;
       box[0].h=139;
    }

    //Move the collision boxes to their proper spot
    shift_boxes();
}

void Fish::shift_boxes()
{
    //The row offset
    int r = 0;

    //Go through the dot's collision boxes
    for( int set = 0; set < box.size(); set++ )
    {
        //Center the collision box
        box[ set ].x = x ;

        //Set the collision box at its row offset
        box[ set ].y = y ;

        //Move the row offset down the height of the collision box
        r += box[ set ].h;
    }
}

void Fish::handle_input()
{
    //If a key was pressed
    if( event.type == SDL_KEYDOWN )
    {
        //Adjust the velocity
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: yVel -= DOT_HEIGHT / 2; break;
            case SDLK_DOWN: yVel += DOT_HEIGHT / 2; break;
            case SDLK_LEFT: xVel -= DOT_WIDTH / 2;
                                           direction  =  left;   break;//////按下左键时，鱼向左
            case SDLK_RIGHT: xVel += DOT_WIDTH / 2;
                                             direction  =  right;  break;///////按下右键时，鱼向右
        }
    }
    //If a key was released
    else if( event.type == SDL_KEYUP )
    {
        //Adjust the velocity
        switch( event.key.keysym.sym )
        {
            case SDLK_UP: yVel += DOT_HEIGHT / 2; break;
            case SDLK_DOWN: yVel -= DOT_HEIGHT / 2; break;
            case SDLK_LEFT: xVel += DOT_WIDTH / 2;
                                                        break;
            case SDLK_RIGHT: xVel -= DOT_WIDTH / 2;
                                                       break;
        }
    }
}

void Fish::move()
{
    //Move the dot left or right
    x += xVel;
    shift_boxes();

    //If the my_fisht went too far to the left or right
    if( ( x < 0 ) || ( x + DOT_WIDTH > SCREEN_WIDTH ) )
    {
        //move back
        x -= xVel;
        shift_boxes();
    }

    //Move the dot up or down
    y += yVel;
    shift_boxes();

    //If the my_fish_right went too far up or down
    if( ( y < -25 ) || ( y + DOT_HEIGHT > SCREEN_HEIGHT -30) )
    {
        //move back
        y -= yVel;
        shift_boxes();
    }
}


void Fish::grow()
{
	 blood++;
	if(blood == 5)
	{
		blood = 0;
		size++;
	}
    if(size>7)
    {
        size=7;
    }

}

void Fish::die()
{
       xVel = 0;
       yVel = 0;
}

int Fish::get_size()
{
	//返回鱼的大小
	return size;
	}

std::vector<SDL_Rect> &Fish::get_rects()
{
    //Retrieve the collision boxes
    return box;
}

void Fish::show()
{
    //Show the my_fish
  if(size==1)
  {
    if(direction==right)
              apply_surface( x, y, my_fish_right[1], screen );
    if(direction==left)
              apply_surface(x,y,my_fish_left[1],screen  );
  }
  else if(size==2)
  {
    if(direction==right)
              apply_surface( x, y, my_fish_right[2], screen );
    if(direction==left)
              apply_surface(x,y,my_fish_left[2],screen  );
  }
  else if(size==3)
  {
    if(direction==right)
              apply_surface( x, y, my_fish_right[3], screen );
    if(direction==left)
              apply_surface(x,y,my_fish_left[3],screen  );
  }
  else if(size==4)
  {
    if(direction==right)
              apply_surface( x, y, my_fish_right[4], screen );
    if(direction==left)
              apply_surface(x,y,my_fish_left[4],screen  );
  }
  else if(size==5)
  {
    if(direction==right)
              apply_surface( x, y, my_fish_right[5], screen );
    if(direction==left)
              apply_surface(x,y,my_fish_left[5],screen  );
  }
  else if(size==6)
  {
    if(direction==right)
              apply_surface( x, y, my_fish_right[6], screen );
    if(direction==left)
              apply_surface(x,y,my_fish_left[6],screen  );
  }
  else if(size==7)
  {
    if(direction==right)
              apply_surface( x, y, my_fish_right[7], screen );
    if(direction==left)
              apply_surface(x,y,my_fish_left[7],screen  );
  }
}

////////////////////////////////////////////////
////-----其他鱼--------------/////////////////////////////
class other_fish
{
    private:
           //其他鱼的大小
           int size;

           //位置
            int x, y;

        //The velocity of the dot
            int xVel, yVel;

           //碰撞检测盒
            std::vector<SDL_Rect> box;
            //移动检测盒
			    void shift_boxes();

    public:
        other_fish();

        //移动及检测
        void move();

        //显现鱼
        void show();

        //建立碰撞盒
        void set_box();

        //死亡
        void die();

        //获取其他鱼的大小
        int get_size();

        //获取鱼的碰撞盒
        std::vector<SDL_Rect> &get_rects();
};


//构造函数
other_fish::other_fish()
{
	   size = rand()%7+1;

	    xVel = rand()%5+1;
			yVel = rand()%3 - rand()%6;

	    x = rand()%70-200;
	    y = rand()%1000-70;

      set_box();

}

//设置碰撞盒的参数
void other_fish::set_box()
{
		        //Create the necessary SDL_Rects
    box.resize( 1 );

    if(size==1)
    {
      box[0].w=125;
      box[0].h=33;
    }
    else if(size==2)
    {
        box[0].w=150;
        box[0].h=85;
    }
    else if(size==3)
    {
        box[0].w=175;
        box[0].h=90;
    }
    else if(size==4)
    {
        box[0].w=200;
        box[0].h=101;
    }
    else if(size==5)
    {
        box[0].w=225;
        box[0].h=114;
    }
    else if(size==6)
    {
        box[0].w=250;
        box[0].h=126;
    }
    else if(size==7)
    {
        box[0].w=275;
        box[0].h=140;
    }

    //Move the collision boxes to their proper spot
    shift_boxes();
}

void other_fish::shift_boxes()
{
    //The row offset
    int r = 0;

    //Go through the dot's collision boxes
    for( int set = 0; set < box.size(); set++ )
    {
        //Center the collision box
        box[ set ].x = x ;

        //Set the collision box at its row offset
        box[ set ].y = y ;

        //Move the row offset down the height of the collision box
        r += box[ set ].h;
    }
}


void other_fish::move()
{

    //Move the dot left or right

    x += xVel;
    shift_boxes();
    y += yVel;
    shift_boxes();

    if(x>SCREEN_WIDTH)//走到尽头后再走一次
         {
            die();
         }

}


void other_fish::show()
{
      if(size==1)
              apply_surface( x, y, fish[1], screen );
			else if(size==2)
              apply_surface( x, y, fish[2], screen );
			else if(size==3)
              apply_surface( x, y, fish[3], screen );
			else if(size == 4)
              apply_surface( x, y, fish[4], screen );
			else if(size == 5)
			          apply_surface( x, y, fish[5], screen );
			else if(size == 6)
			          apply_surface( x, y, fish[6], screen );
			else if(size == 7)
			          apply_surface( x, y, fish[7], screen );


}

void other_fish::die()
{
					 x = -300;
           y = rand()%400-50;

					xVel = rand()%5+1;
					yVel = rand()%2 - rand()%2;

					size = rand()%7;//随机改变类型

					set_box();
}

int other_fish::get_size()
{
	//返回大小
	return size;
}

std::vector<SDL_Rect> &other_fish::get_rects()
{
    //Retrieve the collision boxes
    return box;
}

//The button
class Button
{
    private:
    //The attributes of the button
    SDL_Rect box;

    public:
    //Initialize the variables
    Button( int x, int y, int w, int h );

    //Handles events and set the button's sprite region
    bool handle_events();

    //Shows the button on the screen
    void show();
};





Button::Button( int x, int y, int w, int h )
{
    //Set the button's attributes
    box.x = x;
    box.y = y;
    box.w = w;
    box.h = h;

}

bool Button::handle_events()
{
    //The mouse offsets
    int x = 0, y = 0;

    //If the mouse moved
    if( event.type == SDL_MOUSEMOTION )
    {
        //Get the mouse offsets
        x = event.motion.x;
        y = event.motion.y;

        //If the mouse is over the button
        if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
        {
            show();
        }

    }
    //If a mouse button was pressed
    if( event.type == SDL_MOUSEBUTTONDOWN )
    {
        //If the left mouse button was pressed
        if( event.button.button == SDL_BUTTON_LEFT )
        {
            //Get the mouse offsets
            x = event.button.x;
            y = event.button.y;

            //If the mouse is over the button
            if( ( x > box.x ) && ( x < box.x + box.w ) && ( y > box.y ) && ( y < box.y + box.h ) )
            {
                //Set the button sprite
                 show();
                return true;
            }
        }
    }

    return false;
}

void Button::show()
{
    //Show the button
    apply_surface( box.x-20, box.y, buttonSheet, screen);

}


///////////////////////////////////////////////////////////
//-----------------------操作游戏流程的函数-----------/////////////
//////////////////////////////////////////////////////////

//显示主菜单
int Main_menu();

int Set_difficulty();

int Run_game();

int Game_over();

int Comfirm_quit();
////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////
//--------------------------------主程序-------------------------//////////
///////////////////////////////////////////////////////////////////
int main( int argc, char* args[] )
{

    //Initialize
    if( init() == false )
            {
                return 1;
            }

    //Load the files
    if( load_files() == false )
            {
               return 1;
            }

    //大框架开始了!!
    while(1)
    {
       if(quit==false)
 	 switch(state)
	     {
	     	   case menu :
	     	                                    Main_menu();
	     	                                    break;
	     	   case set_difficulty:
                                             Set_difficulty();
                                            break;
	     	   case run_game:
	     	                       //             if(player==1)
	     	                     //                         Run_game(1);
	     	                      //              if(player==2)
	     	                                              Run_game();
	     	                                      if(state == run_game)
	     	                                      {
	     	                                            state = game_over;
	     	                                      }
	     	                                     break;
	     	   case game_over:
                                           Game_over();
                                           break;
	     	   case comfirm_quit:

	     	                                   break;

	     }

             //清空内存并退出
							if(quit == true)
                    {
                    	 Comfirm_quit();
                     }

    }

}




///////////////////////////////////////////////////////////
//-----------------------操作游戏流程的函数-----------/////////////
//////////////////////////////////////////////////////////

int Main_menu()
{
	           //鼠標按鈕
    Button start_button( 424, 137, 159, 67 );//橫坐標,縱坐標,寬,高
		Button difficulty_button( 347, 244, 325, 102 );
		Button quit_button( 391, 379, 225, 107 );


	                //开始界面
             apply_surface( 0, 0, main_menu, screen );


        if( SDL_PollEvent( &event ) )
        {
            //如果鼠標點了"開始"
            if(start_button.handle_events() ==true )
                    {
                    	  over = false;
                    	  state = run_game;
                    }

						  //如果點了"難度按鈕"
						if(difficulty_button.handle_events() ==true)
						       {
						       	   state = set_difficulty;
						       }
             //如果鼠標點了"結束"
            if(quit_button.handle_events() ==true )
                  {
                  	  quit = true;
                  }

            //If the user has Xed out the window
            if( event.type == SDL_QUIT )
            {
                //Quit the program
                quit = true;
            }

                        if( SDL_Flip( screen ) == -1 )
                                {
                                    return 1;
                                }
        }
}

int Set_difficulty()
{
	  Button easy_button( 389, 138, 232, 80 );//橫坐標,縱坐標,寬,高
		Button middle_button( 368, 253, 277, 94 );
		Button crazy_button( 389, 392, 235, 82 );
		Button quit_button( 424, 513, 170, 92 );

         apply_surface( 0, 0, difficulty, screen );

            if( SDL_PollEvent( &event ) )
        {
                if(easy_button.handle_events() ==true )
                     {
                     	  FRAMES_PER_SECOND = 20;
                    	  state = menu;
                     }
                if(middle_button.handle_events() ==true )
                     {
                     	  FRAMES_PER_SECOND = 25;
                    	  state = menu;
                     }
                if(crazy_button.handle_events() ==true )
                     {
                     	  FRAMES_PER_SECOND = 50;
                    	  state = menu;
                     }
                if(quit_button.handle_events() ==true )
                     {
                    	  quit = true;
                     }
                if( event.type == SDL_QUIT )
                      {
                        //Quit the program
                               quit = true;
											}
      				if( SDL_Flip( screen ) == -1 )
								 {
													return 1;
							   }
        }
}


int Run_game()
{
    //The frame rate regulator
    Timer fps;

    //玩家控制的鱼
    Fish myFish;
    //其他鱼
    other_fish fish[8];

	             while( over == false )
									{
               //Start the frame timer
                        fps.start();

		 					     //While there's events to handle
                        while( SDL_PollEvent( &event ) )
                          {
                                   //Handle events for the dot
                                    myFish.handle_input();

                                    //If the user has Xed out the window
                                    if( event.type == SDL_QUIT )
                                      {
                                        //Quit the program
                                        Comfirm_quit();
                                      }
                         }

										//逐个检测碰撞
                        for(int i=1;i<8;i++)
                          {
        	                  //如果检测到有碰撞发生
                            if(check_collision(myFish.get_rects() , fish[i].get_rects() )==1   )
                            {
																					if(myFish.get_size()  >=  fish[i].get_size() )
                                           {
                                               fish[i].die();
                                               myFish.grow();
                                           }
                                        else
                                           {
                   	                           myFish.die();
                   	                           over = true;
                                           }
														}

                        //移动玩家的鱼
                        myFish.move();
                        //移动其他鱼
                        for(int i=1;i<8;i++)
                                {
                                    fish[i].move();
                                }

                        //Fill the screen white
                        apply_surface(0,0,background,screen);

                        //Show the my_fish on the screen
                        myFish.show();

                        for(int i=1;i<8;i++)
                                {
                                    fish[i].show();
                                }

                        //Update the screen
                        if( SDL_Flip( screen ) == -1 )
                                {
                                    return 1;
                                }

        //Cap the frame rate
                        if( fps.get_ticks() < 1000 / FRAMES_PER_SECOND )
                                {
                                    SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.get_ticks() );
                                }
                    }

                 if(over == true)
                   {
												apply_surface( 0, 0, end, screen );
                   	    myFish.die();
                     for(int i=1;i<8;i++)
                       {
                       	   fish[i].die();
                       }
                        if( SDL_Flip( screen ) == -1 )
                                {
                                    return 1;
                                }
                   }

             }
    //游戏结束
}

int Game_over()
{
			 apply_surface( 0, 0, end, screen );

   	    Button restart_button( 284, 255, 460, 98 );
   	    Button menu_button( 298, 373, 412, 116 );
	    	Button quit_button( 406, 516, 202, 85 );

        if( SDL_PollEvent( &event ) )
        {
                if(menu_button.handle_events() ==true )
                     {
                    	  state = menu;
                     }
                if(restart_button.handle_events() ==true )
                     {
                    	  over = false;
                    	  state = run_game;
                     }
                if(quit_button.handle_events() ==true )
                     {
                    	  quit = true;
                     }
                if( event.type == SDL_QUIT )
                      {
                        //Quit the program
                               quit = true;

											}
      				if( SDL_Flip( screen ) == -1 )
								 {
													return 1;
							   }
        }
}

int Comfirm_quit()
{

   	    Button continue_button( 365, 250, 330, 102 );
   	    Button menu_button( 394, 382, 250, 95 );
	    	Button quit_button( 427, 504, 207, 90 );
     while(1)
     {
     	 	apply_surface( 0, 0, quitting, screen );

        if( SDL_PollEvent( &event ) )
        {
                if(menu_button.handle_events() ==true )
                     {
                     	  quit = false;
                     	  over = true;
                    	  state = menu;
                    	  break;
                     }
                if(continue_button.handle_events() ==true )
                     {
                     	  over = false;
                    	  quit = false;
                    	  break;
                     }
                if(quit_button.handle_events() ==true )
                     {
                    	  clean_up();
                    	  return 0;
                     }

      	if( SDL_Flip( screen ) == -1 )
								 {
													return 1;
							   }
         }
     }
}

bool test()
{
       apply_surface( SCREEN_WIDTH/3, SCREEN_HEIGHT/3, end, screen );
	                          if( SDL_Flip( screen ) == -1 )
                                {
                                    return 1;
                                }
}
