#include "game.h"

Game::Game()
{
  printf("constructor\n");
  quit = 0;
  gameState = GAME_STATE_INIT; //0 menu 1 start
}

Game::~Game()
{

}

void Game::timer_handler(int signum)
{
  printf("timer tick %d\n", myGame.round_cnt);
  if (myGame.round_cnt++ >= ROUND_TIMES)
  {
    myGame.gameOver = 1;
    setitimer(ITIMER_REAL, NULL, NULL);
  }
  // TODO: Update mole states
  printf("mole states: ");
  for (int i = 0; i < NUM_MOLES; i++)
  {
    myGame.moles[i] = (rand() % 2) == 0;
    printf("%d ", myGame.moles[i]);
  }
  printf("\n");

  myGame.broadcastToPlayers();
}

void Game::gameLoop()
{
  while (!myGame.quit)
  {
    switch (myGame.gameState)
    {
    case GAME_STATE_INIT:
      myGame.gameState = handleInit();
      break;
    case GAME_STATE_PLAYING:
      myGame.gameState = handlePlaying();
      break;
    case GAME_STATE_END:
      myGame.gameState = GAME_STATE_INIT;
      break;
    default:
      break;
    }
    myGame.broadcastToPlayers();
  }
}

void Game::gameStop(){
  myGame.gameOver = true;
  myGame.quit = true;
}

void Game::broadcastToPlayers()
{
  // printf("player1\n");
  // printf("score: %d\n", players[0].score);
  // printf("player2\n");
  // printf("score: %d\n", players[1].score);

  server_pkt pkt;
  memcpy(pkt.mole_states, moles, sizeof(moles));
  pkt.gameState = gameState;
  for (int i = 0; i < NUM_PLAYERS; i++)
  {
    if(fcntl(players[i].connfd, F_GETFD) == -1) continue;
    printf("writing to connfd = %d\n", players[i].connfd);
    pkt.score = players[i].score;
    write(players[i].connfd, &pkt, sizeof(pkt));
  }
}

int Game::handleInit()
{
  // Accept client connections
  int i;
  printf("binding port %d\n", SERVER_PORT);
  if(fcntl(sockfd, F_GETFD) != -1){
    close(sockfd);
  }
  sockfd = createServerSock(SERVER_PORT, TRANSPORT_TYPE_TCP);
  printf("waiting for connections...\n");
  //connect
  for (i = 0; i < NUM_PLAYERS && !this->quit; i++)
  {
    // TODO
    struct sockaddr_in cln_addr;
    socklen_t sLen = sizeof(cln_addr);
    int connfd = accept(sockfd, (struct sockaddr *)&cln_addr, &sLen);

    if (connfd == -1)
    {
      perror("Error: accept()");
    }
    myGame.players[i].connfd = connfd;
    myGame.players->score = 0;
    printf("accept connection from: %d, connfd = %d\n", cln_addr.sin_addr, myGame.players[i].connfd);
  }
  return GAME_STATE_PLAYING;
}

int Game::handlePlaying()
{
  // Setup Game environment: Game timer, Mole States, Player Threads
  myGame.sem = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | SEM_MODE);
  // sem = semget(SEM_KEY, 1, IPC_CREAT | SEM_MODE);
  if (myGame.sem < 0)
  {
    perror("Error create semaphore\n");
    semctl(myGame.sem, 0, IPC_RMID, 0);
    exit(-1);
  }
  printf("sem created\n");
  if (semctl(myGame.sem, 0, SETVAL, 1) < 0)
  {
    perror("Error semctl\n");
    exit(-1);
  }

  myGame.round_cnt = 0;
  //timer start
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = &this->timer_handler;
  sigaction(SIGALRM, &sa, NULL);

  game_timer.it_value.tv_sec = ROUND_INTERVAL;
  game_timer.it_value.tv_usec = 0;

  game_timer.it_interval.tv_sec = ROUND_INTERVAL;
  game_timer.it_interval.tv_usec = 0;
  setitimer(ITIMER_REAL, &game_timer, NULL);

  for (int i = 0; i < NUM_MOLES; i++)
  {
    moles[i] = (rand() % 8) == 0;
  }

  for (int i = 0; i < NUM_PLAYERS; i++)
  {
    pthread_t thread;
    // struct ThreadArgs* args = (struct ThreadArgs* )malloc(sizeof(args));
    // args->player = &players[i];
    // args->theGame = &myGame;
    printf("thread> id = %d\n", i);
    printf("thread> connfd = %d\n", myGame.players[i].connfd);
    if (pthread_create(&thread, NULL, (void *(*)(void *)) & (Game::thread_handler), (void *)i))
    {
      perror("Error: pthread_create()\n");
    }
  }
  while (!myGame.gameOver)
  {
  }

  printf("remove sem\n");
  if (semctl(myGame.sem, 0, IPC_RMID, 0) < 0)
  {
    perror("Error removing sem\n");
    exit(-1);
  }
  close(sockfd);
  for(int i=0; i<NUM_PLAYERS; i++){
    close(myGame.players[i].connfd);
  }

  return GAME_STATE_END;
}

void *Game::thread_handler(void *arg)
{
  int index = *((int *)(&arg));
  char msg[20];
  int key;
  printf("id = %d\n", index);
  printf("conndf = %d\n", myGame.players[index].connfd);
  while (!myGame.gameOver)
  {
    int n;
    if ((n = read(myGame.players[index].connfd, msg, 20)) == 0)
    {
      printf("Connection closed\n");
      close(myGame.players[index].connfd);
      return NULL;
    }
    // for(int i=0; i<9 ; i++){
    //   printf("%d ", (int)myGame.moles[i]);
    // }
    // printf("\n");
    if (msg[n - 1] == '\n')
      msg[n - 1] = '\0';
    else
      msg[n] = '\0';
    printf("> %s\n", msg);
    std::cout << msg << std::endl;
    key = msg[0] - '1';
    // Check key and hit moles
    myGame.P(myGame.sem);
    if (myGame.moles[key])
    {
      myGame.players[index].score++;
      myGame.moles[key] = 0;
      // write(myGame.players[index].connfd, "HIT\n", sizeof("HIT\n"));
    }
    else
    {
      // write(myGame.players[index].connfd, "MISS\n", sizeof("MISS\n"));
    }
    usleep(200000);
    myGame.V(myGame.sem);
    myGame.broadcastToPlayers();
  }
  close(myGame.players[index].connfd);
  return NULL;
}

int Game::P(int s)
{
  struct sembuf sop;
  sop.sem_num = 0;
  sop.sem_op = -1;
  sop.sem_flg = 0;

  if (semop(s, &sop, 1) < 0)
  {
    perror("P(): sem failed\n");
    return -1;
  }
  else
  {
    return 0;
  }
}

int Game::V(int s)
{
  struct sembuf sop;
  sop.sem_num = 0;
  sop.sem_op = 1;
  sop.sem_flg = 0;

  if (semop(s, &sop, 1) < 0)
  {
    perror("V(): sem failed\n");
    return -1;
  }
  else
  {
    return 0;
  }
}