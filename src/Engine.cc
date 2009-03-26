/*
 * =====================================================================================
 *
 *       Filename:  Engine.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2009年02月14日 16时58分36秒 CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  lerosua (), lerosua@gmail.com
 *        Company:  Cyclone
 *
 * =====================================================================================
 */

#include "Engine.h"
#include <string.h>
#include <stdlib.h>
Engine::Engine():black_player(0)
{
	clean_board();
}

Engine::~Engine()
{}

void Engine::clean_board()
{
	int i,j;
	for(i=0;i<16;i++)
		for(j=0;j<16;j++)
			chessboard[i*16+j] =0;

	for(i=0;i<3;i++)
		for(j=0;j<16;j++)
			chessmans[i*16 + j]=0;

}
void Engine::reset()
{
	
	clean_board();

	black_player = 0;
	fen_snapshots.clear();
	move_snapshots.clear();
	move_chinese.clear();
	move_comment.clear();
}

void Engine::add_piece(int sq,int pc)
{
	chessboard[sq]=pc;
	chessmans[pc]=sq;
}

void Engine::add_comment(const std::string& str)
{
	int step=how_step();
	/** 直接赋值据说性能比较差*/
	//move_comment[step] = str;
	move_comment.insert(std::map<int,std::string>::value_type(step,str));
}
std::string* Engine::get_comment(int f_step)
{
	std::map<int,std::string>::iterator iter = move_comment.find(f_step);
	if(iter == move_comment.end()){

		/** 没找到 */
		return NULL;
	}
	else{
		return &(iter->second);
	}
}

void Engine::from_fens(const char *szFen) {
  int i, j, k;
  int pcRed[7];
  int pcBlack[7];
  const char *lpFen;
  // FEN串的识别包括以下几个步骤：
  // 1. 初始化，清空棋盘
  pcRed[0] = side_tag(0) + KING_FROM;
  pcRed[1] = side_tag(0) + ADVISOR_FROM;
  pcRed[2] = side_tag(0) + BISHOP_FROM;
  pcRed[3] = side_tag(0) + KNIGHT_FROM;
  pcRed[4] = side_tag(0) + ROOK_FROM;
  pcRed[5] = side_tag(0) + CANNON_FROM;
  pcRed[6] = side_tag(0) + PAWN_FROM;
  for (i = 0; i < 7; i ++) {
    pcBlack[i] = pcRed[i] + 16;
  }
  /* 数组"pcRed[7]"和"pcBlack[7]"分别代表红方和黑方每个兵种即将占有的序号，
   * 以"pcRed[7]"为例，由于棋子16到31依次代表“帅仕仕相相马马车车炮炮兵兵兵兵兵”，
   * 所以最初应该是"pcRed[7] = {16, 17, 19, 21, 23, 25, 27}"，每添加一个棋子，该项就增加1，
   * 这种做法允许添加多余的棋子(例如添加第二个帅，就变成仕了)，但添加前要做边界检测
   */
//  ClearBoard();
  lpFen = szFen;
  if (*lpFen == '\0') {
 //   SetIrrev();
    return;
  }
  // 2. 读取棋盘上的棋子
  i = RANK_TOP;
  j = FILE_LEFT;
  while (*lpFen != ' ') {
    if (*lpFen == '/') {
      j = FILE_LEFT;
      i ++;
      if (i > RANK_BOTTOM) {
        break;
      }
    } else if (*lpFen >= '1' && *lpFen <= '9') {
      for (k = 0; k < (*lpFen - '0'); k ++) {
        if (j >= FILE_RIGHT) {
          break;
        }
        j ++;
      }
    } else if (*lpFen >= 'A' && *lpFen <= 'Z') {
      if (j <= FILE_RIGHT) {
        k = fen_to_piece(*lpFen);
        if (k < 7) {
          if (pcRed[k] < 32) {
            //if (this->ucsqchessmans[pcRed[k]] == 0) {
              add_piece(get_coord(j, i), pcRed[k]);
              pcRed[k] ++;
            //}
          }
        }
        j ++;
      }
    } else if (*lpFen >= 'a' && *lpFen <= 'z') {
      if (j <= FILE_RIGHT) {
        k = fen_to_piece(*lpFen);
        if ((k>6)&&(k < 14)) {
          if (pcBlack[k-7] < 48) {
		  //printf("pcBlack[%d-7] = %d \n",k,pcBlack[k-7]);
            if (chessmans[pcBlack[k-7]] == 0) {
              add_piece(get_coord(j, i), pcBlack[k-7]);
              pcBlack[k-7] ++;
            }
          }
        }
        j ++;
      }
    }
    lpFen ++;
    if (*lpFen == '\0') {
      //SetIrrev();
      return;
    }
  }
  lpFen ++;
  // 3. 确定轮到哪方走
  if (this->black_player == (*lpFen == 'b' ? 0 : 1)) {
    change_side();
  }
//  // 4. 把局面设成“不可逆”
//  SetIrrev();
}

void Engine::to_fens(char *szFen)  {
  int i, j, k, pc;
  char *lpFen;

  lpFen = szFen;
  for (i = RANK_TOP; i <= RANK_BOTTOM; i ++) {
    k = 0;
    for (j = FILE_LEFT; j <= FILE_RIGHT; j ++) {
      pc = chessboard[get_coord(j, i)];
      if (pc != 0) {
        if (k > 0) {
          *lpFen = k + '0';
          lpFen ++;
          k = 0;
        }
        *lpFen = piece_to_fen(get_chessman_type(pc));
        lpFen ++;
      } else {
        k ++;
      }
    }
    if (k > 0) {
      *lpFen = k + '0';
      lpFen ++;
    }
    *lpFen = '/';
    lpFen ++;
  }
  *(lpFen - 1) = ' '; // 把最后一个'/'替换成' '
  *lpFen = (this->black_player == 0 ? 'w' : 'b');
  lpFen ++;
  *lpFen = '\0';
}

