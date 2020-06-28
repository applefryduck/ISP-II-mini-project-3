#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
using namespace std;
int player;
const int SIZE = 8;
struct Point {
    int x, y;
	Point() : Point(0, 0) {}
	Point(float x, float y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};
Point corner[4]{Point(0,0),Point(0,7),Point(7,0),Point(7,7)};
int eff[SIZE][SIZE]{
{100 ,-60 ,12  ,10  ,10  ,12  ,-60 ,100 },
{-60 ,-100,5   ,5   ,5   ,5   ,-100,-60 },
{12  ,5   ,5   ,5   ,5   ,5   ,5   ,10  },
{10  ,5   ,5   ,5   ,5   ,5   ,5   ,10  },
{10  ,5   ,5   ,5   ,5   ,5   ,5   ,10  },
{12  ,5   ,5   ,5   ,5   ,5   ,5   ,12  },
{-60 ,-100,5   ,5   ,5   ,5   ,-100,-60 },
{100 ,-60 ,12  ,10  ,10  ,12  ,-60 ,100 }
};
class OthelloBoard {
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 8;
    const array<Point, 8> directions{{
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    array<array<int, SIZE>, SIZE> board;
    vector<Point> next_valid_spots;
    array<int, 3> disc_count;
    int cur_player;
    bool done;

private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }

    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {
        if (get_disc(center) != EMPTY)
            return false;
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player))
                    return true;
                p = p + dir;
            }
        }
        return false;
    }
public:
    void flip_discs(Point center) {
        for (Point dir: directions) {
            // Move along the direction while testing.
            Point p = center + dir;
            if (!is_disc_at(p, get_next_player(cur_player)))
                continue;
            std::vector<Point> discs({p});
            p = p + dir;
            while (is_spot_on_board(p) && get_disc(p) != EMPTY) {
                if (is_disc_at(p, cur_player)) {
                    for (Point s: discs) {
                        set_disc(s, cur_player);
                    }
                    disc_count[cur_player] += discs.size();
                    disc_count[get_next_player(cur_player)] -= discs.size();
                    break;
                }
                discs.push_back(p);
                p = p + dir;
            }
        }
    }
    OthelloBoard() {
       for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
        disc_count[EMPTY] = 62;
        disc_count[BLACK] = 1;
        disc_count[WHITE] = 1;
    }

    OthelloBoard(const OthelloBoard &ref){
       for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                board[i][j] = EMPTY;
            }
        }
       for (int i = 0; i < 3 ; i++)
        disc_count[i] = ref.disc_count[i];
    }
    vector<Point> get_valid_spots() const {
        vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    void set_valid_spots(){
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))
                    next_valid_spots.push_back(p);
            }
        }
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
        //disc_count[cur_player]++;
        //disc_count[EMPTY]--;
    }
    int Heuristic(){
        int H = 0;
        int bonus = 0;
        bool corner_occupied[4] = {false};
        //check corner occupied
        for(int i = 0 ; i < 4 ; i++)
            if(board[corner[i].x][corner[i].y] == player)
                corner_occupied[i] = true;
        OthelloBoard opponent = *this;
        opponent.cur_player = 3 - player;
        opponent.set_valid_spots();

        if(disc_count[EMPTY] >= 44){
            bonus = 5;
            //disc_count
            H -= disc_count[player] - disc_count[3 - player];
            //map weight
            for(auto np : next_valid_spots){
            int x = 0 , y = 0;
                for(int i = 0; i ++ ; i < 4){
                    if(corner_occupied[i] && np.x == corner[i].x) x++;
                    if(corner_occupied[i] && np.y == corner[i].y) y++;
                }
            H += 10*(eff[np.x][np.y] + bonus * x + bonus * y);
            }
            //moving potential
            H += (next_valid_spots.size() - opponent.next_valid_spots.size())* 100;
        }else if(disc_count[EMPTY] >= 8){
            bonus = 10;
            //disc_count
            H += disc_count[player] - disc_count[3 - player];
            //map weight
            for(auto np : next_valid_spots){
            int x = 0 , y = 0;
                for(int i = 0; i ++ ; i < 4){
                    if(corner_occupied[i] && np.x == corner[i].x) x++;
                    if(corner_occupied[i] && np.y == corner[i].y) y++;
                }
            H += 10*(eff[np.x][np.y] + bonus * x + bonus * y);
            }
            //moving potential
            H += (next_valid_spots.size() - opponent.next_valid_spots.size())* 200;
        }else{
            bonus = 3;
            //disc_count
            H += (disc_count[player] - disc_count[3 - player]) * 20;
            //map weight
            for(auto np : next_valid_spots){
            int x = 0 , y = 0;
                for(int i = 0; i ++ ; i < 4){
                    if(corner_occupied[i] && np.x == corner[i].x) x++;
                    if(corner_occupied[i] && np.y == corner[i].y) y++;
                }
            H += 10*(eff[np.x][np.y] + bonus * x + bonus * y);
            }
            //moving potential
            H += (next_valid_spots.size() - opponent.next_valid_spots.size())* 300;
            //win move
                if(opponent.next_valid_spots.size() == 0)
                    H += 100000;
        }
        return H;
    }
};
OthelloBoard cur_board;


