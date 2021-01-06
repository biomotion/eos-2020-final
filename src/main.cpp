#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/time.h>
#include<sys/types.h>
#include<signal.h>
#include<iostream>
#include"main.h"

using namespace std;

Game::Game(){
	quit = false;
	over = false;
	gameState = 0;  //0 menu 1 start

	score = 0;


	moles[0] = new mole();
	moles[1] = new mole();
	moles[2] = new mole();
	moles[3] = new mole();
	moles[4] = new mole();
	moles[5] = new mole();
	moles[6] = new mole();
	moles[7] = new mole();
	moles[8] = new mole();
}

Game::~Game(){
}

void timer_handler(int signum){
	over = true;
}

void Game::gameLoop(){
	while(!quit){
		if(gameState==0){ //menu
			score = 0;
			cout << "Wait for connect" << endl;
			//connect
			gameState = 1;
			miss = 9;
			over = false;
			//timer
			struct sigaction sa;
			struct itimerval timer;
			memset(&sa,0,sizeof(sa));
			sa.sa_handler = &timer_handler;
			sigaction(SIGALRM,&sa,NULL);
			
			timer.it_value.tv_sec = 60;
			timer.it_value.tv_usec = 0;
			
			timer.it_interval.tv_sec = 60;
			timer.it_interval.tv_usec = 0;
		}

		else if(gameState==1){  //start
			//timer start
			setitimer(ITIMER_REAL, &timer,NULL);

			for(int i=0; i<9; i++){
				// if mole is inside, randomly decide if it should come out
				if(moles[i]->show == false){
					if(rand()%2000+1 == 1){
						moles[i]->show = true;
						moles[i]->waitTime = 3000;
					}
				}
				else{
					// if wait time is over
 					if(moles[i]->waitTime == 0){
 						// and mole has not been hit, take away a life
 						if(moles[i]->show == true){
 							score--;	
 						}
 						// return underground
						moles[i]->show = false;
					}
					else
						moles[i]->waitTime--;
				}

			if(keyboard_click){   //fix
				if(moles[key]->show == true){
					score++;
					moles[key]->show = false;
					moles[key]->waitTime = 0;
				}
			}
		}
			if(over){
				gameState = 0;
			}
		}

	}

int main( int argc, char* argv[] ) { 
	// set up
	Game game;
	// play
	game.gameLoop();

	return 0;
} 




























	}

}