void Engine::init_snapshot(const char* fen)
{
	reset();
	from_fens(fen);
	fen_snapshots.push_back(std::string(fen));
	move_snapshots.push_back(0);

	/*
	int i,j;
	for(i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
			printf(" %2d ",chessboard[i*16+j]);
		printf("\n");
	}
	*/
}

void Engine::get_snapshot(int num)
{
	std::string fens = fen_snapshots[num];
	//DLOG("get_snapshot = %s\n",fens.c_str());
	clean_board();
	from_fens(fens.c_str());

}
int Engine::get_piece(int rx,int ry)
{
	int site=0;
	site = get_coord(rx + 3,ry + 3);
	return chessboard[site];

}
int Engine::get_dst_xy(int rx, int ry)
{
	return get_coord(rx+3,ry+3);
}

char Engine::get_iccs_y(int nArg)
{
	int tmp = (nArg&240)>>4;
	switch(tmp){
	case 3:
		return '9';
	case 4:
		return '8';
	case 5:
		return '7';
	case 6:
		return '6';
	case 7:
		return '5';
	case 8:
		return '4';
	case 9:
		return '3';
	case 10:
		return '2';
	case 11:
		return '1';
	case 12:
		return '0';
	default:
		return -1;
		};
}
char Engine::get_iccs_x(int nArg)
{
	return (nArg&15)+94;
}

/** iccs中的x坐标转成中文纵线的x坐标（红黑方坐标相反)*/
char Engine::alpha_to_digit(int nArg)
{
	if(black_player){
		return nArg-48;
	}
	else{
	switch(nArg){
		case 'i':
			return '1';
		case 'h':
			return '2';
		case 'g':
			return '3';
		case 'f':
			return '4';
		case 'e':
			return '5';
		case 'd':
			return '6';
		case 'c':
			return '7';
		case 'b':
			return '8';
		case 'a':
			return '9';
		default:
			return -1;
		}
	}


}
char Engine::digit_to_alpha(int nArg)
{
	if(!black_player){
	switch(nArg){
		case '1':
			return 'i';
		case '2':
			return 'h';
		case '3':
			return 'g';
		case '4':
			return 'f';
		case '5':
			return 'e';
		case '6':
			return 'd';
		case '7':
			return 'c';
		case '8':
			return 'b';
		case '9':
			return 'a';
		default:
			return -1;
	}
	}
	else{
		switch(nArg){
		case '1':
			return 'a';
		case '2':
			return 'b';
		case '3':
			return 'c';
		case '4':
			return 'd';
		case '5':
			return 'e';
		case '6':
			return 'f';
		case '7':
			return 'g';
		case '8':
			return 'h';
		case '9':
			return 'i';
		default:
			return -1;
	}
	}
}

/** 
 * FEN串中棋子类型标识  */
int Engine::fen_to_piece(int nArg) 
{
  switch (nArg) {
  case 'K':
    return 0;
  case 'G':
  case 'A':
    return 1;
  case 'B':
  case 'E':
    return 2;
  case 'N':
  case 'H':
    return 3;
  case 'R':
    return 4;
  case 'C':
    return 5;
  case 'P':
    return 6;
    case 'k':
        return 7;
    case 'g':
    case 'a':
        return 8;
    case 'b':
    case 'e':
        return 9;
    case 'h':
    case 'n':
        return 10;
    case 'r':
        return 11;
    case 'c':
        return 12;
    case 'p':
        return 13;
  default:
    return 14;
  }
}

int Engine::knight_cmp(int y,int x)
{
	if((x-y)<0)
		return x+1;
	else
		return x-1;
}

int Engine::get_knight_leg(int f_src, int f_dst)
{
	int a=knight_cmp(RANK_Y(f_src),RANK_Y(f_dst));
	int b=knight_cmp(RANK_X(f_src),RANK_X(f_dst));
	return a*16+b;

}

/** 目前只做基本检测，将军之类的走棋暂不考虑
 * 着法的合法化有两种，
 * 一是先根据棋子生成合法的着法，然后检测目标着法是否
 * 匹配，如果不匹配则为非法的着法。
 * 二是只判断目标着法是否合法的着法即可，以下函数目前使用此方法
 **/