void read_board(ifstream& fin) {
    fin >> player;
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fin >> cur_board.board[i][j];
        }
    }
}

void read_valid_spots(ifstream& fin) {
    int n_valid_spots;
    fin >> n_valid_spots;
    float x, y;
    for (int i = 0; i < n_valid_spots; i++) {
        fin >> x >> y;
        cur_board.next_valid_spots.push_back({x, y});
    }
}
int alpha_beta(OthelloBoard cur,int depth,int a, int b,int cur_player){
    cur.set_valid_spots();
    if(depth == 0 || cur.next_valid_spots.size() == 0)
        return cur.Heuristic();
    if(cur_player == player){
            int maximum = INT32_MIN;
        for(auto np : cur.next_valid_spots){
            OthelloBoard next = cur;
            next.set_disc(np,cur_player);
            next.flip_discs(np);
            next.disc_count[cur_player]++;
            next.disc_count[0]--;
            maximum = max(maximum,alpha_beta(next,depth - 1, a , b , 3 - cur_player));
            if(b <= a)
                break;
        }
        return maximum;
    }
    else {
        int minimum = INT32_MAX;
        for(auto np : cur.next_valid_spots){
            OthelloBoard next = cur;
            next.set_disc(np,cur_player);
            next.flip_discs(np);
            next.disc_count[cur_player]++;
            next.disc_count[0]--;
            minimum = min(minimum,alpha_beta(next,depth - 1, a , b, 3 - cur_player));
            if(minimum <= a)
                break;
        }
        return minimum;
    }
}
bool check_opponent_corner(OthelloBoard op,int player){
        op.cur_player = 3 - player;
        op.set_valid_spots();
        for(auto np: op.next_valid_spots)
            if(np == Point(0,0) || np == Point(0,7) || np == Point(7,0) || np == Point(7,7))
                return true;
       return false;
}
void write_valid_spot(ofstream& fout) {
        //count discs
        int empty = 0 , black = 0 , white = 0;
        for(int i = 0 ; i < 8 ; i++)
        for(int j = 0 ; j < 8 ; j++){
            if(cur_board.board[i][j] == 0) empty++;
            else if(cur_board.board[i][j] == 1) black++;
            else if(cur_board.board[i][j] == 2) white++;
        }
        cur_board.disc_count[0] = empty;
        cur_board.disc_count[1] = black;
        cur_board.disc_count[2] = white;
        Point p(-1,-1);
        Point bad_idea(0,0);
        int min_num = -2147483647;
        for(auto np : cur_board.next_valid_spots){
            if(np == Point(0,0) || np == Point(0,7) || np == Point(7,0) || np == Point(7,7)){
                p = np;
                break;
            }
            OthelloBoard next = cur_board;
            next.flip_discs(np);
            next.set_disc(np,player);
            int next_a = alpha_beta(next,6,-2147483648,2147483647, 3 - player);
            if(next_a > min_num){
                if(check_opponent_corner(next,player))
                    bad_idea = np;
                else{
                min_num = next_a;
                p = np;
                }
            }
        }
        if(p == Point(-1,-1)) p = bad_idea;
    fout << p.x << " " << p.y << std::endl;
    fout.flush();
}


int main(int, char** argv) {

    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);
    read_valid_spots(fin);
    write_valid_spot(fout);
    fin.close();
    fout.close();
    return 0;
}