bool Engine::logic_move(int mv)
{
	int src = get_move_src(mv);
	int dst = get_move_dst(mv);
	int eated = get_move_eat(mv);

	if(!in_board(dst))
		return false;
	/** 判断是否过时的方法，dst & 0x80,在下方是非0, 上方是0 */
	/** 获取要移动棋子的类型*/
	int chess_t = get_chessman_type(chessboard[src]);
	DLOG("逻辑判断棋子chesboard[src] = %d  %d\n",chessboard[src],chess_t);
	switch(chess_t){
		/** 将/帅的着法，同一纵线或横线，移动只一个单位，在九宫内*/
		case RED_KING:
		case BLACK_KING:
			DLOG("帅走\n");
			if(in_fort(dst)){
				if((1==abs(RANK_X(src)-RANK_X(dst))) || 
					(1==abs(RANK_Y(src)-RANK_Y(dst)))){
					if((RANK_X(src) == RANK_X(dst)) ||
							(RANK_Y(src)==RANK_Y(dst))){
						return true;
					}
				}
			}
			break;
		/** 士的着法也是绝对值检测，但相差1即可，还需要检测
		 * 是否在九宫格内*/
		case RED_ADVISOR:
		case BLACK_ADVISOR:
			if(in_fort(dst) &&
					(1==abs(RANK_X(src)-RANK_X(dst))) && 
					(1==abs(RANK_Y(src)-RANK_Y(dst)))){
				DLOG("可走，返回真\n");
				return true;
			}
			break;
		/** 相的着法初级只需要检测目标及起点x,y绝对值是否相
		 * 差2即可,还需要注意不能过河,及阻象脚
		 */
		case RED_BISHOP:
			if((dst&0x80) != 0){
				if((2==abs(RANK_X(src)-RANK_X(dst))) && 
					(2==abs(RANK_Y(src)-RANK_Y(dst)))){
					int leg=get_bishop_leg(src,dst);
					if(chessboard[leg]==0)
						return true;
				}
			}
			break;
		case BLACK_BISHOP:
			DLOG("相走\n");
			if((dst&0x80) == 0){
				DLOG("进来了吗相走\n");
				if((2==abs(RANK_X(src)-RANK_X(dst))) && 
					(2==abs(RANK_Y(src)-RANK_Y(dst)))){
					int leg=get_bishop_leg(src,dst);
					if(chessboard[leg]==0)
						return true;
				}
			}
			break;
		/** 马的着法，目标及起点绝对值相差1-2或2-1
		 *  判断绊马脚搞定
		 */
		case RED_KNIGHT:
		case BLACK_KNIGHT:
			DLOG("马走\n");
			if(((1==abs(RANK_X(src)-RANK_X(dst))) && 
					(2==abs(RANK_Y(src)-RANK_Y(dst))))||
					((2==abs(RANK_X(src)-RANK_X(dst))) && 
					(1==abs(RANK_Y(src)-RANK_Y(dst))))){

				int leg=get_knight_leg(src,dst);
				if(chessboard[leg]==0)
					return true;
			}
			break;
		/** 炮和车合法着法的特点是同一横线或同一纵线，
		 * 初级只需要检测是否同一横线或纵线即可
		 * 但往下还需要检测是否跨子,跨子检测搞定*/
		case RED_ROOK:
		case BLACK_ROOK:
			DLOG("车走\n");
			if((RANK_X(src) == RANK_X(dst))){
				int min_t = src<dst?src:dst;
				int num_t = abs(RANK_Y(src)-RANK_Y(dst))-1;
				min_t +=16;
				for(int i =0;i<num_t;i++){
					if(chessboard[min_t]==0)
						min_t +=16;
					else
						return false;
				}
				return true;
			}
			else if((RANK_Y(src)==RANK_Y(dst))){
				int min_t = src<dst?src:dst;
				int num_t = abs(src-dst) -1 ;
				min_t++;
				for(int i =0;i<num_t; i++){
					if(chessboard[min_t]==0)
						min_t++;
					else
						return false;
				}
				return true;

			}
			break;
		case RED_CANNON:
		case BLACK_CANNON:
			DLOG("炮走\n");
			if((RANK_X(src) == RANK_X(dst))){
				int min_t = src<dst?src:dst;
				int num_t = abs(RANK_Y(src)-RANK_Y(dst))-1;
				min_t +=16;
				int paotai=0;
				if(!eated){
					for(int i =0;i<num_t;i++){
						if(chessboard[min_t]==0)
							min_t +=16;
						else
							return false;
					}
					return true;
				}
				else{
					for(int i =0;i<num_t;i++){
						if(chessboard[min_t]==0)
							;
						else
							paotai++;
						min_t +=16;
					}
					if(1==paotai)
						return true;
				}

			}
			else if((RANK_Y(src)==RANK_Y(dst))){
				int min_t = src<dst?src:dst;
				int num_t = abs(src-dst) -1 ;
				min_t++;
				int paotai=0;
				if(!eated){
					for(int i =0;i<num_t; i++){
						if(chessboard[min_t]==0)
							min_t++;
						else
							return false;
					}
					return true;
				}
				else {
					for(int i =0;i<num_t; i++){
						if(chessboard[min_t]==0)
							;
						else
							paotai++;
						min_t++;
					}
					DLOG(" 吃子走法,中间有%d个\n",paotai);
					if(1==paotai)
						return true;
				}

			}

			break;
		/** 兵的走法和帅类似，过河判断搞定*/
		case RED_PAWN:
			if((dst&0x80) != 0){
				if((src-dst) == 16)
					return true;
			}
			else{
				if(1== abs(dst-src) || (src-dst)==16)
						return true;
			}
			break;
		case BLACK_PAWN:
			if((dst&0x80) == 0){
				if((dst-src) == 16) 
					return true;
			}
			else{
				if(1== abs(dst-src) || (dst-src)==16)
						return true;
			}
			break;
		default:
			return false;
			break;
	};

	return false;

}
/**
 * @brief 
 * 执行了mv着法，将生成的棋盘数组转成FEN串添加到FEN快照里
 * 界面棋盘将根据棋盘数组更新
 * 另外生成着法的中文字符/ICCS 表示
 * @param mv 着法
 * @return 0表示执行，-1表示执行失败
 */
int Engine::do_move(int mv)
{
	int src = get_move_src(mv);
	int dst = get_move_dst(mv);
	//int eated = get_move_eat(mv);
	int eated = chessboard[dst];

	/** 得到中文表示法怎么这么曲折呢*/
	uint32_t iccs =move_to_iccs(mv);
	uint32_t hanzi = iccs_to_hanzi(iccs);
	Glib::ustring mv_line = hanzi_to_move_chinese(hanzi);

	DLOG(" src = %x dst = %x mv = %d\n chessboard[src]= %d , chessboard[dst] = %d,eated = %d\n",src,dst,mv,chessboard[src],chessboard[dst],eated);

	//if(eated != chessboard[dst])
	//	return -1;
	/**
	 * 如果dst的位置上有棋子，即是出现被吃子现象，
	 * 则要将这个棋子的位置值置0,表示被吃掉，不再出现在棋盘上
	 */
	if(eated !=0){
		DLOG(" %d has been eat\n",chessboard[dst]);
		chessmans[eated]=0;
	}
	chessboard[dst] = chessboard[src];
	chessboard[src] = 0;
	/** 被移动的棋子的位置值变成目标地点 */
	chessmans[chessboard[dst]] = dst ;

	/** 交换走子方 */
	change_side();

	/** 着法添加进快照 */
	move_snapshots.push_back(mv);
	char str_fen[128];
	to_fens(str_fen);
	fen_snapshots.push_back(std::string(str_fen));

	add_move_chinese(mv_line);

	/*
	printf("\n ==after do move ==\n");
	int i,j;
	for(i=0;i<16;i++)
	{
		for(j=0;j<16;j++)
			printf(" %2d ",chessboard[i*16+j]);
		printf("\n");
	}
	*/
	
	return 0;
}

void Engine::undo_move(int mv)
{
	int src = get_move_src(mv);
	int dst = get_move_dst(mv);
	int eated = get_move_eat(mv);

	chessboard[src] = chessboard[dst];
	chessboard[dst] = eated;

	chessmans[chessboard[src]]=src;
	chessmans[eated]=dst ;

	/** 交换走子方 */
	change_side();


	move_snapshots.pop_back();
	fen_snapshots.pop_back();
	move_chinese.pop_back();
}
uint32_t Engine::move_to_iccs(int mv)
{
	union{
		char c[4];
		uint32_t dw;
	}Ret;
	int src=get_move_src(mv);
	int dst=get_move_dst(mv);

	Ret.c[0] = get_iccs_x(src);
	Ret.c[1] = get_iccs_y(src) ;
	Ret.c[2] = get_iccs_x(dst);
	Ret.c[3] = get_iccs_y(dst);

	DLOG("%s : %c%c%c%c\n",__FUNCTION__,Ret.c[0],Ret.c[1],Ret.c[2],Ret.c[3]);
	return Ret.dw;

}
int Engine::iccs_to_move(uint32_t iccs)
{
	int src,dst;
	char *p;
	p = (char*)&iccs;
	src = get_coord( p[0] - 'a' + FILE_LEFT, '9'-p[1] +RANK_TOP);
	dst = get_coord( p[2] - 'a' + FILE_LEFT, '9'-p[3] +RANK_TOP);

	return (in_board(src) && in_board(dst) ? get_move(src,dst):0);
}



uint32_t Engine::iccs_to_hanzi(uint32_t f_iccs)
{
	union Hanzi c_hanzi;
	union Hanzi c_iccs;
	c_iccs.digit = f_iccs;
	int a1_x,a2_x;

	int mv = iccs_to_move(f_iccs);
	int src = get_move_src(mv);
	int chess_t  = get_chessman_type(chessboard[src]);

	c_hanzi.word[0] = piece_to_fen(chess_t);

	switch(chess_t){
		case RED_KING:
		case BLACK_KING:

			c_hanzi.word[1] = alpha_to_digit(c_iccs.word[0]);

			if(c_iccs.word[1]==c_iccs.word[3]){
				c_hanzi.word[2]='.';
				/** alpha_to_digit 解决了红黑方方位表示的问题*/
				c_hanzi.word[3]= alpha_to_digit(c_iccs.word[2]);
			}
			else if(c_iccs.word[1] > c_iccs.word[3]){
				if(black_player)
					c_hanzi.word[2]='+';
				else
					c_hanzi.word[2]='-';
				c_hanzi.word[3]='1';
			}
			else{
				if(black_player)
					c_hanzi.word[2]='-';
				else
					c_hanzi.word[2]='+';
				c_hanzi.word[3]='1';
			}
			break;
		/** 士相只有和进退，所以同一纵线上能退的肯定是前面那个*/
		case RED_ADVISOR:
		case BLACK_ADVISOR:
		case RED_BISHOP:
		case BLACK_BISHOP:
			c_hanzi.word[1] = alpha_to_digit(c_iccs.word[0]);
			if(c_iccs.word[1] > c_iccs.word[3]){
				if(black_player)
					c_hanzi.word[2]='+';
				else
					c_hanzi.word[2]='-';
			}
			else{
				if(black_player)
					c_hanzi.word[2]='-';
				else
					c_hanzi.word[2]='+';
			}
			c_hanzi.word[3]= alpha_to_digit(c_iccs.word[2]);
			break;
		/** 马和士相情况类似，但要区分前后*/
		case RED_KNIGHT:
		case BLACK_KNIGHT:
			a1_x = get_iccs_x(chessmans[side_tag(black_player)+KNIGHT_FROM]);
			a2_x = get_iccs_x(chessmans[side_tag(black_player)+KNIGHT_TO]);
			if(a1_x==a2_x){
				int a1_y = get_iccs_y(chessmans[side_tag(black_player)+KNIGHT_FROM]);
				int a2_y = get_iccs_y(chessmans[side_tag(black_player)+KNIGHT_TO]);
				if(a1_y > a2_y){
					if(a1_y== c_iccs.word[1]){
						if(black_player)
							c_hanzi.word[1]='b';
						else
							c_hanzi.word[1]='a';
					}
					else{
						if(black_player)
							c_hanzi.word[1]='a';
						else
							c_hanzi.word[1]='b';

					}

				}
				else{
					if(a1_y== c_iccs.word[1]){
						if(black_player)
							c_hanzi.word[1]='a';
						else
							c_hanzi.word[1]='b';
					}
					else{
						if(black_player)
							c_hanzi.word[1]='b';
						else
							c_hanzi.word[1]='a';
					}
				}

			}
			else
				c_hanzi.word[1] = alpha_to_digit(c_iccs.word[0]);
			if(c_iccs.word[1] > c_iccs.word[3]){
				if(black_player)
					c_hanzi.word[2]='+';
				else
					c_hanzi.word[2]='-';
			}
			else{
				if(black_player)
					c_hanzi.word[2]='-';
				else
					c_hanzi.word[2]='+';
			}
			c_hanzi.word[3]= alpha_to_digit(c_iccs.word[2]);
			break;
		/**车和炮情况雷同*/
		case RED_ROOK:
		case BLACK_ROOK:
			a1_x = get_iccs_x(chessmans[side_tag(black_player)+ROOK_FROM]);
			a2_x = get_iccs_x(chessmans[side_tag(black_player)+ROOK_TO]);
			if(a1_x==a2_x){
				int a1_y = get_iccs_y(chessmans[side_tag(black_player)+ROOK_FROM]);
				int a2_y = get_iccs_y(chessmans[side_tag(black_player)+ROOK_TO]);
				if(a1_y > a2_y){
					if(a1_y== c_iccs.word[1]){
						if(black_player)
							c_hanzi.word[1]='b';
						else
							c_hanzi.word[1]='a';
					}
					else{
						if(black_player)
							c_hanzi.word[1]='a';
						else
							c_hanzi.word[1]='b';

					}

				}
				else{
					if(a1_y== c_iccs.word[1]){
						if(black_player)
							c_hanzi.word[1]='a';
						else
							c_hanzi.word[1]='b';
					}
					else{
						if(black_player)
							c_hanzi.word[1]='b';
						else
							c_hanzi.word[1]='a';
					}
				}

			}
			else
				c_hanzi.word[1] = alpha_to_digit(c_iccs.word[0]);

			if(c_iccs.word[1]==c_iccs.word[3]){
				c_hanzi.word[2]='.';
				/** alpha_to_digit 解决了红黑方方位表示的问题*/
				c_hanzi.word[3]= alpha_to_digit(c_iccs.word[2]);
			}
			else if(c_iccs.word[1] > c_iccs.word[3]){
				if(black_player)
					c_hanzi.word[2]='+';
				else
					c_hanzi.word[2]='-';
				c_hanzi.word[3]=c_iccs.word[1]-c_iccs.word[3]+48;
			}
			else{
				if(black_player)
					c_hanzi.word[2]='-';
				else
					c_hanzi.word[2]='+';
				c_hanzi.word[3]=c_iccs.word[3]-c_iccs.word[1]+48;
			}
			break;
		case RED_CANNON:
		case BLACK_CANNON:
			a1_x = get_iccs_x(chessmans[side_tag(black_player)+CANNON_FROM]);
			a2_x = get_iccs_x(chessmans[side_tag(black_player)+CANNON_TO]);
			if(a1_x==a2_x){
				int a1_y = get_iccs_y(chessmans[side_tag(black_player)+CANNON_FROM]);
				int a2_y = get_iccs_y(chessmans[side_tag(black_player)+CANNON_TO]);
				if(a1_y > a2_y){
					if(a1_y== c_iccs.word[1]){
						if(black_player)
							c_hanzi.word[1]='b';
						else
							c_hanzi.word[1]='a';
					}
					else{
						if(black_player)
							c_hanzi.word[1]='a';
						else
							c_hanzi.word[1]='b';

					}

				}
				else{
					if(a1_y== c_iccs.word[1]){
						if(black_player)
							c_hanzi.word[1]='a';
						else
							c_hanzi.word[1]='b';
					}
					else{
						if(black_player)
							c_hanzi.word[1]='b';
						else
							c_hanzi.word[1]='a';
					}
				}

			}
			else
				c_hanzi.word[1] = alpha_to_digit(c_iccs.word[0]);

			if(c_iccs.word[1]==c_iccs.word[3]){
				c_hanzi.word[2]='.';
				/** alpha_to_digit 解决了红黑方方位表示的问题*/
				c_hanzi.word[3]= alpha_to_digit(c_iccs.word[2]);
			}
			else if(c_iccs.word[1] > c_iccs.word[3]){
				if(black_player)
					c_hanzi.word[2]='+';
				else
					c_hanzi.word[2]='-';
				c_hanzi.word[3]=c_iccs.word[1]-c_iccs.word[3]+48;
			}
			else{
				if(black_player)
					c_hanzi.word[2]='-';
				else
					c_hanzi.word[2]='+';
				c_hanzi.word[3]=c_iccs.word[3]-c_iccs.word[1]+48;
			}
			break;
		case RED_PAWN:
		case BLACK_PAWN:
			/** fixed it */
			c_hanzi.word[1] = alpha_to_digit(c_iccs.word[0]);
			if(c_iccs.word[1]==c_iccs.word[3]){
				c_hanzi.word[2]='.';
				/** alpha_to_digit 解决了红黑方方位表示的问题*/
				c_hanzi.word[3]= alpha_to_digit(c_iccs.word[2]);
			}
			else if(c_iccs.word[1] > c_iccs.word[3]){
				if(black_player)
					c_hanzi.word[2]='+';
				else
					c_hanzi.word[2]='-';
				c_hanzi.word[3]='1';
			}
			else{
				if(black_player)
					c_hanzi.word[2]='-';
				else
					c_hanzi.word[2]='+';
				c_hanzi.word[3]='1';
			}
			break;
		default:
			return 0;
			break;
	};

	DLOG("%s : %c%c%c%c\n",__FUNCTION__,c_iccs.word[0],c_iccs.word[1],c_iccs.word[2],c_iccs.word[3]);
	DLOG("%s : %c%c%c%c\n",__FUNCTION__,c_hanzi.word[0],c_hanzi.word[1],c_hanzi.word[2],c_hanzi.word[3]);
	return c_hanzi.digit;

}

Glib::ustring  Engine::hanzi_to_move_chinese(uint32_t f_hanzi)
{
	//Glib::ustring tmp_t;
	union Hanzi c_hanzi;
	c_hanzi.digit = f_hanzi;
	if(c_hanzi.word[1]>='a'){
		return digit_to_word(c_hanzi.word[1])
			+code_to_word(c_hanzi.word[0])
			+action_to_word(c_hanzi.word[2])
			+digit_to_word(c_hanzi.word[3]) ;
		
	}
	else{
		//tmp_t=code_to_word(c_hanzi.word[0])
		return code_to_word(c_hanzi.word[0])
			+digit_to_word(c_hanzi.word[1])
			+action_to_word(c_hanzi.word[2])
			+digit_to_word(c_hanzi.word[3]) ;
	}
	


}

Glib::ustring Engine::digit_to_word(char digit)
{
	DLOG("digit = %c",digit);
	if(black_player){
		switch(digit){
			case '1':
				return Glib::ustring("1");
				break;
			case '2':
				return Glib::ustring("2");
				break;
			case '3':
				return Glib::ustring("3");
				break;
			case '4':
				return Glib::ustring("4");
				break;
			case '5':
				return Glib::ustring("5");
				break;
			case '6':
				return Glib::ustring("6");
				break;
			case '7':
				return Glib::ustring("7");
				break;
			case '8':
				return Glib::ustring("8");
				break;
			case '9':
				return Glib::ustring("9");
				break;
			case 'a':
				return Glib::ustring("前");
				break;
			case 'b':
				return Glib::ustring("后");
				break;
			case 'c':
				return Glib::ustring("三");
				break;
			case 'd':
				return Glib::ustring("四");
				break;
			case 'e':
				return Glib::ustring("五");
				break;
			default:
				break;
			};
	}
	else{
	switch(digit){
		case '1':
			return Glib::ustring("一");
			break;
		case '2':
			return Glib::ustring("二");
			break;
		case '3':
			return Glib::ustring("三");
			break;
		case '4':
			return Glib::ustring("四");
			break;
		case '5':
			return Glib::ustring("五");
			break;
		case '6':
			return Glib::ustring("六");
			break;
		case '7':
			return Glib::ustring("七");
			break;
		case '8':
			return Glib::ustring("八");
			break;
		case '9':
			return Glib::ustring("九");
			break;
		case 'a':
			return Glib::ustring("前");
			break;
		case 'b':
			return Glib::ustring("后");
			break;
		case 'c':
			return Glib::ustring("三");
			break;
		case 'd':
			return Glib::ustring("四");
			break;
		case 'e':
			return Glib::ustring("五");
			break;
		default:
			break;
		};
	}
	return Glib::ustring("NULL");

}
Glib::ustring Engine::action_to_word(char action)
{
	DLOG(" action = %c\n",action);
	switch(action){
		case '.':
			return Glib::ustring("平");
			break;
		case '+':
			return Glib::ustring("进");
			break;
		case '-':
			return Glib::ustring("退");
			break;
		default:
			break;
	};
	return Glib::ustring("NULL");
}
Glib::ustring Engine::code_to_word(char code)
{
	DLOG("code = %c\n",code);
	switch(code){
		case 'K':
			return Glib::ustring("帅");
			break;
		case 'A':
			return Glib::ustring("仕");
			break;
		case 'B':
			return Glib::ustring("相");
			break;
		case 'N':
		case 'n':
			return Glib::ustring("马");
			break;
		case 'C':
		case 'c':
			return Glib::ustring("炮");
		case 'R':
		case 'r':
			return Glib::ustring("车");
			break;
		case 'P':
			return Glib::ustring("兵");
			break;
		case 'k':
			return Glib::ustring("将");
			break;
		case 'a':
			return Glib::ustring("士");
			break;
		case 'b':
			return Glib::ustring("象");
			break;
		case 'p':
			return Glib::ustring("卒");
			break;
		default:
			break;
	};
	return Glib::ustring("NULL");
}
uint32_t Engine::hanzi_to_iccs(uint32_t f_hanzi)
{
	/** hazi中文纵线表示方式中，hanzi的四个字符依次是: 炮二平五(C2.5) */
	
	union Hanzi c_hanzi;
	union Hanzi c_iccs;

	c_hanzi.digit = f_hanzi;

	DLOG("hazi[0...3] = %c%c%c%c\n",c_hanzi.word[0],c_hanzi.word[1],c_hanzi.word[2],c_hanzi.word[3]);
	/** cman_type 是棋子类型*/
	int cman_type = fen_to_piece(c_hanzi.word[0]);
	int src_x,src_y,dst_x,dst_y;
	int num;

	DLOG("who is going black=%d\n",black_player);
	if(black_player)
		num = 16;
	else
		num= 0;

	if(0 == cman_type ){
		/** 处理帅将的源地址*/
		src_x = get_iccs_x(chessmans[16+num]);  
		src_y = get_iccs_y(chessmans[16+num]);  

		DLOG("chessmans[%d]=%x\n",16+num,chessmans[16+num]);
		DLOG("src_x = %c ,  src_y = %c\n",src_x,src_y);
		if(c_hanzi.word[2] == '+'){
			dst_x = src_x;
			if(!black_player)
				dst_y = src_y+1;
			else
				dst_y = src_y-1;
		}
		else if(c_hanzi.word[2] == '-'){
			dst_x = src_x;
			if(!black_player)
				dst_y = src_y-1;
			else
				dst_y = src_y+1;

		}
		else if(c_hanzi.word[2] == '.'){
			dst_x = digit_to_alpha(c_hanzi.word[3]);
			dst_y = src_y;

		}
		c_iccs.word[0] = src_x;
		c_iccs.word[1] = src_y;
		c_iccs.word[2] = dst_x;
		c_iccs.word[3] = dst_y;

		DLOG("hazi[0...3] = %c%c%c%c\n",c_iccs.word[0],c_iccs.word[1],c_iccs.word[2],c_iccs.word[3]);
		return c_iccs.digit;
	}
	else if(1 == cman_type){
		/** 处理士的源及目标地址*/
		/** 士与相没有前后之分,能进则是后面那个，能退则是前面那个*/
		src_x = digit_to_alpha(c_hanzi.word[1]);
		c_iccs.word[0] = src_x;

		dst_x = digit_to_alpha(c_hanzi.word[3]);
		int a1_x = get_iccs_x(chessmans[17+num]);
		int a2_x = get_iccs_x(chessmans[18+num]);
		if(a1_x == a2_x){
			/** 士在同一纵线上*/
			int a1_y = get_iccs_y(chessmans[17+num]);
			int a2_y = get_iccs_y(chessmans[18+num]);
			
			if(c_hanzi.word[2] == '+'){
				if(!black_player){
					src_y = a1_y<a2_y?a1_y:a2_y;
					dst_y = src_y+1;
				}
				else{
					src_y = a1_y>a2_y?a1_y:a2_y;
					dst_y = src_y-1;
				}

			}
			else if(c_hanzi.word[2] == '-'){
				if(!black_player){
					src_y = a1_y>a2_y?a1_y:a2_y;
					dst_y = src_y-1;
				}
				else{
					src_y = a1_y>a2_y?a1_y:a2_y;
					dst_y = src_y-1;
				}
			}

			c_iccs.word[1] =src_y;
			c_iccs.word[2] =dst_x;
			c_iccs.word[3] =dst_y;

		}
		else{
			if(a1_x == src_x){
				src_y = get_iccs_y(chessmans[17+num]);
			}
			else if(a2_x == src_x){
				src_y = get_iccs_y(chessmans[18+num]);
			}
			else{
				DLOG("士的匹配位置出现问题\n");
				goto error_out;
			}

			if(c_hanzi.word[2] == '+'){
				if(!black_player)
					dst_y = src_y+1;
				else
					dst_y = src_y-1;
			}
			else if(c_hanzi.word[2] == '-'){
				if(!black_player)
					dst_y = src_y-1;
				else
					dst_y = src_y+1;
			}
			c_iccs.word[1]=src_y;
			c_iccs.word[2] =dst_x;
			c_iccs.word[3] =dst_y;

		}
			
		DLOG("hazi[0...3] = %c%c%c%c\n",c_iccs.word[0],c_iccs.word[1],c_iccs.word[2],c_iccs.word[3]);
		return c_iccs.digit;
	}
	else if(2 == cman_type){
		/** 相/象的处理*/
		src_x = digit_to_alpha(c_hanzi.word[1]);
		c_iccs.word[0] = src_x;

		dst_x = digit_to_alpha(c_hanzi.word[3]);
		int a1_x = get_iccs_x(chessmans[19+num]);
		int a2_x = get_iccs_x(chessmans[20+num]);
		if(a1_x == a2_x){
			/** 象在同一纵线上*/
			int a1_y = get_iccs_y(chessmans[19+num]);
			int a2_y = get_iccs_y(chessmans[20+num]);
			
			if(c_hanzi.word[2] == '+'){
				if(!black_player){
					src_y = a1_y<a2_y?a1_y:a2_y;
					dst_y = src_y+2;
				}
				else{
					src_y = a1_y>a2_y?a1_y:a2_y;
					dst_y = src_y-2;

				}

			}
			else if(c_hanzi.word[2] == '-'){
				if(!black_player){
					src_y = a1_y>a2_y?a1_y:a2_y;
					dst_y = src_y-2;
				}
				else{
					src_y = a1_y<a2_y?a1_y:a2_y;
					dst_y = src_y+2;
				}
			}

		//	c_iccs.word[1] =src_y;
		//	c_iccs.word[2] =dst_x;
		//	c_iccs.word[3] =dst_y;

		}
		else{
			if(a1_x == src_x){
				src_y = get_iccs_y(chessmans[19+num]);
			}
			else if(a2_x == src_x){
				src_y = get_iccs_y(chessmans[20+num]);
			}
			else{
				DLOG("象的匹配位置出现问题\n");
				goto error_out;
			}

			if(c_hanzi.word[2] == '+'){
				if(!black_player)
					dst_y = src_y+2;
				else
					dst_y = src_y-2;
			}
			else if(c_hanzi.word[2] == '-'){
				if(!black_player)
					dst_y = src_y-2;
				else
					dst_y = src_y+2;
			}
			//c_iccs.word[1]=src_y;
			//c_iccs.word[2] =dst_x;
			//c_iccs.word[3] =dst_y;
		}

		c_iccs.word[1]=src_y;
		c_iccs.word[2] =dst_x;
		c_iccs.word[3] =dst_y;
		DLOG("hazi[0...3] = %c%c%c%c\n",c_iccs.word[0],c_iccs.word[1],c_iccs.word[2],c_iccs.word[3]);
		return c_iccs.digit;
	}
	else if(3 == cman_type){
		/** 马的处理*/
		int type_num=21;
		src_x = digit_to_alpha(c_hanzi.word[1]);
		dst_x = digit_to_alpha(c_hanzi.word[3]);

		int a1_x = get_iccs_x(chessmans[type_num+num]);
		int a2_x = get_iccs_x(chessmans[type_num+1+num]);
		if(src_x<0){
			/** 同一纵线上有两匹马 */
			int a1_y = get_iccs_y(chessmans[type_num+num]);
			int a2_y = get_iccs_y(chessmans[type_num+1+num]);
			if(c_hanzi.word[1] == 'a'){

				if(!black_player)
					src_y = a1_y>a2_y?a1_y:a2_y;
				else
					src_y = a1_y<a2_y?a1_y:a2_y;
			}
			else{
				if(!black_player)
					src_y = a1_y<a2_y?a1_y:a2_y;
				else
					src_y = a1_y>a2_y?a1_y:a2_y;

				}


			src_x=a1_x;
		}
		else{

				DLOG("a1_x = %c , a2_x = %c , src_x = %c\n",a1_x,a2_x,src_x);
				DLOG("chessmans[%d]=%x\n",type_num+num,chessmans[type_num+num]);
				DLOG("chessmans[%d]=%x\n",type_num+1+num,chessmans[type_num+1+num]);
			if(a1_x == src_x){
				src_y = get_iccs_y(chessmans[type_num+num]);
			}
			else if(a2_x == src_x){
				src_y = get_iccs_y(chessmans[type_num+1+num]);
			}
			else{
				DLOG("马的匹配位置出现问题\n");
				goto error_out;
			}
			DLOG("src_y = %c\n",src_y);

		}
			if(c_hanzi.word[2] == '+'){
				if(abs(dst_x-src_x)== 1){
					if(!black_player)
						dst_y = src_y +2;
					else
						dst_y = src_y -2;
				}
				else{
					if(!black_player)
						dst_y = src_y +1;
					else
						dst_y=src_y-1;
					}
			}
			else if(c_hanzi.word[2] == '-'){
				if(abs(dst_x-src_x)== 1){
					if(!black_player)
						dst_y = src_y -2;
					else
						dst_y = src_y +2;
				}else{
					if(!black_player)
						dst_y = src_y -1;
					else
						dst_y = src_y +1;
				}
			}


			c_iccs.word[0]=src_x;
			c_iccs.word[1]=src_y;
			c_iccs.word[2] =dst_x;
			c_iccs.word[3] =dst_y;

		DLOG("hazi[0...3] = %c%c%c%c\n",c_iccs.word[0],c_iccs.word[1],c_iccs.word[2],c_iccs.word[3]);
		return c_iccs.digit;
	}
	else if(4==cman_type||5==cman_type){
		/** 车炮的目标坐标生成*/
		int type_num;
			if(cman_type==4)
				type_num = 23;
			else if(5==cman_type)
				type_num=25;

		src_x = digit_to_alpha(c_hanzi.word[1]);

		int a1_x = get_iccs_x(chessmans[type_num+num]);
		int a2_x = get_iccs_x(chessmans[type_num+1+num]);
		if(src_x<0){
			/** 同一纵线上有两个车或炮 */
			int a1_y = get_iccs_y(chessmans[type_num+num]);
			int a2_y = get_iccs_y(chessmans[type_num+1+num]);
			if(c_hanzi.word[1] == 'a'){
				if(!black_player)
					src_y = a1_y>a2_y?a1_y:a2_y;
				else
					src_y = a1_y<a2_y?a1_y:a2_y;
			}
			else{
				if(!black_player)
					src_y = a1_y<a2_y?a1_y:a2_y;
				else
					src_y = a1_y>a2_y?a1_y:a2_y;
			}
			src_x= a1_x;
		}
		else {
				DLOG("a1_x = %c , a2_x = %c , src_x = %c\n",a1_x,a2_x,src_x);
				DLOG("chessmans[%d]=%x\n",type_num+num,chessmans[type_num+num]);
				DLOG("chessmans[%d]=%x\n",type_num+1+num,chessmans[type_num+1+num]);

			if(a1_x == src_x){
				src_y = get_iccs_y(chessmans[type_num+num]);
			}
			else if(a2_x == src_x){
				src_y = get_iccs_y(chessmans[type_num+1+num]);
			}
			else{
				DLOG("车炮的匹配位置出现问题\n");
				goto error_out;
			}
		}

		char c= c_hanzi.word[3];
		DLOG("src_y=%c, c=%c\n",src_y,c);
		if(c_hanzi.word[2] == '+'){
			dst_x = src_x;
			if(!black_player)
				dst_y = src_y + c;
				//dst_y = src_y + atoi(&c);
			else
				dst_y = src_y - c;
				//dst_y = src_y - atoi(&c);
		}
		else if(c_hanzi.word[2] == '-'){
			dst_x = src_x;
			if(!black_player)
				dst_y = src_y - c;
				//dst_y = src_y - atoi(&c);
			else
				dst_y = src_y + c;
				//dst_y = src_y + atoi(&c);
		}
		else if(c_hanzi.word[2] == '.'){
			dst_x = digit_to_alpha(c_hanzi.word[3]);
			dst_y = src_y;

		}
		c_iccs.word[0]=src_x;
		c_iccs.word[1]=src_y;
		c_iccs.word[2] =dst_x;
		c_iccs.word[3] =dst_y;

		DLOG("hazi[0...3] = %c%c%c%c\n",c_iccs.word[0],c_iccs.word[1],c_iccs.word[2],c_iccs.word[3]);
		return c_iccs.digit;
	}
	else if(6==cman_type){

		src_x = digit_to_alpha(c_hanzi.word[1]);
		int a_x[5]={0};
		int a_y[5]={0};
		for(int i=0;i<5;i++){
			a_x[i] = get_iccs_x(chessmans[i+27+num]);
			a_y[i] = get_iccs_y(chessmans[i+27+num]);
		}

		if(src_x<0){
			int x_rand[9]={0};
			int i;
			/** 五个兵，分好纵线*/
			for(i=0;i<5;i++){
				int n = a_x[i] -'a';
				if(n>=0)
					x_rand[n]++;
			}
			int p1_line[5]={0};
			int n=0;
			int start=0;
			for(i=9;i>=0;i--){
				if(x_rand[i]>1){
					/**此纵线上有两个以上兵*/
					for(int j=0;j<5;j++){
						if(i == (a_x[j]-'a')){
							p1_line[n]=j;
							n++;
						}
					}
					/** 为纵线上的棋子排序*/
					for(i=start;i<n-1;i++)
						for(int j=n-1;j>i;j--){
							if(a_y[p1_line[j]]>a_y[ p1_line[j-1] ]){
								int tmp=p1_line[j];
								p1_line[j]=p1_line[j-1];
								p1_line[j-1]=tmp;
						}

					}
					start=n;
				}


			}

			/** 处理纵线上多兵的问题，还要考虑黑方位置问题，未解决*/
			if(c_hanzi.word[1] == 'a'){
				src_x=a_x[p1_line[0]];
				src_y=a_y[p1_line[0]];

			}
			else if(c_hanzi.word[1] == 'b'){

				src_x=a_x[p1_line[1]];
				src_y=a_y[p1_line[1]];

			}
			else if(c_hanzi.word[1] == 'c'){
				src_x=a_x[p1_line[2]];
				src_y=a_y[p1_line[2]];
			}
			else if(c_hanzi.word[1] == 'd'){
				src_x=a_x[p1_line[3]];
				src_y=a_y[p1_line[3]];
			}
			else if(c_hanzi.word[1] == 'e'){
				src_x=a_x[p1_line[4]];
				src_y=a_y[p1_line[4]];
			}


		}
		else{
			for(int i=0;i<5;i++){
				if(a_x[i] == src_x)
					src_y = a_y[i];
			}

		}

		if(c_hanzi.word[2] == '+'){
			char c= c_hanzi.word[3];
			dst_x = src_x;
			if(!black_player)
				dst_y = src_y + 1;
			else
				dst_y = src_y - 1;
		}
		else if(c_hanzi.word[2] == '-'){
			char c= c_hanzi.word[3];
			dst_x = src_x;
			if(!black_player)
				dst_y = src_y - 1;
			else
				dst_y = src_y + 1;
		}
		else if(c_hanzi.word[2] == '.'){
			dst_x = digit_to_alpha(c_hanzi.word[3]);
			dst_y = src_y;

		}


		c_iccs.word[0]=src_x;
		c_iccs.word[1]=src_y;
		c_iccs.word[2] =dst_x;
		c_iccs.word[3] =dst_y;

		DLOG("hazi[0...3] = %c%c%c%c\n",c_iccs.word[0],c_iccs.word[1],c_iccs.word[2],c_iccs.word[3]);
		return c_iccs.digit;
	}
error_out:
	printf(" %s error\n",__FUNCTION__);
	return 0;
}
